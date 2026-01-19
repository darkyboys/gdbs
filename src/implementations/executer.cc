/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under the MIT License
 * Checkout the README.md for more information
 */

// This file contains the fs utils of the project

// Header
// Local Libraries
#include <components/core.hh>
#include <components/fs-utils.hh>
#include <components/executer.hh>
#include <h699/h699.hh>
#include <ConsolePrint/ConsolePrint.hh>

namespace gdbs {
    std::vector <std::string> executer(std::vector <gdbs::core_file_t> tokens, int thread_limit, bool show_command){
        int active_threads = 0;
        // This part is taken from the core.cc so this must be updated with it as well
        const std::string CACHE_DIRECTORY = ".gdbs-cache/"; // The directory where the cache files will be created
        const std::string DS_FILE = CACHE_DIRECTORY + "ds.h699"; // The data set file path
        const std::string IPC_FILE = CACHE_DIRECTORY + "ipc.file"; // The inter process cmmunication file path
        // The part from the core.cc ends here

        std::vector <std::string> error_files = {};

        for (int i = 0;i < tokens.size();i++){
            if (active_threads == thread_limit){
                i -= 1;
                continue;
            }
            if (active_threads < thread_limit){
                // std::cout << "i:"<<i<<"\n";
                active_threads += 1; // This will actually run before the thread
                std::thread([&error_files, &show_command, &i, &active_threads, &tokens](){
                    ConsolePrint::print ("Building " + tokens[i].output + ", Compiling " + tokens[i].filename + ", Progress [" + std::to_string((float(i) / float((tokens.size()-1.0f == 0) ? 1 : tokens.size()-1.0f))*100) + "%]\n", ConsolePrint::Type::Log);
                    if (show_command) ConsolePrint::print ("Commands: " + tokens[i].command, ConsolePrint::Type::Message);
                    if (std::system (tokens[i].command.c_str()) != 0) error_files.push_back (tokens[i].filename);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    active_threads -= 1;
                }).detach();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        while (active_threads > 0) {}

        return error_files;
    }
}
