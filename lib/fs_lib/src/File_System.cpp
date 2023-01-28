#include "File_System.hpp"
#include "sha256.hpp"

#include <algorithm>
#include <cmath>

#include <iostream>

using namespace std;

FileSystem::FileSystem() {
  mounted_ = false;
  fs_disk = std::make_shared<Disk>();

}

void FileSystem::debug(const std::shared_ptr<Disk> &disk) {
  Block b_super_block, b_char_block;

  std::shared_ptr<char> buffer = std::make_shared<char>(sizeof(Block));

  disk->read(0, buffer);

  b_char_block = buffer;

  unique_ptr<SuperBlock> sb_Ptr = std::make_unique<SuperBlock>(std::get<SuperBlock>(b_char_block));

  std::cout << "Superblock:" << std::endl;
  std::cout << "  Magic Number: " << sb_Ptr->MagicNumber << std::endl;
  std::cout << "  Blocks: " << sb_Ptr->Blocks << std::endl;
  std::cout << "  Inode Blocks: " << sb_Ptr->InodeBlocks << std::endl;
  std::cout << "  Inodes: " << sb_Ptr->Inodes << std::endl;
  std::cout << "  Dir Blocks: " << sb_Ptr->DirBlocks << std::endl;
  std::cout << "  Protected: " << sb_Ptr->Protected << std::endl;
//  std::cout << "  Password Hash: " << sb_Ptr->PasswordHash << std::endl;

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

  disk->write(0, std::get<std::shared_ptr<char>>(block));

  std::get<SuperBlock>(block).Protected = false;

  std::fill(std::get<SuperBlock>(block).PasswordHash.begin(), std::get<SuperBlock>(block).PasswordHash.end(), 0);

  //Clear all other blocks
  for (uint32_t i = 1; i < disk->size(); i++) {
    Block b_inode_block;
    for (uint32_t j = 0; j < FileSystem::INODES_PER_BLOCK; j++) {
      std::get<std::array<Inode, FileSystem::INODES_PER_BLOCK>>(b_inode_block)[j].Valid = false;
      std::get<std::array<Inode, FileSystem::INODES_PER_BLOCK>>(b_inode_block)[j].Size = 0;

      for (uint32_t k = 0; k < FileSystem::POINTERS_PER_INODE; k++) {
        std::get<std::array<Inode, FileSystem::INODES_PER_BLOCK>>(b_inode_block)[j].Direct[k] = 0;
      }
      std::get<std::array<Inode, FileSystem::INODES_PER_BLOCK>>(b_inode_block)[j].Indirect = 0;
    }
    disk->write(i, std::get<std::shared_ptr<char>>(b_inode_block));
  }

  //Clear all data blocks
  auto b_data_block = std::get<SuperBlock>(block);
  for (uint32_t i = 1 + b_data_block.InodeBlocks; i < b_data_block.Blocks - b_data_block.DirBlocks; i++) {
    Block data_block;
    for (uint32_t j = 0; j < Disk::BLOCK_SIZE; j++) {
      std::get<std::shared_ptr<char>>(data_block)[j] = 0;
    }
  }

  // Fill with 0

}

//bool FileSystem::mount(Disk *disk) {
//  if (disk->mounted()) return false;
//
//  Block block;
//  disk->read(0, block.Data);
//  if (block.Super.MagicNumber != MAGIC_NUMBER) return false;
//  if (block.Super.InodeBlocks != std::ceil((block.Super.Blocks * 1.00) / 10)) return false;
//  if (block.Super.Inodes != (block.Super.InodeBlocks * INODES_PER_BLOCK)) return false;
//  if (block.Super.DirBlocks != (uint32_t) std::ceil((int(block.Super.Blocks) * 1.00) / 100)) return false;
//
//  if (block.Super.Protected) {
//    char pass[1000], line[1000];
//    printf("Enter password: ");
//    if (fgets(line, BUFSIZ, stdin) == NULL) {
//      return false;
//    }
//
//    sscanf(line, "%s", pass);
//    SHA256_CTX hasher;
//    //TODO Check hash!!!
//    printf("Disk Unlocked\n");
//    return true;
//
//  }
//
//  disk->mount();
//  fs_disk = disk;
//
//  meta_data_ = block.Super;
//
//  free_blocks_.resize(meta_data_.Blocks, false);
//
//  inode_counter_.resize(meta_data_.InodeBlocks, 0);
//
//  free_blocks_[0] = true;
//
//  for (uint32_t i = 1; i <= meta_data_.InodeBlocks; i++) {
//    disk->read(i, block.Data);
//
//    for (uint32_t j = 0; j < INODES_PER_BLOCK; j++) {
//      if (block.Inodes[j].Valid) {
//        inode_counter_[i - 1]++;
//
//        if (block.Inodes[j].Valid) free_blocks_[i] = true;
//
//        for (uint32_t k = 0; k < POINTERS_PER_INODE; k++) {
//          if (block.Inodes[j].Direct[k]) {
//            if (block.Inodes[j].Direct[k] < meta_data_.Blocks)
//              free_blocks_[block.Inodes[j].Direct[k]] = true;
//            else
//              return false;
//          }
//        }
//
//        if (block.Inodes[j].Indirect) {
//          if (block.Inodes[j].Indirect < meta_data_.Blocks) {
//            free_blocks_[block.Inodes[j].Indirect] = true;
//            Block indirect;
//            fs_disk->read(block.Inodes[j].Indirect, indirect.Data);
//            for (uint32_t k = 0; k < POINTERS_PER_BLOCK; k++) {
//              if (indirect.Pointers[k] < meta_data_.Blocks) {
//                free_blocks_[indirect.Pointers[k]] = true;
//              } else return false;
//            }
//          } else
//            return false;
//        }
//      }
//    }
//  }
//
//  dir_counter_.resize(meta_data_.DirBlocks, 0);
//
//  Block dirblock;
//  for (uint32_t dirs = 0; dirs < meta_data_.DirBlocks; dirs++) {
//    disk->read(meta_data_.Blocks - 1 - dirs, dirblock.Data);
//    for (uint32_t offset = 0; offset < FileSystem::DIR_PER_BLOCK; offset++) {
//      if (dirblock.Directories[offset].Valid == 1) {
//        dir_counter_[dirs]++;
//      }
//    }
//    if (dirs == 0) {
//      curr_dir = dirblock.Directories[0];
//    }
//  }
//
//  mounted_ = true;
//
//  return true;
//}
//
//ssize_t FileSystem::create() {
//  if (!mounted_) return false;
//
//  Block block;
//  fs_disk->read(0, block.Data);
//
//  for (uint32_t i = 1; i <= meta_data_.InodeBlocks; i++) {
//    if (inode_counter_[i - 1] == INODES_PER_BLOCK) continue;
//    else fs_disk->read(i, block.Data);
//
//    for (uint32_t j = 0; j < INODES_PER_BLOCK; j++) {
//      if (!block.Inodes[j].Valid) {
//        block.Inodes[j].Valid = true;
//        block.Inodes[j].Size = 0;
//        block.Inodes[j].Indirect = 0;
//        for (int ii = 0; ii < 5; ii++) {
//          block.Inodes[j].Direct[ii] = 0;
//        }
//        free_blocks_[i] = true;
//        inode_counter_[i - 1]++;
//
//        fs_disk->write(i, block.Data);
//
//        return (((i - 1) * INODES_PER_BLOCK) + j);
//      }
//    }
//  }
//
//  return -1;
//}
//
//bool FileSystem::load_inode(size_t inumber, Inode *node) {
//  if (!mounted_) return false;
//  if ((inumber > meta_data_.Inodes) || (inumber < 1)) { return false; }
//
//  Block block;
//
//  int i = inumber / INODES_PER_BLOCK;
//  int j = inumber % INODES_PER_BLOCK;
//
//  if (inode_counter_[i]) {
//    fs_disk->read(i + 1, block.Data);
//    if (block.Inodes[j].Valid) {
//      *node = block.Inodes[j];
//      return true;
//    }
//  }
//
//  return false;
//}
//
//bool FileSystem::remove(size_t inumber) {
//  if (!mounted_) return false;
//
//  Inode node;
//
//  if (load_inode(inumber, &node)) {
//    node.Valid = false;
//    node.Size = 0;
//
//    if (!(--inode_counter_[inumber / INODES_PER_BLOCK])) {
//      free_blocks_[inumber / INODES_PER_BLOCK + 1] = false;
//    }
//
//    for (uint32_t i = 0; i < POINTERS_PER_INODE; i++) {
//      free_blocks_[node.Direct[i]] = false;
//      node.Direct[i] = 0;
//    }
//
//    if (node.Indirect) {
//      Block indirect;
//      fs_disk->read(node.Indirect, indirect.Data);
//      free_blocks_[node.Indirect] = false;
//      node.Indirect = 0;
//
//      for (uint32_t i = 0; i < POINTERS_PER_BLOCK; i++) {
//        if (indirect.Pointers[i]) free_blocks_[indirect.Pointers[i]] = false;
//      }
//    }
//
//    Block block;
//    fs_disk->read(inumber / INODES_PER_BLOCK + 1, block.Data);
//    block.Inodes[inumber % INODES_PER_BLOCK] = node;
//    fs_disk->write(inumber / INODES_PER_BLOCK + 1, block.Data);
//
//    return true;
//  }
//
//  return false;
//}
//
//ssize_t FileSystem::stat(size_t inumber) {
//  if (!mounted_) return -1;
//
//  Inode node;
//
//  if (load_inode(inumber, &node)) return node.Size;
//
//  return -1;
//}
//
//void FileSystem::read_helper(uint32_t blocknum, int offset, int *length, char **data, char **ptr) {
//  fs_disk->read(blocknum, *ptr);
//  *data += offset;
//  *ptr += Disk::BLOCK_SIZE;
//  *length -= (Disk::BLOCK_SIZE - offset);
//
//  return;
//}
//
//ssize_t FileSystem::read(size_t inumber, char *data, int length, size_t offset) {
//  if (!mounted_) return -1;
//
//  int size_inode = stat(inumber);
//
//  if ((int) offset >= size_inode) return 0;
//  else if (length + (int) offset > size_inode) length = size_inode - offset;
//
//  Inode node;
//
//  char *ptr = data;
//  int to_read = length;
//
//  if (load_inode(inumber, &node)) {
//    if (offset < POINTERS_PER_INODE * Disk::BLOCK_SIZE) {
//      uint32_t direct_node = offset / Disk::BLOCK_SIZE;
//      offset %= Disk::BLOCK_SIZE;
//
//      if (node.Direct[direct_node]) {
//        read_helper(node.Direct[direct_node++], offset, &length, &data, &ptr);
//        while (length > 0 && direct_node < POINTERS_PER_INODE && node.Direct[direct_node]) {
//          read_helper(node.Direct[direct_node++], 0, &length, &data, &ptr);
//        }
//
//        if (length <= 0) return to_read;
//        else {
//          if (direct_node == POINTERS_PER_INODE && node.Indirect) {
//            Block indirect;
//            fs_disk->read(node.Indirect, indirect.Data);
//
//            for (uint32_t i = 0; i < POINTERS_PER_BLOCK; i++) {
//              if (indirect.Pointers[i] && length > 0) {
//                read_helper(indirect.Pointers[i], 0, &length, &data, &ptr);
//              } else break;
//            }
//
//            if (length <= 0) return to_read;
//            else {
//              return (to_read - length);
//            }
//          } else {
//            return (to_read - length);
//          }
//        }
//      } else {
//        return 0;
//      }
//    } else {
//      if (node.Indirect) {
//        offset -= (POINTERS_PER_INODE * Disk::BLOCK_SIZE);
//        uint32_t indirect_node = offset / Disk::BLOCK_SIZE;
//        offset %= Disk::BLOCK_SIZE;
//
//        Block indirect;
//        fs_disk->read(node.Indirect, indirect.Data);
//
//        if (indirect.Pointers[indirect_node] && length > 0) {
//          read_helper(indirect.Pointers[indirect_node++], offset, &length, &data, &ptr);
//        }
//
//        for (uint32_t i = indirect_node; i < POINTERS_PER_BLOCK; i++) {
//          if (indirect.Pointers[i] && length > 0) {
//            read_helper(indirect.Pointers[i], 0, &length, &data, &ptr);
//          } else break;
//        }
//
//        if (length <= 0) return to_read;
//        else {
//          return (to_read - length);
//        }
//      } else {
//        return 0;
//      }
//    }
//  }
//
//  return -1;
//}
//
//uint32_t FileSystem::allocate_block() {
//  if (!mounted_) return 0;
//
//  for (uint32_t i = meta_data_.InodeBlocks + 1; i < meta_data_.Blocks; i++) {
//    if (free_blocks_[i] == 0) {
//      free_blocks_[i] = true;
//      return (uint32_t) i;
//    }
//  }
//
//  return 0;
//}
//
//ssize_t FileSystem::write_ret(size_t inumber, Inode *node, int ret) {
//  if (!mounted_) return -1;
//
//  int i = inumber / INODES_PER_BLOCK;
//  int j = inumber % INODES_PER_BLOCK;
//
//  Block block;
//  fs_disk->read(i + 1, block.Data);
//  block.Inodes[j] = *node;
//  fs_disk->write(i + 1, block.Data);
//
//  return (ssize_t) ret;
//}
//
//void FileSystem::read_buffer(int offset, int *read, int length, char *data, uint32_t blocknum) {
//  if (!mounted_) return;
//
//  char *ptr = (char *) calloc(Disk::BLOCK_SIZE, sizeof(char));
//
//  for (int i = offset; i < (int) Disk::BLOCK_SIZE && *read < length; i++) {
//    ptr[i] = data[*read];
//    *read = *read + 1;
//  }
//  fs_disk->write(blocknum, ptr);
//
//  free(ptr);
//
//  return;
//}
//
//bool FileSystem::check_allocation(Inode *node,
//                                  int read,
//                                  int orig_offset,
//                                  uint32_t &blocknum,
//                                  bool write_indirect,
//                                  Block indirect) {
//  if (!mounted_) return false;
//
//  if (!blocknum) {
//    blocknum = allocate_block();
//    if (!blocknum) {
//      node->Size = read + orig_offset;
//      if (write_indirect) fs_disk->write(node->Indirect, indirect.Data);
//      return false;
//    }
//  }
//
//  return true;
//}
//
//ssize_t FileSystem::write(size_t inumber, char *data, int length, size_t offset) {
//  if (!mounted_) return -1;
//
//  Inode node;
//  Block indirect;
//  int read = 0;
//  int orig_offset = offset;
//
//  if (length + offset > (POINTERS_PER_BLOCK + POINTERS_PER_INODE) * Disk::BLOCK_SIZE) {
//    return -1;
//  }
//
//  if (!load_inode(inumber, &node)) {
//    node.Valid = true;
//    node.Size = length + offset;
//    for (uint32_t ii = 0; ii < POINTERS_PER_INODE; ii++) {
//      node.Direct[ii] = 0;
//    }
//    node.Indirect = 0;
//    inode_counter_[inumber / INODES_PER_BLOCK]++;
//    free_blocks_[inumber / INODES_PER_BLOCK + 1] = true;
//  } else {
//    node.Size = max((int) node.Size, length + (int) offset);
//  }
//
//  if (offset < POINTERS_PER_INODE * Disk::BLOCK_SIZE) {
//    int direct_node = offset / Disk::BLOCK_SIZE;
//    offset %= Disk::BLOCK_SIZE;
//
//    if (!check_allocation(&node, read, orig_offset, node.Direct[direct_node], false, indirect)) {
//      return write_ret(inumber, &node, read);
//    }
//    read_buffer(offset, &read, length, data, node.Direct[direct_node++]);
//
//    if (read == length) return write_ret(inumber, &node, length);
//    else {
//      for (int i = direct_node; i < (int) POINTERS_PER_INODE; i++) {
//        if (!check_allocation(&node, read, orig_offset, node.Direct[direct_node], false, indirect)) {
//          return write_ret(inumber, &node, read);
//        }
//        read_buffer(0, &read, length, data, node.Direct[direct_node++]);
//
//        if (read == length) return write_ret(inumber, &node, length);
//      }
//
//      if (node.Indirect) fs_disk->read(node.Indirect, indirect.Data);
//      else {
//        if (!check_allocation(&node, read, orig_offset, node.Indirect, false, indirect)) {
//          return write_ret(inumber, &node, read);
//        }
//        fs_disk->read(node.Indirect, indirect.Data);
//
//        for (int i = 0; i < (int) POINTERS_PER_BLOCK; i++) {
//          indirect.Pointers[i] = 0;
//        }
//      }
//
//      for (int j = 0; j < (int) POINTERS_PER_BLOCK; j++) {
//        if (!check_allocation(&node, read, orig_offset, indirect.Pointers[j], true, indirect)) {
//          return write_ret(inumber, &node, read);
//        }
//        read_buffer(0, &read, length, data, indirect.Pointers[j]);
//
//        if (read == length) {
//          fs_disk->write(node.Indirect, indirect.Data);
//          return write_ret(inumber, &node, length);
//        }
//      }
//
//      fs_disk->write(node.Indirect, indirect.Data);
//      return write_ret(inumber, &node, read);
//    }
//  } else {
//    offset -= (Disk::BLOCK_SIZE * POINTERS_PER_INODE);
//    int indirect_node = offset / Disk::BLOCK_SIZE;
//    offset %= Disk::BLOCK_SIZE;
//
//    if (node.Indirect) fs_disk->read(node.Indirect, indirect.Data);
//    else {
//      if (!check_allocation(&node, read, orig_offset, node.Indirect, false, indirect)) {
//        return write_ret(inumber, &node, read);
//      }
//      fs_disk->read(node.Indirect, indirect.Data);
//
//      for (int i = 0; i < (int) POINTERS_PER_BLOCK; i++) {
//        indirect.Pointers[i] = 0;
//      }
//    }
//
//    if (!check_allocation(&node, read, orig_offset, indirect.Pointers[indirect_node], true, indirect)) {
//      return write_ret(inumber, &node, read);
//    }
//    read_buffer(offset, &read, length, data, indirect.Pointers[indirect_node++]);
//
//    if (read == length) {
//      fs_disk->write(node.Indirect, indirect.Data);
//      return write_ret(inumber, &node, length);
//    } else {
//      for (int j = indirect_node; j < (int) POINTERS_PER_BLOCK; j++) {
//        if (!check_allocation(&node, read, orig_offset, indirect.Pointers[j], true, indirect)) {
//          return write_ret(inumber, &node, read);
//        }
//        read_buffer(0, &read, length, data, indirect.Pointers[j]);
//
//        if (read == length) {
//          fs_disk->write(node.Indirect, indirect.Data);
//          return write_ret(inumber, &node, length);
//        }
//      }
//
//      fs_disk->write(node.Indirect, indirect.Data);
//      return write_ret(inumber, &node, read);
//    }
//  }
//
//  return -1;
//}
