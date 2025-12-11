/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under the MIT License
 * Checkout the README.md for more information
 */

// This file contains the core of the project

// Header
// STL
#ifndef GDBS_CORE_HH
#define GDBS_CORE_HH 1

#include <string>
#include <thread>
#include <vector>
#include <filesystem>
#include <fstream>

namespace gdbs{
    int core(std::string file, std::vector <std::string> cli_args, int threads, bool allowed_incremental_build = true, bool show_command = false); // Takes the file's path as a full file.
    struct core_file_t { // A Type For the communication in between the build system's core library and the execution managing library
        std::string command;
        std::string output;
        std::string filename;
    };
}

#endif