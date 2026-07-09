#include "command.hpp"
#include "log.hpp"
#include "response.hpp"
#include "store.hpp"
#include <iostream>

int main() {
    const std::string log_file = "store.log";
    KV_Store kv;
    log_replay(log_file, kv);
    std::string line;

    while (true) {
        std::cout << "> ";

        if (!std::getline(std::cin, line)) {
            break;
        }
        
        if (line == "EXIT" || line == "QUIT") {
            break;
        }

        Command command = parse_command(line);
        Response response = apply_command(kv, command);

        if (response.status == ResponseStatus::OK) {
            log_append(log_file, command);
        }

        std::cout << response_to_string(response) << "\n";
    }
    
    return 0;
}