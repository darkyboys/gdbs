/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under the MIT License
 * Checkout the README.md for more information
 */

#ifndef CONSOLE_PRINT_HH
#define CONSOLE_PRINT_HH

#include <string>

namespace ConsolePrint {
    enum class Type {
        Message,
        Log,
        Error,
        Warning,
        Success
    };

    void print(const std::string &text, Type type = Type::Message);
}

#endif
