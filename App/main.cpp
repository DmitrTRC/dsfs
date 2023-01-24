
#include "Shell_Lib.hpp"

#include <iostream>

const char *DEFAULT_DISK_FILE = "demo.dat";
const char *DEFAULT_N_BLOCKS = "100";

int main(int argc, char *argv[]) {

  std::cout << "DHFS Starting" << std::endl;

  std::unique_ptr<Shell_lib> p_fs_shell;

  if (argc == 3) {

    try {
      p_fs_shell = std::make_unique<Shell_lib>(argc, argv);
    } catch (std::runtime_error &e) {
      std::cerr << "Unable to start DHFS: " << e.what() << std::endl;
      std::cerr << e.what() << std::endl;

      return 1;
    }

  } else {
    std::cout << "Wrong number of arguments. Using default values: " << DEFAULT_DISK_FILE << " " << DEFAULT_N_BLOCKS
              << std::endl;
    int c_argc = 3;
    char *c_argv[] = {argv[0], (char *) DEFAULT_DISK_FILE, (char *) DEFAULT_N_BLOCKS};
    p_fs_shell = std::make_unique<Shell_lib>(c_argc, c_argv);

  }

  p_fs_shell->Run();

  return 0;

}