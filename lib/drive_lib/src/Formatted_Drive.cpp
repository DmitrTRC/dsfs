//
// Created by Dmitry Morozov on 1/12/22.
//

#include "Block.hpp"
#include "VDrive.hpp"
#include "Formatted_Drive.hpp"

FormattedDrive::FormattedDrive( VDrive &drive ) {

    _blockCount = drive.totalSectors() / 8;
    _blocks = new Block[_blockCount];

    for ( int i = 0; i < _blockCount; i++ ) {
        _blocks[i] = Block( drive.getSector( i * 8 ) );
    }

}