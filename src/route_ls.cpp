#include "route_ls.h"
#include "route_message.h"

#include <queue>
#include <string>
#include <cstring>
#include <utility>

// init DV.
// init route table.
void RouteLS::SetNode(const NodeType node) {
    node_ = node;
    default_route_ = UNREACHABLE;
    auto args = Args::GetInstance();
    DV cost(args->GetInterfaces(node));
    intfes_.Set(cost);
    route_table_.resize(cost.size());

    for (std::size_t i = 0; i < cost.size(); i++)
        if (cost[i] == UNREACHABLE) {
            route_table_[i] = default_route_;
        } else {
            if (i != static_cast<std::size_t>(node_)) {
                graph_.AddEdge(node_, i, cost[i]);
                recv_time_.SetTime(i);
            }
            route_table_[i] = i;
        }

    GenerateRouteMsg();
}

void RouteLS::CheckNode(void) {
    std::vector<NodeType> down(recv_time_.CheckTime());
    if (down.size()) {
        for (std::size_t i = 0; i < down.size(); i++) {
            graph_.DelNode(down[i]);
            intfes_.Set(down[i], UNREACHABLE);
        }
        route_table_ = graph_.Dijkstra(node_);
    }
    recv_time_.Print();
    intfes_.Print();
    // graph_.Print();
    RoutePrint();
    GenerateRouteMsg();
}

int RouteLS::UpdateRouteMsg(const std::string& raw_msg_str) {
    auto args = Args::GetInstance();
    LSAdvertisement msg(raw_msg_str);
    int from = args->GetNode(msg.generated_from_ip);
    recv_time_.SetTime(from);

    for (const auto& n : msg.dv)
        graph_.AddEdge(from, args->GetNode(n.first), n.second);
    intfes_.Set(from, graph_.GetCost(from, node_));
    route_table_ = graph_.Dijkstra(node_);

    std::fprintf(stdout, "\nRecv From %s\n", msg.generated_from_ip.c_str());
    return 0;
}

void RouteLS::GenerateRouteMsg() {
    // pair.first is dest_ip
    // pair.second is the message

    // Generate all the Route Message that will be sent to others in one period
    auto args = Args::GetInstance();
    LSAdvertisement ad;
    ad.generated_from_ip = args->GetIp(node_);
    DV cost(intfes_.Get());
    for (std::size_t i = 0; i < cost.size(); i++)
        if (cost[i] != UNREACHABLE && i != static_cast<std::size_t>(node_))
            ad.dv.push_back(std::make_pair(args->GetIp(i),
                static_cast<LSAdvertisement::CostFile>(cost[i])));

    std::string ad_str(ad.GetSerializedMsg());
    for (std::size_t i = 0; i < cost.size(); i++)
        if (i != static_cast<std::size_t>(node_))
            msg_to_send_.push(std::make_pair(args->GetIp(i), ad_str));
}
