#include "File_System.hpp"
#include "sha256.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include <iostream>
#include <memory>

namespace fs {

void FileSystem::debug(const std::shared_ptr<Disk> &disk) {
  Block block;

  disk->read(0, block.Data);

  std::cout << "SuperBlock:" << std::endl;

  if (block.Super.MagicNumber != MAGIC_NUMBER) {
    std::cout << "  Magic Number: " << block.Super.MagicNumber << " Is INVALID! Exiting... " << std::endl;
    return;
  }

  std::cout << "  Magic Number: " << block.Super.MagicNumber << " is Valid" << std::endl;
  std::cout << "  Blocks: " << block.Super.Blocks << std::endl;
  std::cout << "  Inode Blocks: " << block.Super.InodeBlocks << std::endl;
  std::cout << "  Inodes: " << block.Super.Inodes << std::endl;

  //Iterate over InodeBlocks. Print out the Inodes size. directory blocks, indirect blocks, and data blocks

  uint32_t inode_count = 0; //TODO: Refactor to closure

  for (auto i = 1; i <= block.Super.InodeBlocks; ++i) {
    disk->read(i, block.Data);
    for (auto &node : block.Inodes) {
      if (node.Valid) {
        std::cout << "  Inode: " << inode_count << " Size: " << node.Size << std::endl;
        std::cout << "    Direct Blocks: ";
        for (auto &direct : node.Direct) {
          if (direct) {
            std::cout << direct << " ";
          }
          std::cout << std::endl;
        }
        if (node.Indirect) {
          std::cout << "    Indirect Blocks: ";

          disk->read(node.Indirect, block.Data);

          for (auto &indirect : block.Pointers) {
            if (indirect) {
              std::cout << indirect << " ";
            }
          }
          std::cout << std::endl;
        }
      }
    }
  }

  std::cout << "  Inode Size: " << sizeof(Inode) << std::endl;
  std::cout << "  Directory Size: " << sizeof(Directory) << std::endl;
  std::cout << "  Indirect Size: " << sizeof(std::uint32_t) << std::endl;
  std::cout << "  Data Size: " << sizeof(std::byte) << std::endl;

  std::cout << "  Directory Blocks: " << block.Super.DirBlocks << std::endl;

  std::cout << "  Protected: " << block.Super.Protected << std::endl;
  std::cout << "  PasswordHash Size: " << block.Super.PasswordHash.size() << std::endl;
}
/*
 * Format the disk
 * write superblock
 * Reinitialising password protection
 * clear all other blocks
 * clear individual Inodes
 * clear all direct Pointers
 * clear indirect Pointer
 * Free Data Blocks
 * Free Directory Blocks
 * Create Root directory
 * Create table entries for "." and ".."
 * Empty the directories
 */
bool FileSystem::format(const std::shared_ptr<Disk> &disk) {
//  if (disk->mounted()) return false;
//
//  Block block;
//  std::get<SuperBlock>(block).MagicNumber = FileSystem::MAGIC_NUMBER;
//  std::get<SuperBlock>(block).Blocks = static_cast<std::uint32_t>(disk->size());
//  std::get<SuperBlock>(block).InodeBlocks = static_cast<std::uint32_t>(std::ceil((disk->size() * 1.00) / 10));
//  std::get<SuperBlock>(block).Inodes = std::get<SuperBlock>(block).InodeBlocks * FileSystem::INODES_PER_BLOCK;
//  std::get<SuperBlock>(block).DirBlocks = static_cast<std::uint32_t>(std::ceil((disk->size() * 1.00) / 100));
//
//  //FIXME: Exception generated here "std::bad_variant_access"
//
//  std::array<std::byte, Disk::BLOCK_SIZE> data = reinterpret_cast<std::array<std::byte, Disk::BLOCK_SIZE> &>(block);
//  //
//  disk->write(0, data);
//
//  std::get<SuperBlock>(block).Protected = 0;
//
//  std::fill(std::get<SuperBlock>(block).PasswordHash.begin(), std::get<SuperBlock>(block).PasswordHash.end(), 0);
//
//  //Clear all other blocks
//  for (auto i = 1; i < disk->size(); i++) {
//    Block b_inode_block;
//
//    for (uint32_t j = 0; j < FileSystem::INODES_PER_BLOCK; j++) {
//      std::get<std::array<Inode, FileSystem::INODES_PER_BLOCK>>(b_inode_block)[j].Valid = false;
//      std::get<std::array<Inode, FileSystem::INODES_PER_BLOCK>>(b_inode_block)[j].Size = 0;
//
//      for (uint32_t k = 0; k < FileSystem::POINTERS_PER_INODE; k++) {
//        std::get<std::array<Inode, FileSystem::INODES_PER_BLOCK>>(b_inode_block)[j].Direct[k] = 0;
//      }
//
//      std::get<std::array<Inode, FileSystem::INODES_PER_BLOCK>>(b_inode_block)[j].Indirect = 0;
//
//    }
//
//    disk->write(i, std::get<std::array<std::byte, Disk::BLOCK_SIZE>>(b_inode_block));
//  }
//
//  //Use std::span to clear all data blocks
//
//  //Clear all data blocks
//  auto b_data_block = std::get<SuperBlock>(block);
//  for (uint32_t i = 1 + b_data_block.InodeBlocks; i < b_data_block.Blocks - b_data_block.DirBlocks; i++) {
//    Block data_block;
//    std::fill(std::get<std::array<std::byte, Disk::BLOCK_SIZE>>(data_block).begin(),
//              std::get<std::array<std::byte, Disk::BLOCK_SIZE>>(data_block).end(), std::byte(0));
//    disk->write(static_cast<int>(i), std::get<std::array<std::byte, Disk::BLOCK_SIZE>>(data_block));
//  }
//
//  //Clear all directory blocks
//  auto b_dir_block = std::get<SuperBlock>(block);
//
//  //FIXME: This is not working
//  for (uint32_t i = b_dir_block.Blocks - b_dir_block.DirBlocks; i < b_dir_block.Blocks; i++) {
//    Block data_block;
//    Directory dir;
//
//    dir.Name[0] = '\0';
//    dir.I_num = 0;
//    dir.Valid = 0;
//
//    //TEST: This is not working
//    std::fill(std::get<std::array<Directory, FileSystem::DIR_PER_BLOCK>>(data_block).begin(),
//              std::get<std::array<Directory, FileSystem::DIR_PER_BLOCK>>(data_block).end(), dir);
//
//    disk->write(static_cast<int>(i), std::get<std::array<std::byte, Disk::BLOCK_SIZE>>(data_block));
//  }
//
//  //Create Root directory
//  Directory root;
//
//  root.Name[0] = '/';
//  root.Name[1] = '\0';
//
//  root.I_num = 0;
//  root.Valid = 1;
//
//  Dirent temp;
//
//  temp.i_num = 0;
//  temp.type = 0;
//  temp.valid = 1;
//
//  temp.name[0] = '.';
//  temp.name[1] = '\0';
//
//  root.Entries[0] = temp;
//
//  temp.name[0] = '.';
//  temp.name[1] = '.';
//  temp.name[2] = '\0';
//
//  root.Entries[1] = temp;
//
//  Block dir_block;
//
//  std::get<std::array<Directory, FileSystem::DIR_PER_BLOCK>>(dir_block)[0] = root;
//
//  //TEST: This is not working -1?
//  disk->write(static_cast<int>(b_dir_block.Blocks - b_dir_block.DirBlocks),
//              std::get<std::array<std::byte, Disk::BLOCK_SIZE>>(dir_block));

  return true;
}
FileSystem::~FileSystem() {
  if (fs_disk->mounted()) {
    fs_disk->unmount();
  }

}
bool FileSystem::mount(const std::shared_ptr<Disk> &disk) {

  if (disk->mounted()) return false;



  //FIXME: Exception generated here "std::bad_variant_access"
  std::array<std::byte, Disk::BLOCK_SIZE> data{};

  disk->read(0, data);

//  Block block{data};
//
//  std::cout << "Block type: " << block.index() << std::endl; //Block type: 3

  //FIXME: Exception generated here "std::bad_variant_access"
  SuperBlock super_block(data); //Replace 3 with name of the variant

  //Debug purposes. Simplify the code
  if (super_block.MagicNumber != FileSystem::MAGIC_NUMBER) return false;
  if (super_block.InodeBlocks != static_cast<std::uint32_t>(std::ceil((disk->size() * 1.00) / 10))) return false;
  if (super_block.Inodes != super_block.InodeBlocks * FileSystem::INODES_PER_BLOCK) return false;
  if (super_block.DirBlocks != static_cast<std::uint32_t>(std::ceil((disk->size() * 1.00) / 100))) return false;

  // Password check
  //TODO: Implement password check
  //

  disk->mount();

  fs_disk = disk;

  meta_data_ = super_block;

  free_blocks_.resize(meta_data_.Blocks, false);

  inode_counter_.resize(meta_data_.Inodes, 0);

  free_blocks_[0] = true;

  //Read all inodes

//  for (uint32_t i = 1; i < meta_data_.InodeBlocks + 1; i++) {
//    Block b_inode_block;
//    disk->read(i, std::get<std::array<std::byte, Disk::BLOCK_SIZE>>(b_inode_block));
//
//    for (uint32_t j = 0; j < FileSystem::INODES_PER_BLOCK; j++) {
//      auto inode = std::get<std::array<Inode, FileSystem::INODES_PER_BLOCK>>(b_inode_block)[j];
//
//      if (inode.Valid) {
//        inode_counter_[i - 1]++;
//
//        if (inode.Valid) {
//          free_blocks_[i] = true;
//        }
//
//        for (uint32_t k = 0; k < FileSystem::POINTERS_PER_INODE; k++) {
//
//          if (inode.Direct[k] != 0) {
//            if (inode.Direct[k] < meta_data_.Blocks) {
//              free_blocks_[inode.Direct[k]] = true;
//            } else {
//              return false;
//            }
//
//          }
//        }
//        return false;
//--------------------- ??? ---------------------
//        if (inode.Indirect != 0) {
////          if (inode.Indirect < meta_data_.Blocks) {
//            free_blocks_[inode.Indirect] = true;
//
//            Block b_indirect_block;
//
//            for (uint32_t dirs = 0; dirs < meta_data_.DirBlocks; dirs++) {
//              disk->read(meta_data_.Blocks - 1 - dirs,
//                         std::get<std::array<Directory, FileSystem::DIR_PER_BLOCK>>(b_indirect_block));
//
//              for (uint32_t offset = 0; offset < FileSystem::DIR_PER_BLOCK; offset++) {
//                if (b_indirect_block.)
//              }
//            }
//          }
//
//        }
//

  return false;
}

//TODO: Refactor this function to STL algorithms
FileSystem::SuperBlock::SuperBlock(std::array<std::byte, Disk::BLOCK_SIZE> &block) {
  std::memcpy(&MagicNumber, &block[0], sizeof(MagicNumber));
  std::memcpy(&Blocks, &block[4], sizeof(Blocks));
  std::memcpy(&InodeBlocks, &block[8], sizeof(InodeBlocks));
  std::memcpy(&Inodes, &block[12], sizeof(Inodes));
  std::memcpy(&DirBlocks, &block[16], sizeof(DirBlocks));
  std::memcpy(&Protected, &block[20], sizeof(Protected));
  std::memcpy(&PasswordHash, &block[24], sizeof(PasswordHash));

}
} // namespace fs
