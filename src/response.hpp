#pragma once
#include <optional>
#include <string>

enum class ResponseStatus {
    OK,
    NotFound,
    Error
};

struct Response {
    ResponseStatus status;
    std::optional<std::string> result;
};

std::string response_to_string(const Response& response);