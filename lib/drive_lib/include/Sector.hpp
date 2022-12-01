//
// Created by Dmitry Morozov on 30/11/22.
//

#ifndef DSFS_MAIN_SECTOR_HPP
#define DSFS_MAIN_SECTOR_HPP


struct Sector {

    Sector();


    int _byteCount;
    char _bytes[512];

};


#endif //DSFS_MAIN_SECTOR_HPP
