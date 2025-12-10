/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under the MIT License
 * Checkout the README.md for more information
 */

// This file covers the cli part of the build system

// Header
// STL
#include <iostream>
#include  <fstream>
#include   <string>

#include <components/core.hh>

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
│                                      │
│                                      │
│                                      │
│                                      │
│                                      │
│                                      │
│                                      │
│                                      │
│                                      │
╰──────────────────────────────────────╯

        )";
    }
    else {
        gdbs::core("build.gdbs");
    }
}