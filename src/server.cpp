#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <functional> // std::ref
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "command.hpp"
#include "log.hpp"
#include "response.hpp"
#include "server.hpp"
#include "store.hpp"

// Using anonymous namespace to make this function private (similar to static).
namespace {
    std::string handle_request(
        KV_Store& store,
        const std::string& log_file, 
        const std::string& request
    ) {
        Command command = parse_command(request);
        Response response = apply_command(store, command);

        if (response.status == ResponseStatus::OK) {
            log_append(log_file, command);
        }

        return response_to_string(response) + "\n";
    }
    
    void handle_client(
        int client_fd,
        KV_Store& store,
        const std::string& log_file,
        std::mutex& mutex
    ) {
        std::cout << "Client connected\n";
        char buffer[1024];
        std::string pending;

        /*
         * pending stores bytes received from this client that have not
         * been turned into complete commands yet.
         *
         * TCP is a byte stream, not a message protocol. One recv() call may
         * give us part of a command, exactly one command, or multiple commands.
         *
         * We use '\n' as the end of one command.
         */
        while (true) {
            /*
             * recv() copies bytes from the OS receive queue into our buffer.
             *
             * recv() is blocking by default. If there is currently no data available,
             * it waits until data arrives or the connection is closed.
             *
             * Return value:
             * - > 0: number of bytes read
             * - == 0: the peer closed the connection
             * - -1: an error occurred
             */
            ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);

            if (bytes_read <= 0) {
                break;
            }

            // Add the newly received bytes to pending.
            // We do not assume that this is one complete request.
            pending.append(buffer, bytes_read);

            // Process every complete command currently in pending.
            // A complete command ends with '\n'.
            std::size_t newline_pos;

            while ((newline_pos = pending.find('\n')) != std::string::npos) {
                std::string request = pending.substr(0, newline_pos);
                // Remove the processed command, including the '\n', from pending.
                pending.erase(0, newline_pos + 1);
                std::string response;

                {
                    // Only one thread will enter this block at a time.
                    std::lock_guard<std::mutex> lock(mutex);
                    response = handle_request(store, log_file, request);
                    // Unlocked at the end of the block.
                }
                
                send(client_fd, response.c_str(), response.size(), 0);
            }
        }
        
        close(client_fd);
        std::cout << "Client disconnected\n";
    }
}

void run(int port, const std::string& log_file) {
    KV_Store store;
    log_replay(log_file, store);
    std::mutex mutex;

    /*
     * 1. Create a socket
     *
     * A socket is an interface provided by the OS that abstracts away
     * the lower-level networking stack. It's used when a process wants
     * to communicate over a protocol such as TCP or UDP.
     * 
     * - domain = AF_INET: Use IPv4.
     * - type = SOCK_STREAM: Use a stream-oriented socket type.
     * - protocol = 0: Let the OS choose the default protocol for this address family
     *      and socket type. For AF_INET + SOCK_STREAM, this means TCP.
     */
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_fd == -1) {
        std::cerr << "Failed to create socket\n";
        return;
    }

    /*
     * 2. Set socket options
     * 
     * Here, we are enabling the SO_REUSEADDR option.
     * 
     * Usually, only one socket can use a local address + port at a time.
     * If the server fails and restarts quickly, the OS may still think the
     * address + port is in-use. SO_REUSEADDR makes it
     * easier for the server to use the same address + port again.
     * 
     * - fd: File descriptor for the socket.
     * - SOL_SOCKET: We are setting an option on the socket itself.
     *      Other levels, such as IPPROTO_TCP or IPPROTO_IP, are used for
     *      TCP-specific or IP-specific options.
     * - SO_REUSEADDR: The option we are enabling. We pass the address of
     *      opt, whose value is 1, to turn this option on.
     */
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /*
     * 3. bind() is where we say the socket will be attached to this address + port.
     * 
     * We built a sockaddr_in, which is specific to IPv4.
     * sockaddr_in is easier for us to fill in because it has clear fields
     * for the address family, IP address, and port. But bind() expects a generic sockaddr*, 
     * so we cast sockaddr_in* to sockaddr*.
     */
    sockaddr_in server_addr{};  // struct that contains address + port the socket will use.
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        std::cerr << "Failed to bind socket\n";
        close(server_fd);
        return;
    }
    
    // 4. The socket now listens for client requests.
    if (listen(server_fd, 10) == -1) {
        std::cerr << "Failed to listen on socket\n";
        close(server_fd);
        return;
    }

    std::cout << "Listening on port " << port << "...\n";

    while (true) {
        // Create an empty IPv4 address struct and pass it to accept()
        // so the OS can fill it with client address info.
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        // 5. Accept one client connection.
        int client_fd = accept(
            server_fd,
            reinterpret_cast<sockaddr*>(&client_addr),
            &client_len
        );

        if (client_fd == -1) {
            std::cerr << "Failed to accept client\n";
            continue;
        }

        /*
         * Spawn one thread for each client connection.
         *
         * std::thread copies its arguments by default. Even if handle_client()
         * takes KV_Store& or std::mutex&, passing store or mutex directly would
         * make std::thread try to copy them.
         *
         * We use std::ref() to explicitly pass references to the shared KV store
         * and mutex, so all client threads use the same objects.
         *
         * We call detach() so the main thread does not wait for the client
         * thread to finish. After spawning the client thread, the main thread
         * immediately goes back to accept() and waits for the next client.
         *
         * If we used join() here instead, the main thread would wait until the
         * current client disconnects before accepting another client. That would
         * make client connections effectively serialized.
         */
        std::thread(
            handle_client,
            client_fd,
            std::ref(store),
            log_file,
            std::ref(mutex)
        ).detach();
    }
    // close(server_fd);
}