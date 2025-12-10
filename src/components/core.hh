/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under the MIT License
 * Checkout the README.md for more information
 */

// This file contains the core of the project

// Header
// STL
#ifndef CORE_HH
#define CORE_HH

#include <string>

namespace gdbs{
    int core(std::string file, bool allowed_incremental_build = true); // Takes the file's path as a full file.
    struct core_file_t { // A Type For the communication in between the build system's core library and the execution managing library
        std::string command;
        std::string output;
        std::string filename;
    };
}

#endif