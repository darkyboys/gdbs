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
    void executer(std::vector <gdbs::core_file_t> tokens, int thread_limit, bool show_command){
        int active_threads = 0;
        bool errors = false; // if errors then terminate the program
        // This part is taken from the core.cc so this must be updated with it as well
        const std::string CACHE_DIRECTORY = ".gdbs-cache/"; // The directory where the cache files will be created
        const std::string DS_FILE = CACHE_DIRECTORY + "ds.h699"; // The data set file path
        const std::string IPC_FILE = CACHE_DIRECTORY + "ipc.file"; // The inter process cmmunication file path
        // The part from the core.cc ends here

        HELL6_99MO h699_ds_file(DS_FILE); // Read the dataset as a h699 file
        h699_ds_file.Parse(); // Load the file into the memory
        for (int i = 0;i < tokens.size();i++){
            if (active_threads == thread_limit){
                i -= 1;
                continue;
            }
            if (active_threads < thread_limit){
                // std::cout << "i:"<<i<<"\n";
                active_threads += 1; // This will actually run before the thread
                std::thread([&errors, &show_command, &i, &h699_ds_file, &DS_FILE, &active_threads, &tokens](){
                    ConsolePrint::print ("Building " + tokens[i].output + ", Compiling " + tokens[i].filename + ", Progress [" + std::to_string((float(i) / float((tokens.size()-1.0f == 0) ? 1 : tokens.size()-1.0f))*100) + "%]", ConsolePrint::Type::Log);
                    if (show_command) ConsolePrint::print ("Commands: " + tokens[i].command, ConsolePrint::Type::Message);
                    std::system (std::string ("rm -rf " + tokens[i].output).c_str()); // Delete the file if it existed previously just for always making a neat dataset
                    if (std::system (tokens[i].command.c_str()) != 0) errors = true;
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    active_threads -= 1;
                }).detach();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        if (errors) std::exit (3);

        // Now we will set the time stamps of all the binaries
        for (gdbs::core_file_t token : tokens){
            // We will use a while loop to wait until the files physically exists to get rid of any file system errors
            while (not(std::filesystem::exists(token.output) and not std::filesystem::is_directory(token.output)));

            // Registering / Updating the dataset with te binary files timestamps so that we can deal with .o or .so files in advanced incremental builds
            HELL6_99MO_TYPE timestamp = h699_ds_file.get(token.output);
            if (timestamp.type == "string"){
                h699_ds_file.set(token.output, gdbs::timestamp_to_string(std::filesystem::last_write_time(token.output)));
            }
            else {
                h699_ds_file.new_key (token.output, "string");
                h699_ds_file.set(token.output, gdbs::timestamp_to_string(std::filesystem::last_write_time(token.output)));
            }
            h699_ds_file.write(DS_FILE);
        }
    }
}