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

Shell_lib::Shell_lib(int argc, char *argv[]) {

  if (argc != 3) {
    throw std::runtime_error("Usage: <disk_file> <n_blocks>");
  }

  std::string disk_file = argv[1];
  int n_blocks = std::stoi(argv[2]);

  try {
    disk_.open(disk_file, n_blocks);
  } catch (std::runtime_error &e) {
    throw std::runtime_error("Unable to open disk " + std::string(argv[1]) + ": " + e.what());
  }

  register_commands();

}

Shell_lib::Shell_lib() : Shell_lib(0, nullptr) {
//TODO: Set default values.
}

void Shell_lib::register_commands() {

  commands_map.insert({"debug", &Shell_lib::cmd_debug_});
  commands_map.insert({"format", &Shell_lib::cmd_format_});
  commands_map.insert({"mount", &Shell_lib::cmd_mount_});
  commands_map.insert({"cat", &Shell_lib::cmd_cat_});
//  commands_map.insert({"copyout", &Shell_lib::cmd_copyout_});
//  commands_map.insert({"create", &Shell_lib::cmd_create_});
//  commands_map.insert({"remove", &Shell_lib::cmd_remove_});
  commands_map.insert({"quit", &Shell_lib::cmd_exit_});
//  commands_map.insert({"stat", &Shell_lib::cmd_stat_});
  commands_map.insert({"help", &Shell_lib::cmd_help_});
  // commands_map.insert({"copyin", &Shell_lib::cmd_copyin_});

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

      commands_map[cmd](cmd_line);

    } else {

      std::cout << "Unknown command: " << line << std::endl;
      std::cout << "Type 'help' for a list of commands." << std::endl;

    }
  }

}

void Shell_lib::cmd_debug_(Command_Args &args) {
  std::cout << "debug" << std::endl;

  if (!args.empty()) {
    std::cout << "Usage: debug" << std::endl;
    return;
  }

  FileSystem::debug(&disk_);

}
void Shell_lib::cmd_format_(Shell_lib::Command_Args &args) {
  std::cout << "format" << std::endl;

  if (!args.empty()) {
    std::cout << "Usage: format" << std::endl;
    return;
  }

  if (FileSystem::format(&disk_)) {
    std::cout << "Disk formatted successfully." << std::endl;
  } else {
    std::cout << "Disk format failed." << std::endl;
  }

}

void Shell_lib::cmd_mount_(Shell_lib::Command_Args &args) {

  std::cout << "mount" << std::endl;

  if (!args.empty()) {
    std::cout << "Usage: mount" << std::endl;
    return;
  }

  if (Shell_lib::fs_.mount(&disk_)) {
    std::cout << "Disk mounted successfully." << std::endl;
  } else {
    std::cout << "Disk mount failed." << std::endl;
  }

}
void Shell_lib::cmd_cat_(Shell_lib::Command_Args &args) {
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

  if (Shell_lib::fs_.write(i_number, const_cast<char *>(buffer.c_str()), buffer.size(), 0) < 0) {
    std::cout << "Unable to write to file " << path << std::endl;
    return false;
  }

  std::cout << "File " << path << " copied to disk." << std::endl;
  std::cout << buffer.size() << " bytes copied." << std::endl;

  return true;

}
bool Shell_lib::copyout_(const std::string &, size_t) {
  return false;
}
void Shell_lib::cmd_exit_(Shell_lib::Command_Args &) {
  std::cout << "exit" << std::endl;
  exit(0);

}
void Shell_lib::cmd_help_(Shell_lib::Command_Args &) {

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






