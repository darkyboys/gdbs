/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under the MIT License
 * Checkout the README.md for more information
 */

// This file contains the core of the project

// Header
// STL
#include <vector>
#include <filesystem>
#include <fstream>

// Local
#include <components/core.hh>
#include <components/fs-utils.hh>
#include <components/executer.hh>
#include <h699/h699.hh>
#include <ConsolePrint/ConsolePrint.hh>

namespace gdbs{
    int core(std::string file, bool allowed_incremental_build){ // Takes the file's path as a full file.
        int status = 0; // This will be the return value to tell the programmer if something goes right or wrong

        std::vector <gdbs::core_file_t> executer_token_pipeline = {}; // This is the location where all the final tokens will be stored

        const std::string CACHE_DIRECTORY = ".gdbs-cache/"; // The directory where the cache files will be created
        const std::string DS_FILE = CACHE_DIRECTORY + "ds.h699"; // The data set file path
        const std::string IPC_FILE = CACHE_DIRECTORY + "ipc.file"; // The inter process cmmunication file path

        bool can_ignore_this_file = false; // This is relative to the file in the file loop

        HELL6_99MO h699_file(file); // Convert the file into a valid h699 file
        h699_file.Parse(); // Parse the h699 file
        HELL6_99MO h699_ds_file(DS_FILE); // Open as a h699 file

        std::string cfg_compiler = "g++",
                    cfg_compiler_arguments = "",
                    cfg_compiler_parguments = "",
                    cfg_system = "",
                    cfg_psystem = "",
                    cfg_pkg_in = "",
                    cfg_bin = "bin",
                    cfg_include = "-Iinclude",
                    cfg_combine = "",
                    cfg_lib = "";

        if (not(std::filesystem::exists(DS_FILE) and std::filesystem::is_regular_file(DS_FILE))){
            can_ignore_this_file = false; // Do not ignore this file at all because this needs to be built and registered inside the dataset
            std::filesystem::create_directory(CACHE_DIRECTORY);
            std::ofstream ofile(DS_FILE);
        }
        else {
            h699_ds_file.Parse(); // Parse the h699 file and we will use this to collect the previous time stamps if it was written to this file
        }

        for (std::string current_file_name_in_h699_file : h699_file.scopes){
            if (current_file_name_in_h699_file != "global" and not(std::filesystem::exists(current_file_name_in_h699_file) and std::filesystem::is_regular_file(current_file_name_in_h699_file))){
                ConsolePrint::print("Error the file named <" + current_file_name_in_h699_file + "> Was unaccessible! Aborting the build.\n", ConsolePrint::Type::Error);
                std::exit (3);
            }

            if (allowed_incremental_build and current_file_name_in_h699_file != "global"){ // If incremental build is allowed then check for the status
                HELL6_99MO_TYPE h699_ds_file_data = h699_ds_file.get(current_file_name_in_h699_file); // Will return UNIDEF if not found and that will mean that we can not ignore this at alll
                if (h699_ds_file_data.type == H699_UNIDEF){
                    std::cout << "No timestamp was found!\n";
                    h699_ds_file.new_key(current_file_name_in_h699_file, "string"); // Registering the file in the data set
                    h699_ds_file.set(current_file_name_in_h699_file, gdbs::timestamp_to_string(std::filesystem::last_write_time(current_file_name_in_h699_file))); // setting it's timestamps
                    h699_ds_file.write(DS_FILE); // writing the dataset file again
                    can_ignore_this_file = false; // Do not ignore this file at all because this needs to be built and registered inside the dataset
                }
                else {
                    auto timestamp_of_current_source_file = std::filesystem::last_write_time(current_file_name_in_h699_file);
                    if (gdbs::timestamp_to_string(timestamp_of_current_source_file) == h699_ds_file_data.string_value){
                        std::cout << "Time stamps did matched!\n";
                        can_ignore_this_file = true; // This can be the only situation where a file can be confidently ignored by the build system
                    }
                    else {
                        std::cout << "Time stamps didnot matched!\n";
                        h699_ds_file.set(current_file_name_in_h699_file, gdbs::timestamp_to_string(std::filesystem::last_write_time(current_file_name_in_h699_file))); // setting it's timestamps
                        h699_ds_file.write(DS_FILE); // writing the dataset file again
                        can_ignore_this_file = false; // Do not ignore this file at all because this needs to be rebuilt and update inside the dataset
                    }
                }
            }

            // Just check if the output file exists or any of the combine files are changed or not
            if (can_ignore_this_file and current_file_name_in_h699_file != "global"){ // Just sandbox a little bit with a differenct scope
                std::string fcfg_out = "",
                            fcfg_combine = cfg_combine;

                HELL6_99MO_TYPE value_buffer;
                value_buffer = h699_file.get(current_file_name_in_h699_file + ".out");
                if (value_buffer.type == H699_UNIDEF){
                    for (std::size_t i = 0;i < current_file_name_in_h699_file.length();i++){
                        if (current_file_name_in_h699_file[i] == '/')
                            fcfg_out += '.';
                        else fcfg_out += current_file_name_in_h699_file[i];
                    }
                    fcfg_out += ".bin";
                }
                else {
                    fcfg_out = value_buffer.string_value;
                    if (fcfg_out == "") {
                        ConsolePrint::print("Error! Need a valid string for <" + current_file_name_in_h699_file + ":out>\nAborting the build.", ConsolePrint::Type::Error);
                        std::exit (3);
                    }
                }

                if (std::filesystem::exists(fcfg_out) and not std::filesystem::is_directory(fcfg_out));
                else {
                    can_ignore_this_file = false;
                }



                // Only run if the output file exists too!
                if (can_ignore_this_file){
                    value_buffer = h699_file.get(current_file_name_in_h699_file + ".combine");
                    if (value_buffer.type == "string"){
                        HELL6_99MO_TYPE ds_timestamp = h699_ds_file.get(value_buffer.string_value);
                        std::string file_time_stamp = gdbs::timestamp_to_string(std::filesystem::last_write_time(value_buffer.string_value));
                        if (ds_timestamp.type == "string"){
                            if (ds_timestamp.string_value != file_time_stamp){
                                can_ignore_this_file = false; // We can not ignore this file because both file stamps are different means it modified in some ways
                            }
                            else {
                                can_ignore_this_file = true; // We can ignore this file without having any problems.
                            }
                        }
                        else {
                            can_ignore_this_file = false; // We can not ignore this file because it have one of the new dependencies
                        }
                    }
                    else if (value_buffer.type == "array"){
                        std::string processed_value;
                        for (std::string value : value_buffer.array_value){
                            HELL6_99MO_TYPE ds_timestamp = h699_ds_file.get(value);
                            std::string file_time_stamp = gdbs::timestamp_to_string(std::filesystem::last_write_time(value));
                            if (ds_timestamp.type == "string"){
                                if (ds_timestamp.string_value != file_time_stamp){
                                    can_ignore_this_file = false; // We can not ignore this file because both file stamps are different means it modified in some ways
                                    break; // Also end the loop othervise this status can be rewritten
                                }
                                else {
                                    can_ignore_this_file = true; // We can ignore this file without having any problems.
                                }
                            }
                            else {
                                can_ignore_this_file = false; // We can not ignore this file because it have one of the new dependencies
                                break; // Also end the loop othervise this status can be rewritten
                            }
                        }
                    }
                }
            }

            if (not can_ignore_this_file){
                std::string fcfg_compiler = cfg_compiler,
                fcfg_compiler_arguments = cfg_compiler_arguments,
                fcfg_compiler_parguments = cfg_compiler_parguments,
                fcfg_system = cfg_system,
                fcfg_psystem = cfg_psystem,
                fcfg_pkg_in = cfg_pkg_in,
                fcfg_out = "",
                fcfg_bin = cfg_bin,
                fcfg_include = cfg_include,
                fcfg_combine = cfg_combine,
                fcfg_lib = cfg_lib;
                
                // Value checking
                HELL6_99MO_TYPE value_buffer;
                value_buffer = h699_file.get(current_file_name_in_h699_file + ".out");
                if (value_buffer.type == H699_UNIDEF){
                    for (std::size_t i = 0;i < current_file_name_in_h699_file.length();i++){
                        if (current_file_name_in_h699_file[i] == '/')
                            fcfg_out += '.';
                        else fcfg_out += current_file_name_in_h699_file[i];
                    }
                    fcfg_out += ".bin";
                }
                else {
                    fcfg_out = value_buffer.string_value;
                    if (fcfg_out == "") {
                        ConsolePrint::print("Error! Need a valid string for <" + current_file_name_in_h699_file + ":out>\nAborting the build.", ConsolePrint::Type::Error);
                        std::exit (3);
                    }
                }
                // std::cout << "Out: "<< fcfg_out << "\n\n";




                // bin
                bool global = (current_file_name_in_h699_file == "global")? true : false; // for simply checking the global was the file name or not because we have to treat the global carefully.
                value_buffer = h699_file.get(current_file_name_in_h699_file + ".bin");
                if (value_buffer.type == "string"){
                    if (global) cfg_bin = value_buffer.string_value;
                    else fcfg_bin = value_buffer.string_value;
                }


                // compiler
                value_buffer = h699_file.get(current_file_name_in_h699_file + ".compiler");
                if (value_buffer.type == "string"){
                    if (global) cfg_compiler = value_buffer.string_value;
                    else fcfg_compiler = value_buffer.string_value;
                }


                // compiler_arguments
                value_buffer = h699_file.get(current_file_name_in_h699_file + ".compiler_arguments");
                if (value_buffer.type == "string"){
                    if (global) cfg_compiler_arguments = value_buffer.string_value;
                    else fcfg_compiler_arguments = value_buffer.string_value;
                }


                // compiler_parguments
                value_buffer = h699_file.get(current_file_name_in_h699_file + ".compiler_parguments");
                if (value_buffer.type == "string"){
                    if (global) cfg_compiler_parguments = value_buffer.string_value;
                    else fcfg_compiler_parguments = value_buffer.string_value;
                }


                // system
                value_buffer = h699_file.get(current_file_name_in_h699_file + ".system");
                if (value_buffer.type == "string"){
                    if (global) cfg_system = value_buffer.string_value;
                    else fcfg_system = value_buffer.string_value;
                }
                else if (value_buffer.type == "array"){
                    std::string content;
                    for (std::string command : value_buffer.array_value){
                        content += command + '\n';
                    }
                    if (global) cfg_system = content;
                    else fcfg_system = content;
                }


                // psystem
                value_buffer = h699_file.get(current_file_name_in_h699_file + ".psystem");
                if (value_buffer.type == "string"){
                    if (global) cfg_psystem = value_buffer.string_value;
                    else fcfg_psystem = value_buffer.string_value;
                }
                else if (value_buffer.type == "array"){
                    std::string content;
                    for (std::string command : value_buffer.array_value){
                        content += command + '\n';
                    }
                    if (global) cfg_psystem = content;
                    else fcfg_psystem = content;
                }




                // include
                value_buffer = h699_file.get(current_file_name_in_h699_file + ".include");
                if (value_buffer.type == "string"){
                    if (global) cfg_include = " -I" + value_buffer.string_value;
                    else fcfg_include = " -I" + value_buffer.string_value;
                }
                else if (value_buffer.type == "array"){
                    std::string processed_value;
                    for (std::string value : value_buffer.array_value){
                        processed_value += " -I" + value;
                    }
                    if (global) cfg_include = processed_value;
                    else fcfg_include = processed_value;
                }



                // lib
                value_buffer = h699_file.get(current_file_name_in_h699_file + ".lib");
                if (value_buffer.type == "string"){
                    if (global) cfg_lib = " -l" + value_buffer.string_value;
                    else fcfg_lib = " -l" + value_buffer.string_value;
                }
                else if (value_buffer.type == "array"){
                    std::string processed_value;
                    for (std::string value : value_buffer.array_value){
                        processed_value += " -l" + value;
                    }
                    if (global) cfg_lib = processed_value;
                    else fcfg_lib = processed_value;
                }



                // combine
                value_buffer = h699_file.get(current_file_name_in_h699_file + ".combine");
                if (value_buffer.type == "string"){
                    if (global) cfg_combine = " " + value_buffer.string_value;
                    else fcfg_combine = " " + value_buffer.string_value;
                }
                else if (value_buffer.type == "array"){
                    std::string processed_value;
                    for (std::string value : value_buffer.array_value){
                        processed_value += " " + value;
                    }
                    if (global) cfg_combine = processed_value;
                    else fcfg_combine = processed_value;
                }



                // pkg_in
                value_buffer = h699_file.get(current_file_name_in_h699_file + ".pkg_in");
                if (value_buffer.type == "string"){
                    std::string content;
                    int pkg_config = std::system (std::string("pkg-config --cflags --libs " + value_buffer.string_value + " 2>/dev/null 1>" + IPC_FILE).c_str());
                    if (pkg_config == 0){ // The packages were found
                        std::ifstream ipc_file_read (IPC_FILE);
                        if (ipc_file_read.is_open()){
                            char c;
                            while (ipc_file_read.get(c)){
                                content += c;
                            }
                        }
                        else {
                            ConsolePrint::print("The IPC file <" + IPC_FILE + "> was not accessible!\nAborting build.", ConsolePrint::Type::Error);
                            std::exit (3);
                        }
                    }
                    else {
                        ConsolePrint::print("The Package <" + value_buffer.string_value + "> was not found on your system!\nAborting build.", ConsolePrint::Type::Error);
                        std::exit (3);
                    }
                    if (global) cfg_pkg_in = content;
                    else fcfg_pkg_in = content;
                }
                else if (value_buffer.type == "array"){
                    std::string content;
                    for (std::string package : value_buffer.array_value){
                        int pkg_config = std::system (std::string("pkg-config --cflags --libs " + package + " 2>/dev/null 1>" + IPC_FILE).c_str());
                        if (pkg_config == 0){ // The packages were found
                            std::ifstream ipc_file_read (IPC_FILE);
                            if (ipc_file_read.is_open()){
                                char c;
                                while (ipc_file_read.get(c)){
                                    content += c;
                                }
                                content += " ";
                            }
                            else {
                                ConsolePrint::print("The IPC file <" + IPC_FILE + "> was not accessible!\nAborting build.", ConsolePrint::Type::Error);
                                std::exit (3);
                            }
                        }
                        else {
                            ConsolePrint::print("The Package <" + package + "> was not found on your system!\nAborting build.", ConsolePrint::Type::Error);
                            std::exit (3);
                        }
                    }
                    
                    if (global) cfg_pkg_in = content;
                    else fcfg_pkg_in = content;
                }



                // Process the build commands
                if (global) continue; // ignore if it is global
                
                core_file_t token; // Create a token for the executer
                token.command = fcfg_compiler_parguments + "\n" + fcfg_compiler + " " + fcfg_compiler_parguments + " " + current_file_name_in_h699_file + " -o " + fcfg_out  + " " + fcfg_combine + " " + fcfg_include + " " + fcfg_lib + " " + fcfg_pkg_in + " " + fcfg_compiler_arguments + "\n" + fcfg_system;
                token.output = fcfg_out;
                token.filename = current_file_name_in_h699_file;

                // Send the token to the executer's token pipeline
                executer_token_pipeline.push_back(token);
                // std::cout << "Command: "<<token.command<<"\n\n"; // Just for the debugging
            }

        }

        gdbs::executer(executer_token_pipeline, 3);



        return status;
    }
}