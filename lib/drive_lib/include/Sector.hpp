//
// Created by Dmitry Morozov on 30/11/22.
//

#ifndef DSFS_MAIN_SECTOR_HPP
#define DSFS_MAIN_SECTOR_HPP

#include <array>
#include <cinttypes>


struct Sector {

    Sector();


    int _byteCount;
    std::array<std::uint8_t, 512> _bytes{};

};


#endif //DSFS_MAIN_SECTOR_HPP
