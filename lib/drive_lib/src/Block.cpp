//
// Created by Dmitry Morozov on 1/12/22.
//

#include "Block.hpp"

Block::Block(Sector *baseSectorAddress) {

    _usedSectors = 0;

    _baseSectorAddress = baseSectorAddress;


}

Block::Block() {

    _usedSectors = 0;

    _baseSectorAddress = nullptr;

}

