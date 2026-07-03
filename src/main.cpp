#include <iostream>
#include "store.hpp"
#include "command.hpp"

int main() {
    KV_Store kv;
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