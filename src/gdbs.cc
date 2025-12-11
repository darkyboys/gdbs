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

int main(int argc, char* argv[]){
    if (argc < 2){
        std::cout << R"(
┌──────────────────────────────────────┐
│                                      │
│   Goblin's Demonic Build System 1.0  │
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
╰──────────────────────────────────────╯

        )";
    }
    else {
        int total_threads = std::thread::hardware_concurrency();
        std::vector <std::string> args;
        std::string path = ".";
        bool allowed_incremental_build = true;
        bool show_command = false;
        
        for (int i = 1;i < argc;i++){
            std::string argument = argv[i];
            if (argument == "--version" or argument == "-v"){
                std::cout << "GDBS VERSION " << GDBS_CORE_HH << "\n";
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

        if (not (std::filesystem::exists(path + "/build.gdbs") and not std::filesystem::is_directory(path + "/build.gdbs"))){
            ConsolePrint::print("Error: The build.gdbs file wasn't found at <.>.\nAborting the core.", ConsolePrint::Type::Error);
            std::exit (3);
        }

        // std::cout << "Allowed Increment: "<<allowed_incremental_build<<"\n";
        gdbs::core(path + "/build.gdbs", args, total_threads, allowed_incremental_build, show_command);
    }
}