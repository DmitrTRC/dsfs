//
// Created by Dmitry Morozov on 30/11/22.
//

#ifndef DSFS_MAIN_VDRIVE_HPP
#define DSFS_MAIN_VDRIVE_HPP

#include "Sector.h"

class VDrive {
    int totalSectors;
    int usedSectors;
    int freeSectors;

    std::shared_ptr <Sector> sectors;

};


#endif //DSFS_MAIN_VDRIVE_HPP
