#pragma once
#include "response.hpp"
#include "store.hpp"

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

Command parse_command(const std::string& request_string);
Response apply_command(KV_Store& store, const Command& command);