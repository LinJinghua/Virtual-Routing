# Virtual-Routing

Application-layer routing program, using UDP

## Introduction

- self-organized routing
  - Select a virtual topo for members’ computers
  - Build virtual connection between computers according to the virtual topo;
  - Each computer acts as both client and router.
  - Each computer exchanges and updates routing table periodically.
  - A computer can send message to other computers

- centralized routing
  - Like the above self-organized routing
  - Controller determines and distributes routing policy (routing table) to each member

- can choose different topo(the 2 above) & different routing algorithms
  - LS
  - DV

## Coding Doc

All PC Nodes will run as a `Node`. Before a `Node.Start()`, the `RouteAlgoType` and `ActionMode` MUST be specified. OR it will panic out and exit.

```cpp
Node n;
n.SetRouteAlgoType(RouteAlgoType::LS);
n.SetActionMode(ActionMode::NORMAL);
n.Start(std::stoi(argv[1])); // specify the node_number_in_topo in cli: 0, 1, 2, ...
```

Use Multi-Threaded to split the `MessageSender` & `MessageReceiver`, and a thread-safe queue to deal with competition.

Also, the data in `RouteAlgo` (`connectivity_table`, `route_table`, etc) should also be thread-safe to avoid potential competition in runtime.

The `SocketSender` & `SocketReceiver` should be wrapped out in single files, for the convenience when switch between `Windows` & `*nix`. (The socket files for Windows are in `winsocket` folder).

The implementation details can be found at each group member's Readmes.

### Classes Hierarchy

- Args [Singleton] : store the args
- Node : Abstract of the Nodes in routing
- RouteAlgo : virtual base class for the Routing-Algorithm
  - RouteLS : Link-State Algorithm
  - RouteDV : Distance-Vector Algorithm
- struct RouteMessage: Reachability Message
- struct LSAdvertisement & DVAdvertisement : datagram for LS or DV
- Socket Sender & Receiver: wraps the unix/windows socket
- Thread-Safe Queue: a queue of message to be sent to other nodes

The `RouteLS` or `RouteDV` class will be initialized **automatically** inside a `Node` as soon as `SetRouteAlgoType()` is called. So DON'T init a `RouteAlgo` manually.

## Setup & Deploy

First clone the repo.

```bash
git clone https://github.com/MarshallW906/Virtual-Routing
cd Virtual-Routing
```

Then edit the `src/args.cpp`, specify all the nodes' IP and the topo, like the following. The topo is a directed graph, specified by edges. The `(u, v, w)` is like `(pa[i], pb[i], cost[i])`.

Make sure you have all the nodes' `src/args.cpp` modified, or it won't work as normal.

```cpp
// vim src/args.cpp

// edit the following assignment lines
nodes_number_ = 5;
node_to_ip_.resize(nodes_number_);
node_to_ip_[0] = "192.168.1.110";
node_to_ip_[1] = "192.168.1.111";
node_to_ip_[2] = "192.168.1.112";
node_to_ip_[3] = "192.168.1.113";
node_to_ip_[4] = "192.168.1.116";

// ... some code

NodeType pa[] = {0, 0, 0, 1, 1, 3};
NodeType pb[] = {2, 3, 4, 2, 3, 4};
NodeType cost[] = {6, 5, 22, 10, 4, 3};
```

Then `Make` and run. You will need to specify the node number.

```shell
make
# wait for the Makefile progress
./bin/routing [the node_index_in_topo]
```

P.S.: We completed out test on `docker`. Many thanks to [FideoJ](https://github.com/MarshallW906/Virtual-Routing), who did this for us and make things much easier.

## Runtime

It will keep running for `3600s` (can be modified in `src/main.cpp`), as we didn't provide a cli-command in runtime. And during the runtime, each node will print the `reachability table` & the `route table` & `time table` periodically. The tables will update automatically when it detect the change of this "virtual routing network`.