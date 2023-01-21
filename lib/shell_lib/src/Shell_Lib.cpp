//
// Created by Dmitry Morozov on 29/11/22.
//

#include "Shell_Lib.hpp"

#include <sstream>
#include <string>
#include <stdexcept>


Shell_lib::Shell_lib(int argc, char *argv[]) {

    if (argc != 3) {
        throw std::runtime_error("Usage: <disk_file> <n_blocks>");
    }

    std::string disk_file = argv[1];
    int n_blocks = std::stoi(argv[2]);

    try {
        disk.open(disk_file, n_blocks);
    } catch (std::runtime_error &e) {
        throw std::runtime_error("Unable to open disk " + std::string(argv[1]) + ": " + e.what());
    }

    register_commands();

}

Shell_lib::Shell_lib() : Shell_lib(0, nullptr) {
//TODO: Set default values.
}

void Shell_lib::register_commands() {

    commands_map["debug"] = &Shell_lib::debug_;
    commands["format"] = &Shell_lib::format_;
    commands["mount"] = &Shell_lib::do_mount;
    commands["cat"] = &Shell_lib::do_cat;
    commands["copyout"] = &Shell_lib::do_copyout;
    commands["create"] = &Shell_lib::do_create;
    commands["remove"] = &Shell_lib::do_remove;
    commands["stat"] = &Shell_lib::do_stat;
    commands["copyin"] = &Shell_lib::do_copyin;
    commands["help"] = &Shell_lib::do_help;


}




