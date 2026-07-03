#pragma once

#include "store.hpp"
#include "command.hpp"

bool is_loggable_command(const Command& command);
void log_append(const std::string& filename, const Command& command);
void log_replay(const std::string& filename, KV_Store& store);