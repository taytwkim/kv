#include "command.hpp"
#include <sstream>

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
