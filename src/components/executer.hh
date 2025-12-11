/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under the MIT License
 * Checkout the README.md for more information
 */

// This file contains the fs utils of the project

// Header
// STL
#ifndef GDBS_EXECUTER_HH
#define GDBS_EXECUTER_HH
#include <chrono>
#include <cinttypes>
#include <thread>
#include <vector>
#include <filesystem>
#include <string>

// Local Libraries
#include <components/core.hh>
#include <components/fs-utils.hh>

namespace gdbs {
    void executer(std::vector <gdbs::core_file_t> tokens, int thread_limit = std::thread::hardware_concurrency(), bool show_command = false);
}
#endif