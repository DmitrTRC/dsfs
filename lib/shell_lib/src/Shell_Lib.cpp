//
// Created by Dmitry Morozov on 29/11/22.
//

#include "Shell_Lib.hpp"

#include <iostream>
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

  commands_map.insert({"debug", &Shell_lib::debug_});
  commands_map.insert({"format", &Shell_lib::format_});
  commands_map.insert({"mount", &Shell_lib::mount_});
  commands_map.insert({"cat", &Shell_lib::cat_});
  commands_map.insert({"copyout", &Shell_lib::copyout_});
  commands_map.insert({"create", &Shell_lib::create_});
  commands_map.insert({"remove", &Shell_lib::remove_});
  commands_map.insert({"quit", &Shell_lib::exit_});
  commands_map.insert({"stat", &Shell_lib::stat_});
  commands_map.insert({"help", &Shell_lib::help_});
  commands_map.insert({"copyin", &Shell_lib::copyin_});

}

void Shell_lib::Run() {

  std::vector<std::string> cmd_line;
  std::string line, cmd;

  while (cmd != "stop") {

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




