#include "store.hpp"

std::optional<std::string> KV_Store::get(const std::string& key) const {
    auto it = store.find(key);

    if (it == store.end()) {
        return std::nullopt;
    }

    return it->second;
}

void KV_Store::set(const std::string& key, const std::string& value) {
    store[key] = value;
}

bool KV_Store::has(const std::string& key) const {
    return store.find(key) != store.end();
}
    
bool KV_Store::remove(const std::string& key) {
    auto it = store.find(key);

    if (it == store.end()) {
        return false;
    }
    
    store.erase(it);
    
    return true;
}
