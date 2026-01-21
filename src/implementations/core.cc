/*
 * Copyright (c) ghgltggamer 2025
 * Written by ghgltggamer
 * Licensed under the MIT License
 * Checkout the README.md for more information
 */

// This file contains the core of the project

// Header
// Local
#include <components/core.hh>
#include <components/fs-utils.hh>
#include <components/executer.hh>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#define PKGCONF_FILE ".gdbs-cache/pkg-config.stdout"

namespace gdbs{
    int is_timestamp_same(std::string file_name, HELL6_99MO& H699){
        HELL6_99MO_TYPE fileStatus = H699.get(file_name);
        if (fileStatus.type == H699_UNIDEF){
            return 0;
        }
        else {
            if (fileStatus.string_value == gdbs::timestamp_to_string(std::filesystem::last_write_time(file_name))){
                return 1;
            }
            else {
                return 0;
            }
        }
    }

    void set_timestamp(std::string file_name, HELL6_99MO& H699){
        HELL6_99MO_TYPE fileStatus = H699.get(file_name);
        if (fileStatus.type == H699_UNIDEF){
            H699.new_key(file_name, "string");
            H699.set(file_name, gdbs::timestamp_to_string(std::filesystem::last_write_time(file_name)));
        }
        else {
            H699.set(file_name, gdbs::timestamp_to_string(std::filesystem::last_write_time(file_name)));
        }
    }


    std::vector<std::string> listFilesInDirectory(const std::string& directoryPath) {
        std::vector<std::string> files;

        std::filesystem::path dirPath(directoryPath);

        if (std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath)) {
            for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(dirPath)) {
                if (std::filesystem::is_regular_file(entry.status())) {
                    files.push_back(entry.path().filename().string());
                }
            }
        }

        return files;
    }



    int core(std::string file, std::vector <std::string> cli_args, int threads, bool allowed_incremental_build, bool show_command){ // Takes the file's path as a full file.
        int status = 0; // This will be the return value to tell the programmer if something goes right or wrong
        const std::string dsFilePath = "ds.h699";
        const std::string cacheDirectory = ".gdbs-cache/";
        bool can_ignore_this_file = false;
        std::string filePath = cacheDirectory + dsFilePath;
        if (!(std::filesystem::exists(filePath) and std::filesystem::is_regular_file(filePath))){
            std::filesystem::create_directory(cacheDirectory);
            std::ofstream ofile(filePath);
        }

        HELL6_99MO openFilePathH699(filePath);
        openFilePathH699.Parse();

        HELL6_99MO buildFileH699(file);
        buildFileH699.Parse();

        std::vector <std::string> updatedFileVector;
        std::vector <gdbs::core_file_t> executer_list;

        std::vector <std::string> cli,
                                  callback,
                                  afterchange,
                                  onchange,
                                  ontriggerchange;
        bool is_anything_changed = false;
                                  


        // Configuration
        std::string cfg_compiler = "g++",
                    cfg_compiler_arguments = "",
                    cfg_compiler_parguments = "",
                    cfg_system = "",
                    cfg_psystem = "",
                    cfg_pkg_in = "",
                    cfg_bin = "bin",
                    cfg_include = "-Iinclude",
                    cfg_lib = "";
        
        std::vector <std::string> cfg_combine,
                                  cfg_compare;

        std::vector <gdbs::comp_file_t> updated_comp_file_h699 = {};

        bool is_global = false;

        // This part of the code is more lower level because this thing is manipulating the inputs at the file level
        for (std::vector <std::string> current_token : buildFileH699.string_keys){
            // std::cout << <<"\n";
            std::string argument;
            if (current_token[0].substr(0, 4) == "cli."){
                argument = current_token[0].substr(4);
                for (std::string currentArgument : cli_args){
                    if (argument == currentArgument){
                        cli.push_back(current_token[1]);
                    }
                }
            }
        }


        for (std::vector <std::string> current_token : buildFileH699.array_keys){
            // std::cout << <<"\n";
            std::string argument;
            if (current_token[0].substr(0, 4) == "cli."){
                argument = current_token[0].substr(4);
                for (std::string currentArgument : cli_args){
                    if (argument == currentArgument){
                        for (std::size_t i = 1;i < current_token.size();i++){
                            cli.push_back(current_token[i]);
                        }
                    }
                }
            }
        }

        for (std::string cont : cli){ // Enter into the CLI Mode So ignore everything
            std::system (cont.c_str());
        }
        if (cli.size() != 0) std::exit (0);



        for (std::size_t i = 0;i < buildFileH699.scopes.size();i++){
            std::string currentFileName = buildFileH699.scopes[i];
            if (currentFileName == "") continue;
            if (currentFileName[currentFileName.length()-1] == '*' and currentFileName[currentFileName.length()-2] == '/'){
                // std::cout << "Detected!\n"; // for debugging only
                std::string path = currentFileName;
                path = path.substr(0, path.length()-1);
                if (not (std::filesystem::is_directory(path))){
                    ConsolePrint::print("Error:- Couldn't expand `" + currentFileName + "` because `" + path + "` was not a directory. Aborting the build!\n");
                    std::exit (3);
                }
                buildFileH699.scopes[i] = ""; // make it empty so that the expanding instruction becomes nothing and being ignored by the core later
                for (std::string fileAtPath : gdbs::listFilesInDirectory(path)){
                    buildFileH699.scopes.insert(buildFileH699.scopes.begin() + i, path + fileAtPath);
                    // std::cout << buildFileH699.scopes[i] <<"\n"; // for debugging
                    buildFileH699.new_key(path + fileAtPath + ".out", "string");
                    // std::cout << path + fileAtPath + ".out" << "\n\n"; // for debugging
                    std::string file_name_token;
                    std::string buffer;
                    bool dot_found = false;
                    for (std::size_t x = 0;x < fileAtPath.length();x++){
                        if (fileAtPath[x] == '.'){
                            dot_found = true;
                            file_name_token += buffer;
                            buffer.clear();
                        } 
                        buffer += fileAtPath[x];
                    }
                    if (dot_found == false){
                        file_name_token = buffer;
                    }
                    HELL6_99MO_TYPE ext = buildFileH699.get(currentFileName + ".ext");
                    std::string extension = "bin";
                    if (ext.type == "string"){
                        extension = ext.string_value;
                    }
                    else if (ext.type != H699_UNIDEF){
                        ConsolePrint::print("Warning:- ext for `" + currentFileName + "` can only store strings! Ignoring different types.");
                    }
                    file_name_token = file_name_token + "." + extension;
                    buildFileH699.set(path + fileAtPath + ".out", file_name_token);
                }
                i--;
                continue;
            }
            if (currentFileName == "cli"){
                continue;
            }
            if (currentFileName == "call"){
                HELL6_99MO_TYPE call_value = buildFileH699.get(currentFileName + ".command");
                if (call_value.type == "string"){
                    std::system(call_value.string_value.c_str());
                }
                else if (call_value.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- call.command can only be a string type. Please refer the call.commands if you need a list of commands to be executed.", ConsolePrint::Type::Warning);
                }


                HELL6_99MO_TYPE call_arr_value = buildFileH699.get(currentFileName + ".commands");
                if (call_arr_value.type == "array"){
                    for (std::string command : call_arr_value.array_value){
                        std::system(command.c_str());
                    }
                }
                else if (call_arr_value.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- call.commands can only be an array type. Please refer the call.command if you need a single command to be executed.", ConsolePrint::Type::Warning);
                }
                continue;
            }



            if (currentFileName == "callback"){
                HELL6_99MO_TYPE callback_value = buildFileH699.get(currentFileName + ".command");
                if (callback_value.type == "string"){
                    callback.push_back(callback_value.string_value);
                }
                else if (callback_value.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- callback.command can only be a string type. Please refer the callback.commands if you need a list of commands to be executed.", ConsolePrint::Type::Warning);
                }


                HELL6_99MO_TYPE callback_arr_value = buildFileH699.get(currentFileName + ".commands");
                if (callback_arr_value.type == "array"){
                    callback = callback_arr_value.array_value;
                }
                else if (callback_arr_value.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- callback.commands can only be an array type. Please refer the callback.command if you need a single command to be executed.", ConsolePrint::Type::Warning);
                }
                continue;
            }






            if (currentFileName == "onchange"){
                HELL6_99MO_TYPE onchange_value = buildFileH699.get(currentFileName + ".command");
                if (onchange_value.type == "string"){
                    onchange.push_back(onchange_value.string_value);
                }
                else if (onchange_value.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- onchange.command can only be a string type. Please refer the onchange.commands if you need a list of commands to be executed.", ConsolePrint::Type::Warning);
                }


                HELL6_99MO_TYPE onchange_arr_value = buildFileH699.get(currentFileName + ".commands");
                if (onchange_arr_value.type == "array"){
                    onchange = onchange_arr_value.array_value;
                }
                else if (onchange_arr_value.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- onchange.commands can only be an array type. Please refer the onchange.command if you need a single command to be executed.", ConsolePrint::Type::Warning);
                }


                HELL6_99MO_TYPE onchange_need = buildFileH699.get(currentFileName + ".need");
                if (onchange_need.type == "string"){
                    if (not std::filesystem::exists(onchange_need.string_value)){
                        is_anything_changed = true;
                    }
                }
                else if (onchange_need.type == "array"){
                    for (std::string f : onchange_need.array_value){
                        if (not std::filesystem::exists(f)){
                            is_anything_changed = true;
                            break;
                        }
                    }
                }
                else if (onchange_need.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- onchange.commands can only be an array type. Please refer the onchange.command if you need a single command to be executed.", ConsolePrint::Type::Warning);
                }

                continue;
            }






            if (currentFileName == "ontriggerchange"){
                HELL6_99MO_TYPE ontriggerchange_value = buildFileH699.get(currentFileName + ".command");
                if (ontriggerchange_value.type == "string"){
                    ontriggerchange.push_back(ontriggerchange_value.string_value);
                }
                else if (ontriggerchange_value.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- ontriggerchange.command can only be a string type. Please refer the ontriggerchange.commands if you need a list of commands to be executed.", ConsolePrint::Type::Warning);
                }


                HELL6_99MO_TYPE ontriggerchange_arr_value = buildFileH699.get(currentFileName + ".commands");
                if (ontriggerchange_arr_value.type == "array"){
                    ontriggerchange = ontriggerchange_arr_value.array_value;
                }
                else if (ontriggerchange_arr_value.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- ontriggerchange.commands can only be an array type. Please refer the ontriggerchange.command if you need a single command to be executed.", ConsolePrint::Type::Warning);
                }




                HELL6_99MO_TYPE ontriggerchange_need = buildFileH699.get(currentFileName + ".need");
                if (ontriggerchange_need.type == "string"){
                    if (not std::filesystem::exists(ontriggerchange_need.string_value)){
                        is_anything_changed = true;
                    }
                }
                else if (ontriggerchange_need.type == "array"){
                    for (std::string f : ontriggerchange_need.array_value){
                        if (not std::filesystem::exists(f)){
                            is_anything_changed = true;
                            break;
                        }
                    }
                }
                else if (ontriggerchange_need.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- ontriggerchange.commands can only be an array type. Please refer the ontriggerchange.command if you need a single command to be executed.", ConsolePrint::Type::Warning);
                }
                continue;
            }









            if (currentFileName == "afterchange"){
                HELL6_99MO_TYPE afterchange_value = buildFileH699.get(currentFileName + ".command");
                if (afterchange_value.type == "string"){
                    afterchange.push_back(afterchange_value.string_value);
                }
                else if (afterchange_value.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- afterchange.command can only be a string type. Please refer the afterchange.commands if you need a list of commands to be executed.", ConsolePrint::Type::Warning);
                }


                HELL6_99MO_TYPE afterchange_arr_value = buildFileH699.get(currentFileName + ".commands");
                if (afterchange_arr_value.type == "array"){
                    afterchange = afterchange_arr_value.array_value;
                }
                else if (afterchange_arr_value.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- afterchange.commands can only be an array type. Please refer the afterchange.command if you need a single command to be executed.", ConsolePrint::Type::Warning);
                }


                HELL6_99MO_TYPE afterchange_need = buildFileH699.get(currentFileName + ".need");
                if (afterchange_need.type == "string"){
                    if (not std::filesystem::exists(afterchange_need.string_value)){
                        is_anything_changed = true;
                    }
                }
                else if (afterchange_need.type == "array"){
                    for (std::string f : afterchange_need.array_value){
                        if (not std::filesystem::exists(f)){
                            is_anything_changed = true;
                            break;
                        }
                    }
                }
                else if (afterchange_need.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- afterchange.commands can only be an array type. Please refer the afterchange.command if you need a single command to be executed.", ConsolePrint::Type::Warning);
                }

                continue;
            }







            if (currentFileName == "global"){
                is_global = true;
                can_ignore_this_file = true;
            }
            if (not is_global){
                if (not(std::filesystem::exists(currentFileName))){
                    ConsolePrint::print("The file `" + currentFileName + "` defined in `" + file + "` do not exists. Aborting the build!", ConsolePrint::Type::Error);
                    std::exit (3);
                }
                if (gdbs::is_timestamp_same(currentFileName, openFilePathH699)){
                    can_ignore_this_file = true;
                }
            }


            std::string fcfg_compiler = cfg_compiler,
                        fcfg_compiler_arguments = cfg_compiler_arguments,
                        fcfg_compiler_parguments = cfg_compiler_parguments,
                        fcfg_system = cfg_system,
                        fcfg_psystem = cfg_psystem,
                        fcfg_pkg_in = cfg_pkg_in,
                        fcfg_out = "",
                        fcfg_bin = cfg_bin,
                        fcfg_include = cfg_include,
                        fcfg_combine_str = "",
                        fcfg_lib = cfg_lib;

            std::vector <std::string> fcfg_combine = cfg_combine,
                                      fcfg_compare = cfg_compare;


















            // Checking the essential configurations no matter what
            // expanded_compare
            HELL6_99MO_TYPE expanded_compare = buildFileH699.get(currentFileName + ".expanded_compare");
            if (expanded_compare.type == "string"){
                if (not (std::filesystem::is_directory(expanded_compare.string_value))){
                    ConsolePrint::print("Error:- the path passed to the expanded_compare was not a directory, For file `" + currentFileName + "` in `" + file + "`. The Path `" + expanded_compare.string_value + "`. Critical error, Aborting the build!\n", ConsolePrint::Type::Error);
                    std::exit (3);
                }
                if (buildFileH699.get(currentFileName + ".compare").type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- Ignoring expanded_compare because compare key already exists, For file `" + currentFileName + "` in `" + file + "`. The Path `" + expanded_compare.string_value + "`.", ConsolePrint::Type::Warning);
                }
                else {
                    std::vector <std::string> total_files = {};
                    for (std::string files : gdbs::listFilesInDirectory(expanded_compare.string_value)){
                        total_files.push_back (expanded_compare.string_value + "/" + files);
                    }
                    buildFileH699.new_key(currentFileName + ".compare", "array");
                    buildFileH699.set_array(currentFileName + ".compare", total_files);
                }
            }
            else if (expanded_compare.type == "array"){
                if (buildFileH699.get(currentFileName + ".compare").type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- Ignoring expanded_compare because compare key already exists, For file `" + currentFileName + "` in `" + file + "`.", ConsolePrint::Type::Warning);
                }
                else {
                    std::vector <std::string> total_files = {};
                    for (std::string path : expanded_compare.array_value){
                        if (not (std::filesystem::is_directory(path))){
                            ConsolePrint::print("Error:- the path passed to the expanded_compare was not a directory, For file `" + currentFileName + "` in `" + file + "`. The Path `" + path + "`. Critical error, Aborting the build!\n", ConsolePrint::Type::Error);
                            std::exit (3);
                        } 
                        for (std::string files : gdbs::listFilesInDirectory(path)){
                            total_files.push_back (path + "/" + files);
                        }
                    }
                    buildFileH699.new_key(currentFileName + ".compare", "array");
                    buildFileH699.set_array(currentFileName + ".compare", total_files);
                }
            }
            else if (expanded_compare.type != H699_UNIDEF){
                ConsolePrint::print("Warning:- the expanded_compare key can only contain a string or array, For file `" + currentFileName + "` in `" + file + "`. Ignoring this argument", ConsolePrint::Type::Warning);
            }







            // expanded_combine
            HELL6_99MO_TYPE expanded_combine = buildFileH699.get(currentFileName + ".expanded_combine");
            if (expanded_combine.type == "string"){
                if (not (std::filesystem::is_directory(expanded_combine.string_value))){
                    ConsolePrint::print("Error:- the path passed to the expanded_combine was not a directory, For file `" + currentFileName + "` in `" + file + "`. The Path `" + expanded_combine.string_value + "`. Critical error, Aborting the build!\n", ConsolePrint::Type::Error);
                    std::exit (3);
                }
                if (buildFileH699.get(currentFileName + ".combine").type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- Ignoring expanded_combine because combine key already exists, For file `" + currentFileName + "` in `" + file + "`. The Path `" + expanded_combine.string_value + "`.", ConsolePrint::Type::Warning);
                }
                else {
                    std::vector <std::string> total_files = {};
                    for (std::string files : gdbs::listFilesInDirectory(expanded_combine.string_value)){
                        total_files.push_back (expanded_combine.string_value + "/" + files);
                    }
                    buildFileH699.new_key(currentFileName + ".combine", "array");
                    buildFileH699.set_array(currentFileName + ".combine", total_files);
                }
            }
            else if (expanded_combine.type == "array"){
                if (buildFileH699.get(currentFileName + ".combine").type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- Ignoring expanded_combine because combine key already exists, For file `" + currentFileName + "` in `" + file + "`.", ConsolePrint::Type::Warning);
                }
                else {
                    std::vector <std::string> total_files = {};
                    for (std::string path : expanded_combine.array_value){
                        if (not (std::filesystem::is_directory(path))){
                            ConsolePrint::print("Error:- the path passed to the expanded_combine was not a directory, For file `" + currentFileName + "` in `" + file + "`. The Path `" + path + "`. Critical error, Aborting the build!\n", ConsolePrint::Type::Error);
                            std::exit (3);
                        } 
                        for (std::string files : gdbs::listFilesInDirectory(path)){
                            total_files.push_back (path + "/" + files);
                        }
                    }
                    buildFileH699.new_key(currentFileName + ".combine", "array");
                    buildFileH699.set_array(currentFileName + ".combine", total_files);
                }
            }
            else if (expanded_combine.type != H699_UNIDEF){
                ConsolePrint::print("Warning:- the expanded_combine key can only contain a string or array, For file `" + currentFileName + "` in `" + file + "`. Ignoring this argument", ConsolePrint::Type::Warning);
            }










            // bin
            HELL6_99MO_TYPE bin = buildFileH699.get(currentFileName + ".bin");
            if (bin.type == "string"){
                if (is_global) cfg_bin = bin.string_value;
                else fcfg_bin = bin.string_value;
            }
            else if (bin.type != H699_UNIDEF){
                ConsolePrint::print("Warning:- the bin key can only contain a string, For file `" + currentFileName + "` in `" + file + "`. Ignoring this argument", ConsolePrint::Type::Warning);
            }




            if (not is_global){
                // Checking if the binary file of that file exists or not
                HELL6_99MO_TYPE out = buildFileH699.get(currentFileName + ".out");
                if (out.type == "string"){
                    fcfg_out = out.string_value;
                }
                else if (out.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- the out key can only contain a string, For file `" + currentFileName + "` in `" + file + "`. Ignoring this argument", ConsolePrint::Type::Warning);
                }
                else {
                    fcfg_out = currentFileName;
                    for (char& c : fcfg_out){
                        if (c == '/') c = '.';
                    }
                }
                fcfg_out = fcfg_bin + "/" + fcfg_out;
                // std::cout << fcfg_out <<"\n";
                if (not (std::filesystem::exists(fcfg_out) and std::filesystem::is_regular_file(fcfg_out))){
                    can_ignore_this_file = false;
                }

                // Checking the output binary's timestamps
                if (can_ignore_this_file){
                    if (not (gdbs::is_timestamp_same(fcfg_out, openFilePathH699))){
                        can_ignore_this_file = false;
                        updatedFileVector.push_back(fcfg_out);
                    }
                }
            }









            std::string comp_file_name = currentFileName;
            if (is_global){
                comp_file_name = dsFilePath;
            }
            for (char &c : comp_file_name){
                if (c == '/') c = '.';
            }
            comp_file_name = cacheDirectory + comp_file_name;

            if (not (std::filesystem::exists(comp_file_name) and std::filesystem::is_regular_file(comp_file_name))){
                std::ofstream ofile (comp_file_name);
            }

            HELL6_99MO hfile(comp_file_name);
            hfile.Parse();

            HELL6_99MO_TYPE combine = buildFileH699.get(currentFileName + ".combine");
            std::vector <std::string> fcfg_combine_vector;
            if (combine.type == "string"){
                if (is_global) {
                    if (combine.string_value != "") cfg_combine.push_back(combine.string_value);
                    else {
                        cfg_combine.clear();
                    }
                }
                else {
                    if (combine.string_value != "") fcfg_combine.push_back(combine.string_value);
                    else {
                        fcfg_combine.clear();
                    }
                }
            }
            else if (combine.type == "array"){
                if (is_global) cfg_combine = combine.array_value;
                else fcfg_combine = combine.array_value;
            }
            else if (combine.type != H699_UNIDEF){
                ConsolePrint::print("Warning:- the combine key can only contain a string or an array, For file `" + currentFileName + "` in `" + file + "`. Ignoring this argument", ConsolePrint::Type::Warning);
            }
            for (std::string cfile : fcfg_combine){
                if (is_global){
                    break;
                }
                if (not (std::filesystem::exists(cfile) and std::filesystem::is_regular_file(cfile))){
                    ConsolePrint::print("Error:- the file `" + cfile + "` was not found to combine, For file `" + currentFileName + "` in `" + file + "`. Aborting the build!", ConsolePrint::Type::Error);
                    std::exit (3);
                }
                if (not gdbs::is_timestamp_same(cfile, hfile)){
                    can_ignore_this_file = false;
                    comp_file_t comp_file;
                    comp_file.h699_file_name = comp_file_name;
                    comp_file.filename = currentFileName;
                    comp_file.files.push_back(cfile);
                    updated_comp_file_h699.push_back(comp_file);
                }
                fcfg_combine_str += cfile + " ";
            }












            HELL6_99MO_TYPE compare = buildFileH699.get(currentFileName + ".compare");
            std::vector <std::string> fcfg_compare_vector;
            if (compare.type == "string"){
                if (is_global) {
                    if (compare.string_value != "") cfg_compare.push_back(compare.string_value);
                    else {
                        cfg_compare.clear();
                    }
                }
                else {
                    if (compare.string_value != "") fcfg_compare.push_back(compare.string_value);
                    else {
                        fcfg_compare.clear();
                    }
                }
            }
            else if (compare.type == "array"){
                if (is_global) cfg_compare = compare.array_value;
                else fcfg_compare = compare.array_value;
            }
            else if (compare.type != H699_UNIDEF){
                ConsolePrint::print("Warning:- the compare key can only contain a string or an array, For file `" + currentFileName + "` in `" + file + "`. Ignoring this argument", ConsolePrint::Type::Warning);
            }
            for (std::string cfile : fcfg_compare){
                if (is_global){
                    break;
                }
                if (not (std::filesystem::exists(cfile) and std::filesystem::is_regular_file(cfile))){
                    ConsolePrint::print("Error:- the file `" + cfile + "` was not found to compare, For file `" + currentFileName + "` in `" + file + "`. Aborting the build!", ConsolePrint::Type::Error);
                    std::exit (3);
                }
                if (not gdbs::is_timestamp_same(cfile, hfile)){
                    can_ignore_this_file = false;
                    comp_file_t comp_file;
                    comp_file.h699_file_name = comp_file_name;
                    comp_file.filename = currentFileName;
                    comp_file.files.push_back(cfile);
                    updated_comp_file_h699.push_back(comp_file);
                }
            }

            // std::cout << can_ignore_this_file<<"\n";






            // The configuration which should only be executed for the updated files
            if (not can_ignore_this_file or is_global){
                if (not is_global) {
                    updatedFileVector.push_back (currentFileName);
                    updatedFileVector.push_back (fcfg_out);
                    is_anything_changed = true;
                }

                // compiler
                HELL6_99MO_TYPE compiler = buildFileH699.get(currentFileName + ".compiler");
                if (compiler.type == "string"){
                    if (is_global) cfg_compiler = compiler.string_value;
                    else fcfg_compiler = compiler.string_value;
                }
                else if (compiler.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- the compiler key can only contain a string, For file `" + currentFileName + "` in `" + file + "`. Ignoring this argument", ConsolePrint::Type::Warning);
                }








                // compiler_pargument
                HELL6_99MO_TYPE compiler_parguments = buildFileH699.get(currentFileName + ".compiler_parguments");
                if (compiler_parguments.type == "string"){
                    if (is_global) cfg_compiler_parguments = compiler_parguments.string_value;
                    else fcfg_compiler_parguments = compiler_parguments.string_value;
                }
                else if (compiler_parguments.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- the compiler_parguments key can only contain a string, For file `" + currentFileName + "` in `" + file + "`. Ignoring this argument", ConsolePrint::Type::Warning);
                }







                // compiler_argument
                HELL6_99MO_TYPE compiler_arguments = buildFileH699.get(currentFileName + ".compiler_arguments");
                if (compiler_arguments.type == "string"){
                    if (is_global) cfg_compiler_arguments = compiler_arguments.string_value;
                    else fcfg_compiler_arguments = compiler_arguments.string_value;
                }
                else if (compiler_arguments.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- the compiler_arguments key can only contain a string, For file `" + currentFileName + "` in `" + file + "`. Ignoring this argument", ConsolePrint::Type::Warning);
                }











                // Dynamic String Arguments
                // lib
                HELL6_99MO_TYPE lib = buildFileH699.get(currentFileName + ".lib");
                if (lib.type == "string"){
                    if (is_global) cfg_lib = "-l" + lib.string_value;
                    else fcfg_lib = "-l" + lib.string_value;
                }
                else if (lib.type == "array"){
                    std::string normalize = "";
                    for (std::string item : lib.array_value){
                        normalize += "-l" + item + " ";
                    }
                    if (is_global) cfg_lib = normalize;
                    else fcfg_lib = normalize;
                }
                else if (lib.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- the lib key can only contain a string or an array, For file `" + currentFileName + "` in `" + file + "`. Ignoring this argument", ConsolePrint::Type::Warning);
                }











                // include
                HELL6_99MO_TYPE include = buildFileH699.get(currentFileName + ".include");
                if (include.type == "string"){
                    if (is_global) cfg_include = "-I" + include.string_value;
                    else fcfg_include = "-I" + include.string_value;
                }
                else if (include.type == "array"){
                    std::string normalize = "";
                    for (std::string item : include.array_value){
                        normalize += "-I" + item + " ";
                    }
                    if (is_global) cfg_include = normalize;
                    else fcfg_include = normalize;
                }
                else if (include.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- the include key can only contain a string or an array, For file `" + currentFileName + "` in `" + file + "`. Ignoring this argument", ConsolePrint::Type::Warning);
                }









                // system
                HELL6_99MO_TYPE system = buildFileH699.get(currentFileName + ".system");
                if (system.type == "string"){
                    if (is_global) cfg_system = "-l" + system.string_value;
                    else fcfg_system = "-l" + system.string_value;
                }
                else if (system.type == "array"){
                    std::string normalize = "";
                    for (std::string item : system.array_value){
                        normalize += "-l" + item + " ";
                    }
                    if (is_global) cfg_system = normalize;
                    else fcfg_system = normalize;
                }
                else if (system.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- the system key can only contain a string or an array, For file `" + currentFileName + "` in `" + file + "`. Ignoring this argument", ConsolePrint::Type::Warning);
                }










                // psystem
                HELL6_99MO_TYPE psystem = buildFileH699.get(currentFileName + ".psystem");
                if (psystem.type == "string"){
                    if (is_global) cfg_psystem = "-l" + psystem.string_value;
                    else fcfg_psystem = "-l" + psystem.string_value;
                }
                else if (psystem.type == "array"){
                    std::string normalize = "";
                    for (std::string item : psystem.array_value){
                        normalize += "-l" + item + " ";
                    }
                    if (is_global) cfg_psystem = normalize;
                    else fcfg_psystem = normalize;
                }
                else if (psystem.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- the psystem key can only contain a string or an array, For file `" + currentFileName + "` in `" + file + "`. Ignoring this argument", ConsolePrint::Type::Warning);
                }











                // pkg_in
                HELL6_99MO_TYPE pkg_in = buildFileH699.get(currentFileName + ".pkg_in");
                if (pkg_in.type == "string"){
                    // std::cout << "Found: "<<currentFileName<<"\n";
                    if (pkg_in.string_value != ""){
                        if (std::system(std::string("pkg-config --cflags --libs " + pkg_in.string_value + " 1>" + PKGCONF_FILE).c_str()) != 0){
                            ConsolePrint::print("Error:- The pkg_in, package `" + pkg_in.string_value + "` was not found or pkg-config was not installed on your device. Please make sure to install the package and pkg-config before building this project. Aborting the build!", ConsolePrint::Type::Error);
                            std::exit(3);
                        }
                        std::ifstream pkgconfig_ifile (PKGCONF_FILE);
                        std::string content, temp;
                        while (std::getline(pkgconfig_ifile, temp))
                            content += temp + " ";
                        if (is_global) cfg_pkg_in = content;
                        else fcfg_pkg_in = content;
                    }
                    else {
                        if (is_global) cfg_pkg_in = "";
                        else fcfg_pkg_in = "";
                    }
                }
                else if (pkg_in.type == "array"){
                    // std::cout << "Found: "<<currentFileName<<"\n";

                    std::string content = "";
                    for (std::string item : pkg_in.array_value){
                        if (std::system(std::string("pkg-config --cflags --libs " + item + " 1>" + PKGCONF_FILE).c_str()) != 0){
                            ConsolePrint::print("Error:- The pkg_in, package `" + item + "` was not found or pkg-config was not installed on your device. Please make sure to install the package and pkg-config before building this project. Aborting the build!", ConsolePrint::Type::Error);
                            std::exit(3);
                        }
                        std::ifstream pkgconfig_ifile (PKGCONF_FILE);
                        std::string temp;
                        while (std::getline(pkgconfig_ifile, temp))
                            content += temp + " ";
                    }
                    if (is_global) cfg_pkg_in = content;
                    else fcfg_pkg_in = content;
                }
                else if (pkg_in.type != H699_UNIDEF){
                    ConsolePrint::print("Warning:- the pkg_in key can only contain a string or an array, For file `" + currentFileName + "` in `" + file + "`. Ignoring this argument", ConsolePrint::Type::Warning);
                }
                // End of the argument list





                if (not is_global){
                    core_file_t com;
                    std::filesystem::create_directory(fcfg_bin);
                    com.command = fcfg_psystem + "\n" + fcfg_compiler + " " + fcfg_compiler_parguments + " " + currentFileName  + " " + fcfg_combine_str + " " + fcfg_include + " " + fcfg_lib + " " + fcfg_pkg_in + " " + fcfg_compiler_arguments + " -o " + fcfg_out + "\n" + fcfg_system;
                    // std::cout <<com.command<<"\n"; 
                    
                    com.output = fcfg_out;
                    com.filename = currentFileName;
                    executer_list.push_back(com);
                }
            }
            can_ignore_this_file = false;
            is_global = false;
        }

        // Execute the executer from the executer list
        // Execute the ontrigger change before the executer
        for (std::string com : ontriggerchange){
            if (is_anything_changed) std::system(com.c_str());
        }

        std::vector <std::string> exec = gdbs::executer(executer_list, threads, show_command);
        // Execute the onchange before the executer
        for (std::string com : onchange){
            if (is_anything_changed) std::system(com.c_str());
        }

        // Execute the callback change before the executer
        for (std::string com : callback){
            std::system(com.c_str());
        }


        for (std::string file : updatedFileVector){
            bool is_err_file = false;
            for (std::string err : exec){
                if (file == err){
                    is_err_file = true;
                    break;
                }
            }

            if (not is_err_file) set_timestamp(file, openFilePathH699);
        }


        for (gdbs::comp_file_t comp : updated_comp_file_h699){
            bool is_err_file = false;
            for (std::string err : exec){
                if (comp.filename == err){
                    is_err_file = true;
                    break;
                }
            }
            if (not is_err_file){
                HELL6_99MO hfile(comp.h699_file_name);
                hfile.Parse();
                for (std::string comp_file : comp.files){
                    gdbs::set_timestamp(comp_file, hfile);
                }
                hfile.write(comp.h699_file_name);
            }
        }

        openFilePathH699.write(filePath);

        // Run the after change after everything
        for (std::string com : afterchange){
            if (is_anything_changed) std::system(com.c_str());
        }

        return status;
    }
}
