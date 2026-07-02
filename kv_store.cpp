#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <optional>

class KV_Store {
private:
    std::unordered_map<std::string, std::string> store;

public:
    std::optional<std::string> get(const std::string& key) const {
        auto it = store.find(key);

        if (it == store.end()) {
            return std::nullopt;
        }

        return it->second;
    }

    void set(const std::string& key, const std::string& value) {
        store[key] = value;
    }
    
    bool has(const std::string& key) const {
        return store.find(key) != store.end();
    }
    
    bool remove(const std::string& key) {
        auto it = store.find(key);

        if (it == store.end()) {
            return false;
        }

        store.erase(it);
        return true;
    }
};

enum class CommandType {
    GET,
    SET,
    HAS,
    REMOVE,
    INVALID
};

struct Command {
    CommandType type;
    std::optional<std::string> key;
    std::optional<std::string> value;
};

enum class ResponseStatus {
    OK,
    NotFound,
    Error
};

struct Response {
    ResponseStatus status;
    std::optional<std::string> result;
};

Command parse_command(const std::string& request_string) {
    std::istringstream iss(request_string);
    std::string command;
    std::string key;
    std::string value;

    if (!(iss >> command)) {
        return {CommandType::INVALID, std::nullopt, std::nullopt};
    }
    
    if (command == "GET") {
        if (!(iss >> key)) {
            return {CommandType::INVALID, std::nullopt, std::nullopt};
        }

        return {CommandType::GET, key, std::nullopt};
    }
    else if (command == "SET") {
        if (!(iss >> key) || !(iss >> value)) {
            return {CommandType::INVALID, std::nullopt, std::nullopt};
        }

        return {CommandType::SET, key, value};
    }
    else if (command == "HAS") {
        if (!(iss >> key)) {
            return {CommandType::INVALID, std::nullopt, std::nullopt};
        }

        return {CommandType::HAS, key, std::nullopt};
    }
    else if (command == "REMOVE") {
        if (!(iss >> key)) {
            return {CommandType::INVALID, std::nullopt, std::nullopt};
        }

        return {CommandType::REMOVE, key, std::nullopt};
    }

    return {CommandType::INVALID, std::nullopt, std::nullopt};
}

Response apply_command(KV_Store& store, const Command& command) {
    if (command.type == CommandType::INVALID) {
        return {ResponseStatus::Error, std::nullopt};
    }
    if (command.type == CommandType::GET) {
        if (!command.key.has_value()) {
            return {ResponseStatus::Error, std::nullopt};
        }

        std::optional<std::string> value = store.get(command.key.value());

        if (!value.has_value()) {
            return {ResponseStatus::NotFound, std::nullopt};
        }

        return {ResponseStatus::OK, value.value()}; 
    }
    else if (command.type == CommandType::SET) {
        if (!command.key.has_value() || !command.value.has_value()) {
            return {ResponseStatus::Error, std::nullopt};
        }

        store.set(command.key.value(), command.value.value());
        return {ResponseStatus::OK, std::nullopt}; 
    }
    else if (command.type == CommandType::HAS) {
        if (!command.key.has_value()) {
            return {ResponseStatus::Error, std::nullopt};
        }

        bool has = store.has(command.key.value());

        if (has) {
            return {ResponseStatus::OK, "true"};
        }

        return {ResponseStatus::OK, "false"};
    }
    else if (command.type == CommandType::REMOVE) {
        if (!command.key.has_value()) {
            return {ResponseStatus::Error, std::nullopt};
        }

        bool removed = store.remove(command.key.value());

        if (removed) {
            return {ResponseStatus::OK, "true"};
        }

        return {ResponseStatus::NotFound, "false"};
    }
    return {ResponseStatus::Error, std::nullopt};
}

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