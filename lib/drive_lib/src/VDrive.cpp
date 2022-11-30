//
// Created by Dmitry Morozov on 30/11/22.
//

#include "VDrive.hpp"

#include <memory>
#include <type_traits>

VDrive::VDrive(int bytes) {

    if (bytes % 512 != 0) {
        throw std::invalid_argument("The number of VDrive bytes must be a multiple of 512");
    }

    _totalSectors = bytes / 512;
    _usedSectors = 0;
    _freeSectors = _totalSectors;

    _sectors = new Sector[_totalSectors];

}

VDrive::VDrive() : VDrive(1024) {}


