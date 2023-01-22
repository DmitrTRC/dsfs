
#include "Shell_Lib.hpp"

#include <iostream>

int main() {

  std::cout << "DHFS Starting" << std::endl;

  Shell_lib fs_shell;

  fs_shell.Run();

  return 0;
}
