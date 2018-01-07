#include "socket_receiver.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

#define SERVER_PORT 43967
#define BUFFER_SIZE 1024
#define MESSAGE_MAX_SIZE 512

SocketReceiver::SocketReceiver() {
    recv_running_ = false;

    server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket_fd < 0) {
        perror("Create Server Socket Failed!");
        exit(1);
    }
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");

    // bind socket
    if (-1 == (bind(server_socket_fd, (struct sockaddr *)&server_addr,
                    sizeof(server_addr)))) {
        perror("Server Bind Failed:");
        exit(1);
    }
}

SocketReceiver::~SocketReceiver() { close(server_socket_fd); }

void SocketReceiver::StopReceiving() { recv_running_ = false; }

int SocketReceiver::ListenAndReceive(
    std::function<int(std::string)> r_response,
    std::function<int(std::string)> ls_process,
    std::function<int(std::string)> dv_process) {
    recv_running_ = true;
    while (recv_running_) {
        // capture client addr
        struct sockaddr_in client_addr;
        socklen_t client_addr_length = sizeof(client_addr);

        // recv datagram
        char buffer[BUFFER_SIZE];
        int num = recvfrom(server_socket_fd, buffer, BUFFER_SIZE, 0,
                     (struct sockaddr *)&client_addr,
                     &client_addr_length);
        if (num == -1) {
            perror("Receive Data Failed.n");
            // exit(1);
        }

        std::string msg_str(buffer, num);
        switch (msg_str[0]) {
            case 'R':
                // Reachability datagram
                // send a response
                r_response(msg_str);
                break;
            case 'L':
                // LS datagram
                ls_process(msg_str);
                break;
            case 'D':
                // DV datagram
                dv_process(msg_str);
                break;
            default:
                // something error
                // exit?
                break;
        }
    }
    return 0;
}
