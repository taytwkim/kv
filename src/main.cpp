#include "store.hpp"
#include "command.hpp"
#include "log.hpp"
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
            std::cout << "OK";
            
            if (response.result.has_value()) {
                std::cout << ": " << response.result.value();
            }
            std::cout << "\n";
        }
        else if (response.status == ResponseStatus::NotFound) {
            std::cout << "Not Found\n";
        }
        else if (response.status == ResponseStatus::Error) {
            std::cout << "Error\n";
        }
    }
    return 0;
}