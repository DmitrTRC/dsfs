//
// Created by Dmitry Morozov on 29/11/22.
//

#ifndef DSFS_MAIN_SHELL_LIB_HPP
#define DSFS_MAIN_SHELL_LIB_HPP

#include "Disk.hpp"
#include "File_System.hpp"

#include <unordered_map>

class Shell_lib {

 public:
  Shell_lib();

  Shell_lib(int argc, char *argv[]);

  void Run();

 private:

  using Command_Args = std::vector<std::string>;

  std::unordered_map<std::string, void (Shell_lib::*)(const Command_Args &)> commands_map;

  void cmd_debug_(const Command_Args &);

  void cmd_format_(const Command_Args &);

  void cmd_mount_(const Command_Args &);

  void cmd_help_(const Command_Args &);

  void cmd_cat_(const Command_Args &);

  void cmd_quit_(const Command_Args &);

  void cmd_read_(const Command_Args &);

  void cmd_write_(const Command_Args &);

  void register_commands();

  Disk *disk_;

  bool copyin_(size_t i_number, const string &path);
  bool copyout_(const string &, size_t);
};

#endif //DSFS_MAIN_SHELL_LIB_HPP
