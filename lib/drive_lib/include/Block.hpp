//
// Created by Dmitry Morozov on 1/12/22.
//

#ifndef DSFS_MAIN_BLOCK_HPP
#define DSFS_MAIN_BLOCK_HPP

#include "Sector.hpp"

#include <memory>
#include <array>

struct Block {

    int usedSectors;
    std::array<std::shared_ptr<Sector>, 8> sectors;

};


#endif //DSFS_MAIN_BLOCK_HPP
