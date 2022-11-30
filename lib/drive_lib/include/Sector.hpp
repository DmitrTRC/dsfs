//
// Created by Dmitry Morozov on 30/11/22.
//

#ifndef DSFS_MAIN_SECTOR_HPP
#define DSFS_MAIN_SECTOR_HPP


class Sector {
public:

    Sector();

private:
    int _byteCount;
    char _bytes[512];

    friend class VDrive;
};


#endif //DSFS_MAIN_SECTOR_HPP
