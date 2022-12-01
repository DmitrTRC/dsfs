//
// Created by Dmitry Morozov on 29/11/22.
//

#include "Shell_Lib.hpp"
#include "Fs_Lib.hpp"
#include "VDrive.hpp"
#include "Formatted_Drive.hpp"

#include <iostream>

void Shell_lib::info() {

    std::cout << "shell_lib::info() Running" << std::endl;

}

Shell_lib::Shell_lib() {

    info();

    Fs_lib fs_lib;

    VDrive hdd(1024 * 1024 * 1024);

    FormattedDrive f_hdd(hdd);

    std::cout << "Total Sectors: " << hdd.totalSectors() << std::endl;

    std::cout << "Sector Size: " << sizeof(Sector) << std::endl;

    std::cout << "Block Size: " << sizeof(Block) << std::endl;

    std::cout << "Block Count: " << f_hdd.blockCount() << std::endl;


}


