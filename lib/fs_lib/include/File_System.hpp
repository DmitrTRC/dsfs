//
// Created by Dmitry Morozov on 5/12/22.
//

#ifndef DSFS_DEV_FILE_SYSTEM_HPP
#define DSFS_DEV_FILE_SYSTEM_HPP


#include "Disk.hpp"

#include <cinttypes>
#include <cstdio>

class FileSystem {
public:
    const static std::uint32_t K_Control_Number = 0xf0f03410;
    const static std::uint32_t K_Inodes_Per_Block = 128;
    const static std::uint32_t K_Pointers_Per_Node = 5;
    const static std::uint32_t K_Pointers_Per_Block = 1024;

private:
    struct SuperBlock {        /// Superblock structure
        std::uint32_t ControlNumber;    /// File system magic number
        std::uint32_t Blocks;    /// Number of blocks in file system
        std::uint32_t InodeBlocks;    /// Number of blocks reserved for inodes
        std::uint32_t Inodes;    /// Number of inodes in file system
    };

    struct Inode {
        std::uint32_t Valid;        /// is Inode valid
        std::uint32_t Size;        /// Size of file
        std::uint32_t Direct[K_Pointers_Per_Node]; /// Direct pointer
        std::uint32_t Indirect;    /// Indirect pointer
    };

    union Block { // TODO: Make it std::variant
        SuperBlock Super;                /// Superblock
        Inode Inodes[K_Inodes_Per_Block];        /// Inode block
        std::uint32_t Pointers[K_Inodes_Per_Block];   /// Pointer block
        char Data[Disk::K_Block_Size];        /// Data block
    };

    // TODO: Internal helper functions

    // TODO: Internal member variables

public:
    static void debug(Disk *disk);

    static bool format(Disk *disk);

    bool mount(Disk *disk);

    ssize_t create();

    bool remove(size_t i_number);

    ssize_t stat(size_t i_number);

    ssize_t read(size_t i_number, char *data, size_t length, size_t offset);

    ssize_t write(size_t i_number, char *data, size_t length, size_t offset);
};


#endif //DSFS_DEV_FILE_SYSTEM_HPP
