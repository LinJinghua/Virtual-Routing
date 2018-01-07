#include "route_node.h"
#include "args.h"
#include <string>
#include <chrono>
#include <thread>
#include <iostream>

int main(int argc, char **argv) {
    if (argc < 2) {
       std::cerr << "Specify the node." << std::endl;
       return -1;
    }
    RouteNode node;
    node.SetActionMode(ActionMode::NORMAL);
    node.SetRouteAlgoType(RouteAlgoType::LS);
    // node.SetRouteAlgoType(RouteAlgoType::DV);
    node.Start(std::stoi(argv[1]));
    std::this_thread::sleep_for(std::chrono::seconds(3600));
    node.Stop();
    return 0;
}
