#include <string>
#include <iostream>
#include <unordered_map>
#include <optional>

class KV_Store {
private:
    std::unordered_map<std::string, std::string> store;

public:
    void set(const std::string& key, const std::string& value) {
        store[key] = value;
    }
    
    std::optional<std::string> get(const std::string& key) const {
        auto it = store.find(key);

        if (it == store.end()) {
            return std::nullopt;
        }

        return it->second;
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

int main() {
    KV_Store kv;

    kv.set("name", "alice");

    std::cout << kv.has("name") << "\n"; // 1

    auto value = kv.get("name");

    if (value.has_value()) {
        std::cout << value.value() << "\n"; // alice
    }

    kv.set("name", "bob");

    value = kv.get("name");
    
    if (value.has_value()) {
        std::cout << value.value() << "\n"; // bob
    }

    bool removed = kv.remove("name");
    std::cout << removed << "\n";        // 1
    std::cout << kv.has("name") << "\n"; // 0

    return 0;
}