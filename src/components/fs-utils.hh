/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under the MIT License
 * Checkout the README.md for more information
 */

// This file contains the fs utils of the project

// Header
// STL

#ifndef FS_UTILS_HH
#define FS_UTILS_HH

#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace gdbs{
    namespace fs = std::filesystem;

    // Returns 1 if both files have identical last_write_time, otherwise 0
    int compare_timestamps(const std::string& file1, const std::string& file2);

    // Copies last_write_time from file1 to file2
    void copy_timestamps(const std::string& file1, const std::string& file2);

    // Converts the last_write_time to a std::string
    std::string timestamp_to_string(const fs::file_time_type& ft);

}

#endif