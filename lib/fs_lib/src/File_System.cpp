//
// Created by Dmitry Morozov on 5/12/22.
//

#include "File_System.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>


// Debug file system -----------------------------------------------------------

void FileSystem::debug(Disk *disk) {

  Block block{};

  // Read Superblock
  disk->read(0, block.Data);

  std::cout << "Superblock:" << std::endl;

  if (block.Super.MagicNumber == MAGIC_NUMBER) {

    std::cout << "Control number is valid" << std::endl;

  } else {

    std::cout << "Control number is invalid" << std::endl;
    return;

  }

  std::cout << block.Super.Blocks << " blocks" << std::endl;
  std::cout << block.Super.InodeBlocks << " inode blocks" << std::endl;
  std::cout << block.Super.Inodes << " inodes" << std::endl;


  /// Read Inode blocks
  for (size_t i = 0; i < block.Super.InodeBlocks; ++i) {

    disk->read(1 + i, block.Data);

    for (size_t j = 0; j < K_Inodes_Per_Block; ++j) {

      if (block.Inodes[j].Valid) {

        std::cout << "Inode " << i * K_Inodes_Per_Block + j << ":" << std::endl;
        std::cout << "Size : " << block.Inodes[j].Size << " bytes" << std::endl;

        std::cout << "Direct Blocks : " << std::endl;

        for (size_t k = 0; k < K_Pointers_Per_Node; ++k) {

          if (block.Inodes[j].Direct[k]) {

            std::cout << "Direct[" << k << "] = " << block.Inodes[j].Direct[k] << std::endl;

          }

        }

        if (block.Inodes[j].Indirect) {

          std::cout << "Indirect block :  " << block.Inodes[j].Indirect << std::endl;

          Block indirect_block{};

          disk->read(block.Inodes[j].Indirect, indirect_block.Data);

          for (size_t k = 0; k < K_Pointers_Per_Block; ++k) {

            if (indirect_block.Pointers[k]) {

              std::cout << "Indirect[" << k << "] = " << indirect_block.Pointers[k] << std::endl;

            }

          }

          std::cout << std::endl;

        }

      }

    }

  }

}

/**
 * @brief Format file system on disk
 * @param disk
 * @return
 */
bool FileSystem::format(Disk *disk) {

  Block block{};

  block.Super.MagicNumber = MAGIC_NUMBER;
  block.Super.Blocks = disk->blocks();
  block.Super.InodeBlocks = (block.Super.Blocks - 1) / K_Inodes_Per_Block + 1;
  block.Super.Inodes = block.Super.InodeBlocks * K_Inodes_Per_Block;

  disk->write(0, block.Data);



  /// Format Inode blocks
  std::fill(block.Data, block.Data + Disk::Block_Size, 0);

  for (size_t i = 1; i < block.Super.Blocks; ++i) {

    disk->write(i, block.Data);

  }

  return true;
}

/**
 * @brief Mount file system on disk
 * @param disk
 * @return
 */
bool FileSystem::mount(Disk *disk) {
  // Read superblock
  Block block{};
  disk->read(0, block.Data);

  // Set device and mount
  if (block.Super.MagicNumber == MAGIC_NUMBER) {
//    m_disk = disk;
//    m_blocks = block.Super.Blocks;
//    m_inode_blocks = block.Super.InodeBlocks;
//    m_inodes = block.Super.Inodes;
    return true;
  }

  // Copy metadata

  // Allocate free block bitmap

  return true;
}

// Create inode ----------------------------------------------------------------

ssize_t FileSystem::create() {
  // Locate free inode in inode table

  // Record inode if found
  return 0;
}

// Remove inode ----------------------------------------------------------------

bool FileSystem::remove(size_t i_number) {
  // Load inode information

  // Free direct blocks

  // Free indirect blocks

  // Clear inode in inode table
  return true;
}

// Inode stat ------------------------------------------------------------------

ssize_t FileSystem::stat(size_t inumber) {
  // Load inode information
  return 0;
}

// Read from inode -------------------------------------------------------------

ssize_t FileSystem::read(size_t inumber, char *data, size_t length, size_t offset) {
  // Load inode information

  // Adjust length

  // Read block and copy to data
  return 0;
}

// Write to inode --------------------------------------------------------------

ssize_t FileSystem::write(size_t inumber, char *data, size_t length, size_t offset) {
  // Load inode

  // Write block and copy to data
  return 0;
}

