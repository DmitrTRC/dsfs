//
// Created by Dmitry Morozov on 30/11/22.
//

#include "VDrive.hpp"
#include "Block.hpp"


#include <iostream>
#include <memory>
#include <type_traits>

VDrive::VDrive(int bytes) {

    if (bytes % 512 != 0) {
        throw std::invalid_argument("The number of VDrive bytes must be a multiple of 512");
    }

    _totalSectors = bytes / 512;
    _usedSectors = 0;
    _freeSectors = _totalSectors;
    _isFormatted = false;
    _blockCount = 0;

    _sectors = std::make_unique<Sector[]>(_totalSectors);

}

VDrive::VDrive() : VDrive(1024) {}

VDrive::~VDrive() {

    std::cout << "VDrive destructor" << std::endl;
}

long VDrive::totalSectors() {

    return _totalSectors;
}

void VDrive::Format() {

    _isFormatted = true;
    _blockCount = _totalSectors / 8;


    _blocks = std::make_unique<Block[]>(_blockCount);

    for (int i = 0; i < _blockCount; i++) {
        auto block = std::make_unique<Block>(_sectors.get() + i * 8);
        _blocks[i] = *block;

    }


}

Sector *VDrive::getSector(long sector) {

    if (sector > _totalSectors) {
        throw std::invalid_argument("Sector number is out of range");
    }

    return _sectors.get() + sector;
}


Block *VDrive::getBlock(long block) {

    if (block > _blockCount) {
        throw std::invalid_argument("Block number is out of range");
    }

    return _blocks.get() + block;
}

long VDrive::totalBlocks() {

    return _blockCount;
}


