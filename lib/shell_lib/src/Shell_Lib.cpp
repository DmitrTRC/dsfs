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


  commands_map["debug"]=&Shell_lib::debug_;
  commands_map["format"] = &Shell_lib::format_;
  commands_map["mount"] = &Shell_lib::mount_;
    commands_map["cat"] = &Shell_lib::cat_;
    commands_map["copyout"] = &Shell_lib::copyout_;
    commands_map["create"] = &Shell_lib::create_;
    commands_map["remove"] = &Shell_lib::remove_;
    commands_map["stat"] = &Shell_lib::stat_;
    commands_map["copyin"] = &Shell_lib::copyin_;
    commands_map["help"] = &Shell_lib::help_;


}




