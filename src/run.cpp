#include <iostream>
#include "server.hpp"

int main (int argc, char* argv[]) {
    int port = 4000;
    std::string log_file = "store.log";

    if (argc >= 2) {
        port = std::atoi(argv[1]);
    }

    if (argc >= 3) {
        log_file = argv[2];
    }
    
    run(port, log_file);
    return 0;
}