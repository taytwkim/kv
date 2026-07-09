#include "response.hpp"

std::string response_to_string(const Response& response) {
    if (response.status == ResponseStatus::OK) {
        if (response.result.has_value()) {
            return "OK: " + response.result.value();
        }
        
        return "OK";
    }

    else if (response.status == ResponseStatus::NotFound) {
        return "Not Found";
    }
    
    else if (response.status == ResponseStatus::Error) {
        return "Error";
    }
    
    return "Cannot parse response";
}