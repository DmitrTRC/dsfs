//
// Created by Dmitry Morozov on 29/11/22.
//

#include "Shell_Lib.hpp"
#include "Fs_Lib.hpp"
#include "Drive_Lib.hpp"

#include <iostream>

void Shell_lib::info() {

    std::cout << "shell_lib::info() Running" << std::endl;

}

Shell_lib::Shell_lib() {

    info();

    Fs_lib fs_lib;

    Drive_lib drive_lib;

}


