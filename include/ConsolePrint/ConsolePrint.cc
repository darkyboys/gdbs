/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under the MIT License
 * Checkout the README.md for more information
 */

#include "ConsolePrint.hh"
#include <iostream>

namespace ConsolePrint {

    // ANSI color codes
    static const char* WHITE = "\033[37m";
    static const char* LIGHT_GREEN = "\033[92m";
    static const char* ORANGE_RED = "\033[91m";
    static const char* YELLOW = "\033[93m";
    static const char* BLUE = "\033[94m";
    static const char* RESET = "\033[0m";

    void print(const std::string &text, Type type) {
        const char* color;

        switch (type) {
            case Type::Message:
                color = WHITE;
                break;
            case Type::Log:
                color = LIGHT_GREEN;
                break;
            case Type::Error:
                color = ORANGE_RED;
                break;
            case Type::Warning:
                color = YELLOW;
                break;
            case Type::Success:
                color = BLUE;
                break;
            default:
                color = WHITE;
                break;
        }

        std::cout << color << text << RESET << std::endl;
    }
}
