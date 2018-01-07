# Readme - Marshall Wang

## Contributions

- Determine the class (hence file) hierarchy
- Implementation
  - class `RouteNode`
    - Wraps the `RouteAlgo`
    - handles the message sender & receiver
    - Multi-threaded
  - `RouteAlgo` (basically)
    - the virtual base class of `RouteLS` & `RouteDV`
  - Unix Socket Sender & Receiver
  - Thread Safe Queue : the message queue of routing
  - `struct LSAdvertisement` (basically)
  - `struct RouteMessage` (basically)

P.S: Also did a very simple test on VPS, just 2 hosts, and everything was ok.

## Others

### Multi-Threaded

There are 3 threads running in `RouteNode`, which are `MessageSender`, `MessageReceiver`, and `WaitForCommands`.

`MessageSender` checks the message (to be sent) queue consecutively, and sends message to `dest_ip` during the entries in queue;

`MessageReceiver` binds a socket to a port (43967, sorry to [Clearlove](https://lol.gamepedia.com/Clearlove7), but I always know he is one of the best players of LPL), keeps listening and receiving packets from other nodes.

The original purpose of `WaitForCommands` is for receiving the input from `stdin` and hence it's able to simualte some physical changes (such as cable-unplugged, temperarily shutdown) during the runtime without hitting `Ctrl+C`. But then we found it more convenient to make the `RouteAlgo` print out the `route_table` periodically (and each time when it recv a packet) (Thanks to [Linjinghua](https://github.com/LinJinghua/Virtual-Routing)). So we didn't provide command support here, and currently the program will run for a certain time(3600s, can be modified `src/main.cpp`) and stop.

### Thread-Safe data fields

Generally two : the message queue in `RouteNode`, and the route table in `RouteAlgo`.

**Message queue:** there are 2 scenarios that a node should send messages: Periodically distributing its own table, and broadcasting the received packet (from a neighbor) to the other neighbors. As we want all messages to be sent and received in one port without competition, we pushed all the generated & forwarded messages to a **thread-safe queue**, then the outer `MessageSender` checks and sends them.

**Route Algorithm:** Due to the rule of sending messages, one node may receive many packets continuously in a consecutive time. In order to avoiding some potential competition, we also add a mutex on the **Route Table**. But I hadn't know much details about that when I was writing this doc, maybe you can find more details in my teammates' doc.

## Thanks

Many thanks to all of my teammates, who helped me quite a lot on everything (docker test, the bottom-level design & implement, debug, etc) in this project.
