//
// Created by Dmitry Morozov on 29/11/22.
//

#include "Shell_Lib.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

Shell_lib::Shell_lib(int c_argc, char *c_argv[]) {

  if (c_argc != 3) {
    throw std::runtime_error("Usage: <disk_file> <n_blocks>");
  }

  std::string disk_file = c_argv[1];
  int n_blocks = std::stoi(c_argv[2]);

  disk_ = new Disk();

  try {
    disk_->open(disk_file, n_blocks);
  } catch (std::runtime_error &e) {
    throw std::runtime_error("Unable to open disk " + std::string(c_argv[1]) + ": " + e.what());
  }

  register_commands();

}

void Shell_lib::register_commands() {

  commands_map["debug"] = &Shell_lib::cmd_debug_;
  commands_map["format"] = &Shell_lib::cmd_format_;
  commands_map["help"] = &Shell_lib::cmd_help_;
  commands_map["quit"] = &Shell_lib::cmd_quit_;
//  commands_map["read"] = &Shell_lib::cmd_read_; // FIXME: Wrong linking
//  commands_map["write"] = &Shell_lib::cmd_write_;

}

void Shell_lib::Run() {

  std::vector<std::string> cmd_line;
  std::string line, cmd;

  while (cmd != "quit") {

    std::cout << "sfs> ";
    std::getline(std::cin, line);

    std::stringstream ss(line);

    while (ss >> cmd) {
      cmd_line.push_back(cmd);
    }

    if (cmd_line.empty()) {
      continue;
    }

    cmd = cmd_line[0];
    cmd_line.erase(cmd_line.begin());

    if (commands_map.find(cmd) != commands_map.end()) {

      auto cmd_func = commands_map[cmd];

      (this->*cmd_func)(cmd_line);

    } else {

      std::cout << "Unknown command: " << line << std::endl;
      std::cout << "Type 'help' for a list of commands." << std::endl;

    }
  }

}

void Shell_lib::cmd_debug_(const Command_Args &args) {
  std::cout << "debug" << std::endl;

  if (!args.empty()) {
    std::cout << "Usage: debug" << std::endl;
    return;
  }

}

void Shell_lib::cmd_format_(const Command_Args &args) {
  std::cout << "format" << std::endl;

  if (!args.empty()) {
    std::cout << "Usage: format" << std::endl;
    return;
  }

  FileSystem::format(disk_);

}

void Shell_lib::cmd_mount_(const Shell_lib::Command_Args &args) {

  std::cout << "mount" << std::endl;

  if (!args.empty()) {
    std::cout << "Usage: mount" << std::endl;
    return;
  }

}
void Shell_lib::cmd_cat_(const Shell_lib::Command_Args &args) {
  std::cout << "cat" << std::endl;

  if (args.size() != 1) {
    std::cout << "Usage: cat <file_name>" << std::endl;
    return;
  }

  std::string file_name = args[0];

}

bool Shell_lib::copyin_(size_t i_number, const std::string &path) {

  std::ifstream file(path, std::ios::binary);

  if (!file.is_open()) {
    std::cout << "Unable to open file " << path << std::endl;
    return false;
  }

  std::string buffer;
  std::string line;

  while (std::getline(file, line)) {
    buffer += line;
  }

  file.close();

  std::cout << "File " << path << " copied to disk." << std::endl;
  std::cout << buffer.size() << " bytes copied." << std::endl;

  return true;

}
bool Shell_lib::copyout_(const std::string &path, size_t i_number) {
  return false;
}

void Shell_lib::cmd_quit_(const Shell_lib::Command_Args &) {
  std::cout << "exit" << std::endl;
  exit(0);

}
void Shell_lib::cmd_help_(const Shell_lib::Command_Args &) {

  std::cout << "Help. List of commands :" << std::endl << std::endl;
  std::cout << "help" << std::endl;
  std::cout << "Available commands:" << std::endl;
  std::cout << "debug" << std::endl;
  std::cout << "format" << std::endl;
  std::cout << "mount" << std::endl;
  std::cout << "cat" << std::endl;
  std::cout << "copyout" << std::endl;
  std::cout << "create" << std::endl;
  std::cout << "remove" << std::endl;
  std::cout << "quit" << std::endl;
  std::cout << "stat" << std::endl;
  std::cout << "help" << std::endl;
  std::cout << "copyin" << std::endl;

}






