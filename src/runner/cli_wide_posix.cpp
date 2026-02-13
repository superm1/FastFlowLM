/*
*  Copyright (c) 2025 by Contributors
*  \file cli_wide_posix.cpp
*  \brief CLI interactive input implementation for non-Windows platforms
*  \author FastFlowLM Team
*  \date 2026-02-03
*  \version 0.9.24
*/
#include "cli_wide.hpp"
#include <iostream>
#include <cstdlib>
#ifdef FASTFLOWLM_USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#ifndef _WIN32

CLIWide::CLIWide() = default;

CLIWide::~CLIWide() = default;

std::string CLIWide::get_interactive_input() {
#ifdef FASTFLOWLM_USE_READLINE
    char* line = readline(">>> ");
    if (!line) {
        return std::string();
    }
    std::string input(line);
    free(line);
    if (!input.empty()) {
        add_history(input.c_str());
    }
    return input;
#else
    std::string input;
    std::cout << ">>> ";
    std::getline(std::cin, input);
    return input;
#endif
}

void CLIWide::add_to_history(const std::string& command) {
    if (command.empty()) return;
    if (!command_history.empty() && command_history.back() == command) return;

    command_history.push_back(command);
    if (command_history.size() > max_history_size) {
        command_history.pop_front();
    }
    history_index = static_cast<int>(command_history.size());
}

#endif
