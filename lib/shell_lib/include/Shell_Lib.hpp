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
  static std::string currentDirectory_;

  static Disk disk_;
  static FileSystem fs_;

  //Command type
  using CommandFunc = void (*)(Command_Args &);

  //Commands map
  std::unordered_map<std::string, CommandFunc> commands_map;

  static bool copyin_(size_t, const std::string &);

  static bool copyout_(const std::string &, size_t);

  //Command prototypes
  static void cmd_debug_(Command_Args &);

  static void cmd_format_(Command_Args &);

  static void cmd_mount_(Command_Args &);

  static void cmd_cat_(Command_Args &);

//  static void cmd_copyout_(Command_Args &);
//
//  static void cmd_create_(Command_Args &);
//
//  static void cmd_remove_(Command_Args &);
//
//  static void cmd_stat_(Command_Args &);
//
//  static void cmd_copyin_(Command_Args &);

  static void cmd_help_(Command_Args &);

  static void cmd_exit_(Command_Args &);

  void register_commands();

};

#endif //DSFS_MAIN_SHELL_LIB_HPP
