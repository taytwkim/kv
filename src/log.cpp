#include "log.hpp"
#include <fstream>

bool is_loggable_command(const Command& command) {
    // We don't log GET or HAS.
    return command.type == CommandType::SET || command.type == CommandType::REMOVE;
}

void log_append(const std::string& filename, const Command& command) {
    if (!is_loggable_command(command)) {
        return;
    }

    // Creates an empty file if the file is not already there.
    std::ofstream out(filename, std::ios::app);

    if (!out) {
        return;
    }

    if (command.type == CommandType::SET) {
        if (!command.key.has_value() || !command.value.has_value()) {
            return;
        }
        out << "SET "
            << command.key.value()
            << " "
            << command.value.value()
            << "\n";
    }
    else if (command.type == CommandType::REMOVE) {
        if (!command.key.has_value()) {
            return;
        }
        out << "REMOVE "
            << command.key.value()
            << "\n";
    }
}

void log_replay(const std::string& filename, KV_Store& store) {
    std::ifstream in(filename);

    if (!in) {
        return;
    }

    std::string line;

    while (std::getline(in, line)) {
        Command command = parse_command(line);
        apply_command(store, command);
    }
}