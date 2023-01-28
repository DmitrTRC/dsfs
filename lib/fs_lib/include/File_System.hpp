//
// Created by Dmitry Morozov on 5/12/22.
//

#ifndef DSFS_DEV_FILE_SYSTEM_HPP
#define DSFS_DEV_FILE_SYSTEM_HPP

#include "Disk.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

using namespace std;

class FileSystem {
 public:
  const static uint32_t MAGIC_NUMBER = 0xf0f03410; //Magic number helps in checking Validity of the FileSystem on disk
  const static uint32_t INODES_PER_BLOCK = 128; //    Number of Inodes which can be contained in a block of (4kb)
  const static uint32_t POINTERS_PER_INODE = 5; //    Number of Direct block pointers in an Inode Block
  const static uint32_t POINTERS_PER_BLOCK = 1024; //    Number of block pointers in one Indirect pointer
  const static uint32_t NAME_SIZE = 16; //    Max Name size for files/directories
  const static uint32_t ENTRIES_PER_DIR = 7; //    Number of Files/Directory entries within a Directory
  const static uint32_t DIR_PER_BLOCK = 8; //    Number of Directories per 4KB block



  FileSystem();

  ~FileSystem();

  static void debug(Disk *disk);

  static bool format(Disk *disk);

  bool mount(Disk *disk);


  //  Security Functions

  bool set_password();

  bool change_password();

  bool remove_password();

  //  Directories and Files

  bool touch(char name[]);

  bool mkdir(char name[]);

  bool rmdir(char name[]);

  bool cd(char name[]);

  bool ls();

  bool rm(char name[]);

  bool copyout(char name[], const char *path);

  bool copyin(const char *path, char name[]);

  bool ls_dir(char name[]);

  void exit();

  void stat();

 private:
  struct SuperBlock {
    uint32_t MagicNumber;
    uint32_t Blocks;
    uint32_t InodeBlocks;
    uint32_t DirBlocks;
    uint32_t Inodes;
    uint32_t Protected;
    std::array<char, 257> PasswordHash;
  };

  struct Dirent {
    uint8_t type;
    uint8_t valid;
    uint32_t i_num;
    std::array<char, NAME_SIZE> name;
  };

  struct Directory {
    uint16_t Valid;
    uint32_t I_num;
    std::array<char, NAME_SIZE> Name;
    std::array<Dirent, ENTRIES_PER_DIR> Entries;
  };

  struct Inode {
    uint32_t Valid;
    uint32_t Size;
    std::array<uint32_t, FileSystem::POINTERS_PER_INODE> Direct;
    uint32_t Indirect;
  };

  std::variant<SuperBlock, std::array<Inode, FileSystem::INODES_PER_BLOCK>,
               std::array<std::uint32_t, FileSystem::POINTERS_PER_BLOCK>,
               std::array<char, Disk::BLOCK_SIZE>,
               std::array<Directory, FileSystem::DIR_PER_BLOCK>> Block;

  // Internal member variables
  std::shared_ptr<Disk> fs_disk;

  std::vector<bool> free_blocks_;
  std::vector<int> inode_counter_;
  std::vector<uint32_t> dir_counter_;
  struct SuperBlock meta_data_;         //  Caches the SuperBlock to save a disk-read
  bool mounted_;                       //  Boolean to check if the disk is mounted_ and saved

  // Base Layer Core Functions
  ssize_t create();

  bool remove(size_t);

  ssize_t stat(size_t inumber);

  ssize_t read(size_t inumber, char *data, int length, size_t offset);

  ssize_t write(size_t inumber, char *data, int length, size_t offset);

  //  Helper functions for Layer 1
  bool load_inode(size_t inumber, Inode *node);

  ssize_t allocate_free_block();

  void read_helper(uint32_t blocknum, int offset, int *length, char **data, char **ptr);

  ssize_t write_ret(size_t inumber, Inode *node, int ret);

  void read_buffer(int offset, int *read, int length, char *data, uint32_t blocknum);

  bool check_allocation(Inode *node,
                        int read,
                        int orig_offset,
                        uint32_t &blocknum,
                        bool write_indirect,
                        Block indirect);

  uint32_t allocate_block();

  Directory curr_dir;

  Directory add_dir_entry(Directory dir, uint32_t inum, uint32_t type, char name[]);

  void write_dir_back(struct Directory dir);

  int dir_lookup(Directory dir, char name[]);

  Directory read_dir_from_offset(uint32_t offset);

  Directory rmdir_helper(Directory parent, char name[]);

  Directory rm_helper(Directory parent, char name[]);

};

#endif //DSFS_DEV_FILE_SYSTEM_HPP
