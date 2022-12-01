//
// Created by Dmitry Morozov on 1/12/22.
//

#ifndef DSFS_DEV_FORMATTED_DRIVE_HPP
#define DSFS_DEV_FORMATTED_DRIVE_HPP

#include "Block.hpp"
#include "VDrive.hpp"

#include <array>

class FormattedDrive {
public:
    explicit FormattedDrive(VDrive &drive);

    inline int blockCount() {return _blockCount;}

private:
    int _blockCount;
    Block *_blocks;

};


#endif //DSFS_DEV_FORMATTED_DRIVE_HPP
