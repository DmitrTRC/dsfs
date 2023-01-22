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

  Disk disk_;
  FileSystem fs_;

  //Command type
  using CommandFunc = void (*)(Command_Args);

  //Commands map
  std::unordered_map<std::string, CommandFunc> commands_map;

  //Command prototypes
  static void debug_(Command_Args);

    static void format_(Command_Args);

    static void mount_(Command_Args);

    static void cat_(Command_Args);

    static void copyout_(Command_Args);

    static void create_(Command_Args);

    static void remove_(Command_Args);

    static void stat_(Command_Args);

    static void copyin_(Command_Args);

    static void help_(Command_Args);

    static void exit_(Command_Args);

    void register_commands();

};


#endif //DSFS_MAIN_SHELL_LIB_HPP
