#pragma once
#include <optional>
#include <string>
#include <unordered_map>

class KV_Store {
private:
    std::unordered_map<std::string, std::string> store;

public:
    // const at the end means the function will not modify the object.
    std::optional<std::string> get(const std::string& key) const;
    void set(const std::string& key, const std::string& value);
    bool has(const std::string& key) const;
    bool remove(const std::string& key);
};
