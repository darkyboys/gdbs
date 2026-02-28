/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under the MIT License
 * Checkout the README.md for more information
 */

// This file covers the cli part of the build system

// Header
// STL
#include <filesystem>
#include <iostream>
#include  <fstream>
#include   <string>

#include <components/core.hh>
#include <ConsolePrint/ConsolePrint.hh>
#include <h699/h699.hh>

std::string replaceAll(std::string target_string, // just a helper function
                       std::string what,
                       std::string with)
{
    if (what.empty()) return target_string;

    size_t pos = 0;
    while ((pos = target_string.find(what, pos)) != std::string::npos)
    {
        target_string.replace(pos, what.length(), with);
        pos += with.length(); // Move past the replacement
    }

    return target_string;
}

int main(int argc, char* argv[]){
    if (argc < 2){
        std::cout << R"(
┌──────────────────────────────────────┐
│                                      │
│   Goblin's Demonic Build System 1.8  │
│                                      │
└──────────────────────────────────────┘
╭──────────────────────────────────────╮
│             Offline Docs             │
│                                      │
│ 1.) --version : Shows the version    │
│                                      │
│ 2.) --show-commands: Show the system │
│     Command for the build.           │
│                                      │
│ 3.) --thread <number>: Sets the      │
│     thread limit for build.          │
│                                      │
│ 4.) --clean : Builds the project     │
│      without any configurations.     │
│                                      │
│ 5.) --noincrement: disables the      │
│     incremental build.               │
│                                      │
│ 6.) --cache-dir <name>: sets the     │
│     cachedirectory for the build     │
│     system to use.                   │
│                                      │
│ 7.) --build-type <type>: sets the    │
│     build type.                      │
│                                      │
╰──────────────────────────────────────╯

        )";
    }
    else {
        int total_threads = std::thread::hardware_concurrency();
        std::vector <std::string> args;
        std::string path = ".";
        bool allowed_incremental_build = true;
        bool show_command = false;
        std::string cache_directory = ".gdbs-cache/";
        std::string build_type = "default";
        std::string build_type_arguments;

        for (int i = 1;i < argc;i++){
            std::string argument = argv[i];
            if (argument == "--version" or argument == "-v"){
                std::cout << "GDBS VERSION 1.8\nGDBS CORE " << GDBS_CORE_HH << "\n";
                std::exit (0);
            }
            else if (argument == "--show-commands" or argument == "-sc"){
                show_command = true;
            }
            else if (argument == "--noincrement" or argument == "-ni"){
                allowed_incremental_build = false;
            }
            else if (argument == "--thread" or argument == "-t"){
                if (argc - 1 == i){
                    ConsolePrint::print ("Error: Needs the total thread count, Please execute gdbs without any argument for the offline documentation.", ConsolePrint::Type::Error);
                    std::exit (3);
                }
                else {
                    bool is_number = true;
                    for (char c : std::string(argv[i+1])){
                        if (c >= '0' or c <= '9'){
                            continue;
                        }
                        else {
                            is_number = false;
                            break;
                        }
                    }
                    if (not is_number){
                        ConsolePrint::print ("Error: The total thread count must be a valid number, Please execute gdbs without any argument for the offline documentation.", ConsolePrint::Type::Error);
                        std::exit (3);
                    }
                    total_threads = std::stoi(std::string(argv[i+1]));
                    i+=1; // Ignore the extra argument
                }
            }
            else if (argument == "--cache-dir" or argument == "-cd"){
                if (argc - 1 == i){
                    ConsolePrint::print ("Error: Needs the path to the cache directory, Please execute gdbs without any argument for the offline documentation.", ConsolePrint::Type::Error);
                    std::exit (3);
                }
                else {
                    cache_directory = std::string(argv[i+1]);
                    cache_directory += "/";
                    i+=1;
                }
            }
            else if (argument == "--build-type" or argument == "-bt"){
                if (argc - 1 == i){
                    ConsolePrint::print ("Error: Needs a proper build type to build, Please execute gdbs without any argument for the offline documentation.", ConsolePrint::Type::Error);
                    std::exit (3);
                }
                else {
                    build_type = std::string(argv[i+1]);
                    if (std::filesystem::exists("build-type.gdbs") and std::filesystem::is_regular_file("build-type.gdbs")){
                        HELL6_99MO build_type_h699("build-type.gdbs");
                        build_type_h699.Parse();
                        bool is_valid_build_type = false;
                        for (std::string current_build_type_in_h699 : build_type_h699.scopes){
                            if (current_build_type_in_h699 == build_type){
                                is_valid_build_type = true;
                                break;
                            }
                        }

                        if (not is_valid_build_type){
                            ConsolePrint::print ("Error: Build type `" + build_type + "` was not found.", ConsolePrint::Type::Error);
                            std::exit (3);
                        }

                        std::string full_comparison_token = build_type + ".";
                        
                        // This part of the code is more lower level because this thing is manipulating the inputs at the file level
                        for (std::vector <std::string> current_token : build_type_h699.string_keys){
                            // std::cout << <<"\n";
                            std::string property;
                            if (current_token[0].substr(0, full_comparison_token.length()) == full_comparison_token){
                                property = "global: " + current_token[0].substr(full_comparison_token.length());
                                property += " = \"" + replaceAll(current_token[1], "\"", "\\\"") + "\""; // normalize the strings too
                                build_type_arguments += (property + "\n");
                            }
                        }


                        for (std::vector <std::string> current_token : build_type_h699.array_keys){
                            // std::cout << <<"\n";
                            std::string property;
                            if (current_token[0].substr(0, full_comparison_token.length()) == full_comparison_token){
                                property = "global: " + current_token[0].substr(full_comparison_token.length());
                                // property += " = \"" + current_token[1] + "\"";
                                // std::cout << "Value: "<<property<<"\n";
                                property += " = ["; // start the array
                                for (std::size_t x = 1;x < current_token.size();x++)
                                    property += "\"" + replaceAll(current_token[x], "\"", "\\\"") + "\","; // fill the array
                                property.pop_back(); // remove the last ',' symbol
                                property += "]";
                                // std::cout << "Value: "<<property<<"\n";
                                build_type_arguments += (property + "\n");
                            }
                        }
                    }
                    else {
                        ConsolePrint::print ("Error: Build type `" + build_type + "` was not found.", ConsolePrint::Type::Error);
                        std::exit (3);
                    }
                    i+=1;
                }
            }
            else if (argument == "--clean" or argument == "-c"){
                std::system ("rm -rf .gdbs-cache"); // Remove the gdbs cache to clean the project
            }
            else {
                if (argument[0] == '-'){
                    args.push_back(argument);
                }
                else {
                    if (not (std::filesystem::exists(argument + "/build.gdbs") and not std::filesystem::is_directory(argument + "/build.gdbs"))){
                        ConsolePrint::print("Error: The build.gdbs file wasn't found at <" + argument + ">.\nAborting the core.", ConsolePrint::Type::Error);
                        std::exit (3);
                    }
                    path = argument;
                }
            }
        }

        std::string build_file_name = path + "/build.gdbs";

        if (not (std::filesystem::exists(build_file_name) and not std::filesystem::is_directory(path + "/build.gdbs"))){
            ConsolePrint::print("Error: The build.gdbs file wasn't found at <.>.\nAborting the core.", ConsolePrint::Type::Error);
            std::exit (3);
        }

        if (not(std::filesystem::exists(cache_directory) and std::filesystem::is_directory(cache_directory))){
            std::filesystem::create_directory(cache_directory);
        }
        const std::string genBuildFileName = cache_directory + "/gen-build.gdbs";
        if (build_type != ""){
            std::ofstream ofile_genBuildFile(genBuildFileName);
            std::ifstream ifile_build_gdbs(build_file_name);
            std::string temp, content;
            while (std::getline(ifile_build_gdbs, temp))
                content += temp + '\n';

            ofile_genBuildFile<<content<<build_type_arguments;
            
            build_file_name = genBuildFileName;
        }
        // std::cout << "Allowed Increment: "<<allowed_incremental_build<<"\n";
        gdbs::core(build_file_name, args, total_threads, allowed_incremental_build, show_command, cache_directory);
        
        // std::system (std::string("rm -rf " + genBuildFileName).c_str());
    }
}