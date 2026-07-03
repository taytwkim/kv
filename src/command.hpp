#pragma once
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

enum class ResponseStatus {
    OK,
    NotFound,
    Error
};

struct Response {
    ResponseStatus status;
    std::optional<std::string> result;
};

Command parse_command(const std::string& request_string);
Response apply_command(KV_Store& store, const Command& command);