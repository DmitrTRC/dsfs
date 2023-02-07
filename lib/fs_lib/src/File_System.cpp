#include "File_System.hpp"
#include "sha256.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include <iostream>
#include <memory>

using namespace fs;

void FileSystem::debug(const std::shared_ptr<Disk> &disk) {
  Block b_super_block, b_byte_block;

  std::array<std::byte, Disk::BLOCK_SIZE> buffer = {};

  disk->read(0, buffer);

  b_byte_block = buffer;

  std::unique_ptr<SuperBlock> sb_Ptr = std::make_unique<SuperBlock>(std::get<SuperBlock>(b_byte_block));

  std::cout << "SuperBlock:" << std::endl;
  std::cout << "  Magic Number: " << sb_Ptr->MagicNumber << std::endl;
  std::cout << "  Blocks: " << sb_Ptr->Blocks << std::endl;
  std::cout << "  Inode Blocks: " << sb_Ptr->InodeBlocks << std::endl;
  std::cout << "  Inodes: " << sb_Ptr->Inodes << std::endl;
  std::cout << "  Dir Blocks: " << sb_Ptr->DirBlocks << std::endl;
  std::cout << "  Protected: " << sb_Ptr->Protected << std::endl;
  std::cout << "  Password Hash: " << sb_Ptr->PasswordHash.data() << std::endl;

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
  if (disk->mounted()) return false;

  Block block;
  std::get<SuperBlock>(block).MagicNumber = FileSystem::MAGIC_NUMBER;
  std::get<SuperBlock>(block).Blocks = static_cast<std::uint32_t>(disk->size());
  std::get<SuperBlock>(block).InodeBlocks = static_cast<std::uint32_t>(std::ceil((disk->size() * 1.00) / 10));
  std::get<SuperBlock>(block).Inodes = std::get<SuperBlock>(block).InodeBlocks * FileSystem::INODES_PER_BLOCK;
  std::get<SuperBlock>(block).DirBlocks = static_cast<std::uint32_t>(std::ceil((disk->size() * 1.00) / 100));

  //FIXME: Exception generated here "std::bad_variant_access"

  std::array<std::byte, Disk::BLOCK_SIZE> data = reinterpret_cast<std::array<std::byte, Disk::BLOCK_SIZE> &>(block);
  disk->write(0, data);

  std::get<SuperBlock>(block).Protected = 0;

  std::fill(std::get<SuperBlock>(block).PasswordHash.begin(), std::get<SuperBlock>(block).PasswordHash.end(), 0);

  //Clear all other blocks
  for (auto i = 1; i < disk->size(); i++) {
    Block b_inode_block;

    for (uint32_t j = 0; j < FileSystem::INODES_PER_BLOCK; j++) {
      std::get<std::array<Inode, FileSystem::INODES_PER_BLOCK>>(b_inode_block)[j].Valid = false;
      std::get<std::array<Inode, FileSystem::INODES_PER_BLOCK>>(b_inode_block)[j].Size = 0;

      for (uint32_t k = 0; k < FileSystem::POINTERS_PER_INODE; k++) {
        std::get<std::array<Inode, FileSystem::INODES_PER_BLOCK>>(b_inode_block)[j].Direct[k] = 0;
      }

      std::get<std::array<Inode, FileSystem::INODES_PER_BLOCK>>(b_inode_block)[j].Indirect = 0;

    }

    disk->write(i, std::get<std::array<std::byte, Disk::BLOCK_SIZE>>(b_inode_block));
  }

  //Use std::span to clear all data blocks

  //Clear all data blocks
  auto b_data_block = std::get<SuperBlock>(block);
  for (uint32_t i = 1 + b_data_block.InodeBlocks; i < b_data_block.Blocks - b_data_block.DirBlocks; i++) {
    Block data_block;
    std::fill(std::get<std::array<std::byte, Disk::BLOCK_SIZE>>(data_block).begin(),
              std::get<std::array<std::byte, Disk::BLOCK_SIZE>>(data_block).end(), std::byte(0));
    disk->write(static_cast<int>(i), std::get<std::array<std::byte, Disk::BLOCK_SIZE>>(data_block));
  }

  //Clear all directory blocks
  auto b_dir_block = std::get<SuperBlock>(block);

  //FIXME: This is not working
  for (uint32_t i = b_dir_block.Blocks - b_dir_block.DirBlocks; i < b_dir_block.Blocks; i++) {
    Block data_block;
    Directory dir;

    dir.Name[0] = '\0';
    dir.I_num = 0;
    dir.Valid = 0;

    //TEST: This is not working
    std::fill(std::get<std::array<Directory, FileSystem::DIR_PER_BLOCK>>(data_block).begin(),
              std::get<std::array<Directory, FileSystem::DIR_PER_BLOCK>>(data_block).end(), dir);

    disk->write(static_cast<int>(i), std::get<std::array<std::byte, Disk::BLOCK_SIZE>>(data_block));
  }

  //Create Root directory
  Directory root;

  root.Name[0] = '/';
  root.Name[1] = '\0';

  root.I_num = 0;
  root.Valid = 1;

  Dirent temp;

  temp.i_num = 0;
  temp.type = 0;
  temp.valid = 1;

  temp.name[0] = '.';
  temp.name[1] = '\0';

  root.Entries[0] = temp;

  temp.name[0] = '.';
  temp.name[1] = '.';
  temp.name[2] = '\0';

  root.Entries[1] = temp;

  Block dir_block;

  std::get<std::array<Directory, FileSystem::DIR_PER_BLOCK>>(dir_block)[0] = root;

  //TEST: This is not working -1?
  disk->write(static_cast<int>(b_dir_block.Blocks - b_dir_block.DirBlocks),
              std::get<std::array<std::byte, Disk::BLOCK_SIZE>>(dir_block));

  return true;
}
FileSystem::~FileSystem() {
  if (fs_disk->mounted()) {
    fs_disk->unmount();
  }

}

FileSystem::FileSystem(const std::string &path, size_t size) : mounted_(false), fs_disk(nullptr) {
  fs_disk = std::make_shared<Disk>(path, size);

}

