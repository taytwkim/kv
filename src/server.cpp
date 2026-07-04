#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

#include "command.hpp"
#include "log.hpp"
#include "response.hpp"
#include "server.hpp"
#include "store.hpp"

// Using anonymous namespace to make this function private.
namespace {
    std::string handle_request(KV_Store& store, const std::string& log_file, const std::string& request) {
        Command command = parse_command(request);
        Response response = apply_command(store, command);

        if (response.status == ResponseStatus::OK) {
            log_append(log_file, command);
        }
        return response_to_string(response) + "\n";
    }
}

void run(int port, const std::string& log_file) {
    KV_Store store;
    log_replay(log_file, store);

    /*
     * 1. Create a socket
     *
     * A socket is an OS-level communication endpoint used for network I/O.
     * 
     * - domain = AF_INET: Use IPv4.
     * - type = SOCK_STREAM: Use a stream-oriented socket type.
     * - protocol = 0: Let the OS choose the default protocol for this address family
     *      and socket type. For AF_INET + SOCK_STREAM, this means TCP.
     */
    int fd = socket(AF_INET, SOCK_STREAM, 0);    // int socket(int domain, int type, int protocol)
    
    if (fd == -1) {
        std::cerr << "Failed to create socket\n";
        return;
    }

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        std::cerr << "Failed to bind socket\n";
        close(fd);
        return;
    }

    if (listen(fd, 10) == -1) {
        std::cerr << "Failed to listen on socket\n";
        close(fd);
        return;
    }

    std::cout << "Listening on port " << port << "...\n";

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(
            fd,
            reinterpret_cast<sockaddr*>(&client_addr),
            &client_len
        );

        if (client_fd == -1) {
            std::cerr << "Failed to accept client\n";
            continue;
        }

        std::cout << "Client connected\n";

        char buffer[1024];

        while (true) {
            std::memset(buffer, 0, sizeof(buffer));
            ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

            if (bytes_read <= 0) {
                break;
            }
            std::string request(buffer);
            std::string response = handle_request(store, log_file, request);
            send(client_fd, response.c_str(), response.size(), 0);
        }
        close(client_fd);
        std::cout << "Client disconnected\n";
    }
    close(fd);
}