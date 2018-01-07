#include "args.h"

#include <vector>

std::shared_ptr<Args> Args::m_args = nullptr;
const Args::CostType Args::INFINITE = 0x3f3f3f3f;

Args::Args() {
    nodes_number_ = 5;
    node_to_ip_.resize(nodes_number_);
    node_to_ip_[0] = "192.168.1.110";
    node_to_ip_[1] = "192.168.1.111";
    node_to_ip_[2] = "192.168.1.112";
    node_to_ip_[3] = "192.168.1.113";
    node_to_ip_[4] = "192.168.1.116";

    for (std::size_t i = 0; i < node_to_ip_.size(); i++)
        ip_to_node_[node_to_ip_[i]] = i;

    init_topo_table_ = std::vector<DV>(nodes_number_, DV(nodes_number_, INFINITE));

    for (std::size_t i = 0; i < init_topo_table_.size(); i++)
        init_topo_table_[i][i] = 0;

    NodeType pa[] = {0, 0, 0, 1, 1, 3};
    NodeType pb[] = {2, 3, 4, 2, 3, 4};
    NodeType cost[] = {6, 5, 22, 10, 4, 3};

    for (int i = sizeof(pa)/sizeof(pa[0]); i--; ) {
        init_topo_table_[pb[i]][pa[i]] = init_topo_table_[pa[i]][pb[i]] = cost[i];
    }
}
Args::~Args() { Args::m_args = nullptr; }

std::shared_ptr<Args> Args::GetInstance(void) {
    if (m_args == nullptr) m_args = std::shared_ptr<Args>(new Args());
    return m_args;
}

std::size_t Args::GetNodeNumber(void) const {
    return nodes_number_;
}

Args::NodeType Args::GetNode(const Args::IpType & ip) const {
    return ip_to_node_.at(ip);
}

Args::IpType Args::GetIp(const Args::NodeType node) const {
    return node_to_ip_.at(node);
}

Args::DV Args::GetInterfaces(const Args::IpType & ip) const {
    return init_topo_table_.at(ip_to_node_.at(ip));
}

Args::DV Args::GetInterfaces(const Args::NodeType node) const {
    return init_topo_table_.at(node);
}
