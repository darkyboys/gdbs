/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under the MIT License
 * Checkout the README.md for more information
 */

// This file contains the fs utils of the project

// Header
// STL
#include <filesystem>
#include <string>

// Local
#include <components/fs-utils.hh>

namespace gdbs{
    // Returns 1 if both files have identical last_write_time, otherwise 0
    int compare_timestamps(const std::string& file1, const std::string& file2) {
        try {
            auto t1 = fs::last_write_time(file1);
            auto t2 = fs::last_write_time(file2);
            return t1 == t2 ? 1 : 0;
        } catch (const fs::filesystem_error&) {
            return 0; // If something goes wrong, treat as mismatch
        }
    }

    // Copies last_write_time from file1 to file2
    void copy_timestamps(const std::string& file1, const std::string& file2) {
        try {
            auto t = fs::last_write_time(file1);
            fs::last_write_time(file2, t);
        } catch (const fs::filesystem_error&) {
            // Let your app decide what to do; swallow for now
        }
    }

    // Converts the last_write_time to a std::string
    std::string timestamp_to_string(const fs::file_time_type& ft) {
        // Convert file_time_type to system_clock::time_point
        using namespace std::chrono;
    
        // Difference between file_time_type clock and system_clock
        auto system_now = system_clock::now();
        auto file_now   = fs::file_time_type::clock::now();
    
        auto diff = system_now.time_since_epoch() - file_now.time_since_epoch();
    
        auto sctp = system_clock::time_point(ft.time_since_epoch() + diff);
    
        std::time_t tt = system_clock::to_time_t(sctp);
    
        std::tm tm {};
        localtime_r(&tt, &tm);
    
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
        return oss.str();
    }
}

