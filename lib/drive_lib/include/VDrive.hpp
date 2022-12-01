//
// Created by Dmitry Morozov on 30/11/22.
//

#ifndef DSFS_MAIN_VDRIVE_HPP
#define DSFS_MAIN_VDRIVE_HPP

#include "Sector.hpp"

#include <memory>

class VDrive {
public:
    VDrive();

    explicit VDrive(int bytes); //bytes are the multiple of 512

    ~VDrive();

    long totalSectors();

    [[nodiscard]] inline long usedSectors() const { return _usedSectors; }

    [[nodiscard]] inline long freeSectors() const { return _freeSectors; }

    inline Sector *sectors() { return _sectors; }

private:
    int _totalSectors;
    int _usedSectors;
    int _freeSectors;

    Sector *_sectors;

};


#endif //DSFS_MAIN_VDRIVE_HPP
