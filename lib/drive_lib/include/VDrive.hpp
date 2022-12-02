//
// Created by Dmitry Morozov on 30/11/22.
//

#ifndef DSFS_MAIN_VDRIVE_HPP
#define DSFS_MAIN_VDRIVE_HPP

#include "Sector.hpp"
#include "Block.hpp"

#include <memory>

class VDrive {
public:
    VDrive();

    explicit VDrive(int bytes); //bytes are the multiple of 512

    ~VDrive();

    long totalSectors();

    [[nodiscard]] inline long usedSectors() const { return _usedSectors; }

    [[nodiscard]] inline long freeSectors() const { return _freeSectors; }

    std::shared_ptr<Sector> getSector(long sectorNumber);

    std::shared_ptr<Block> getBlock(long blockNumber);

    long totalBlocks();

    void Format();


private:
    int _totalSectors;
    int _usedSectors;
    int _freeSectors;
    int _blockCount;

    bool _isFormatted;

    std::shared_ptr<Sector[]> _sectors;
    std::shared_ptr<Block[]> _blocks;


};


#endif //DSFS_MAIN_VDRIVE_HPP
