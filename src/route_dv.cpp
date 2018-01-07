#include "route_dv.h"
#include "route_message.h"

#include <queue>
#include <string>
#include <cstring>
#include <utility>

// init DV.
// init route table.
void RouteDV::SetNode(const NodeType node) {
    node_ = node;
    default_route_ = UNREACHABLE;
    auto args = Args::GetInstance();
    DV cost(args->GetInterfaces(node));
    intfes_.Set(cost);

    route_table_.resize(cost.size());
    dv_.Init(cost.size());
    dv_.Set(node_, cost);
    for (std::size_t i = 0; i < cost.size(); i++)
        if (cost[i] == UNREACHABLE) {
            route_table_[i] = default_route_;
        } else {
            route_table_[i] = i;
            if (i != static_cast<std::size_t>(node_))
                recv_time_.SetTime(i);
        }

    GenerateRouteMsg();
}

void RouteDV::CheckNode(void) {
    std::vector<NodeType> down(recv_time_.CheckTime());
    if (down.size()) {
        for (std::size_t i = 0; i < down.size(); i++)
            intfes_.Set(down[i], UNREACHABLE);
        dv_.Update(node_, intfes_.Get(), route_table_);
    }
    recv_time_.Print();
    intfes_.Print();
    // graph_.Print();
    RoutePrint();
    GenerateRouteMsg();
}

int RouteDV::UpdateRouteMsg(const std::string& raw_msg_str) {
    DVAdvertisement msg(raw_msg_str);
    auto args = Args::GetInstance();
    NodeType from = args->GetNode(msg.generated_from_ip);
    recv_time_.SetTime(from);

    DV dv(msg.dv.size(), UNREACHABLE);
    for (const auto& n : msg.dv)
        dv[args->GetNode(n.first)] = n.second;
    intfes_.Set(from, args->GetInterfaces(node_)[from]);

    dv_.Set(from, dv);
    if (dv_.Update(node_, intfes_.Get(), route_table_))
        GenerateRouteMsg();

    std::fprintf(stdout, "\nRecv From %s\n", msg.generated_from_ip.c_str());
    return 0;
}

void RouteDV::GenerateRouteMsg() {
    // pair.first is dest_ip
    // pair.second is the message

    // Generate all the Route Message that will be sent to others in one period
    auto args = Args::GetInstance();
    DVAdvertisement ad;
    ad.generated_from_ip = args->GetIp(node_);
    DV cost(dv_.Get(node_));
    ad.dv.reserve(cost.size());
    for (std::size_t i = 0; i < cost.size(); i++)
        ad.dv.push_back(std::make_pair(args->GetIp(i),
            static_cast<DVAdvertisement::CostFile>(cost[i])));

    std::string ad_str(ad.GetSerializedMsg());
    for (std::size_t i = 0; i < cost.size(); i++)
        if (cost[i] != UNREACHABLE && i != static_cast<std::size_t>(node_))
            msg_to_send_.push(std::make_pair(args->GetIp(i), ad_str));
}
