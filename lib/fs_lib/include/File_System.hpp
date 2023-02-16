//
// Created by Dmitry Morozov on 5/12/22.
//

#ifndef DSFS_DEV_FILE_SYSTEM_HPP
#define DSFS_DEV_FILE_SYSTEM_HPP

#include "Disk.hpp"

#include <algorithm>
#include <any>
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace fs {

//TODO : Move to shell
//const std::string DEFAULT_DISK_PATH_ = "disk.img";
//const int DEFAULT_DISK_SIZE_ = 100;

template<std::size_t N>
std::ostream &operator<<(std::ostream &os, std::array<std::byte, N> const &v1) {

	std::for_each(v1.begin(), v1.end(), [&os](std::byte b) {
	  os << std::hex << static_cast<int>(b) << " ";
	});

	return os;
}

class FileSystem {
 public:
  const static uint32_t
	  MAGIC_NUMBER = 0xf0f03410; //Magic number helps in checking Validity of the FileSystem on disk
  const static uint32_t INODES_PER_BLOCK = 128; //    Number of Inodes which can be contained in a block of (4kb)
  const static uint32_t POINTERS_PER_INODE = 5; //    Number of Direct block pointers in an Inode Block
  const static uint32_t POINTERS_PER_BLOCK = 1024; //    Number of block pointers in one Indirect pointer
  const static uint32_t NAME_SIZE = 16; //    Max Name size for files/directories
  const static uint32_t ENTRIES_PER_DIR = 7; //    Number of Files/Directory entries within a Directory
  const static uint32_t DIR_PER_BLOCK = 8; //    Number of Directories per 4KB block

  FileSystem() = default;

  ~FileSystem();

  static void debug(const std::shared_ptr<Disk> &disk);

  static bool format(const std::shared_ptr<Disk> &disk);

  /**
     * @brief mounts the file system onto the disk
     * @param disk the disk to be mounted
     * @return true if the mount operation was successful; false otherwise
    */
  bool mount(const std::shared_ptr<Disk> &disk);


  //  Security Functions

//  bool set_password();
//
//  bool change_password();
//
//  bool remove_password();

  //  Directories and Files

  bool touch(const std::string name);
//
//  bool mkdir(char name[]);
//
//  bool rmdir(char name[]);
//
//  bool cd(char name[]);
//
//  bool ls();
//
//  bool rm(char name[]);
//
//  bool copyout(char name[], const char *path);
//
  bool copyin(const std::string path, const std::string name);
//
//  bool ls_dir(char name[]);
//
//  void exit();
//
//  void stat();

  // Test Functions
  auto get_cur_disk() { return fs_disk; }
  auto mounted() { return mounted_; }

  void set_cur_disk(std::shared_ptr<Disk> &disk);
 private:

  struct SuperBlock {
	SuperBlock()
		: MagicNumber(0), Blocks(0), InodeBlocks(0), DirBlocks(0), Inodes(0) {}

	u_int32_t MagicNumber;
	u_int32_t Blocks;
	u_int32_t InodeBlocks;
	u_int32_t Inodes;
	u_int32_t DirBlocks;
//	std::array<std::byte, 257> PasswordHash;
//	u_int32_t Protected;
  };

  struct Dirent {
	uint8_t type = 0;
	uint8_t valid = 0;
	uint32_t i_num = 0;
	std::array<char, NAME_SIZE> name{};
  };

  struct Directory {

	Directory() : Valid(0), I_num(-1), Name(), TableOfEntries() {}
	Directory(uint32_t valid,
			  uint32_t i_num,
			  std::array<char, NAME_SIZE> name,
			  std::array<Dirent, ENTRIES_PER_DIR> entries)
		: Valid(valid), I_num(i_num), Name(name), TableOfEntries(entries) {}

	uint16_t Valid;
	uint32_t I_num;
	std::array<char, NAME_SIZE> Name;
	std::array<Dirent, ENTRIES_PER_DIR> TableOfEntries;
  };

  struct Inode {
	uint32_t Valid;
	uint32_t Size;
	std::array<uint32_t, FileSystem::POINTERS_PER_INODE> Direct;
	uint32_t Indirect;

	Inode() : Valid(0), Size(0), Direct(), Indirect(0) {}
  };

  union Block {

	SuperBlock Super;
	std::array<Inode, FileSystem::INODES_PER_BLOCK> Inodes;
	std::array<std::uint32_t, FileSystem::POINTERS_PER_BLOCK> Pointers;
	std::array<std::byte, Disk::BLOCK_SIZE> Data;
	std::array<Directory, FileSystem::DIR_PER_BLOCK> Directories;

	Block() : Data() {}

  };

  // Internal member variables
  std::shared_ptr<Disk> fs_disk;

  std::vector<bool> free_blocks_;
  std::vector<int> inode_counter_;
  std::vector<uint32_t> dir_counter_;
  SuperBlock meta_data_;         //  Caches the SuperBlock to save a disk-read
  bool mounted_ = false;                       //  Boolean to check if the disk is mounted_ and saved

  // Base Layer Core Functions

  /**
   * @brief creates a new inode
   * @return the inumber of the newly created inode
  */
  ssize_t create();

  /**
   * @brief removes the inode
   * @param inumber index into the inode table of the inode to be removed
   * @return true if the remove operation was successful; false otherwise
  */
  bool remove(size_t);

  /**
   * @brief check size of an inode
   * @param i_number index into the inode table of inode whose size is to be determined
   * @return size of the inode; -1 if the inode is invalid
  */
  ssize_t stat(size_t i_number);

  /**
   * @brief reads the data from the inode
   * @param i_number index into the inode table of the inode to be read
   * @param offset Start reading from index = offset
   * @param length Number of bytes to be read
   * @param data data buffer
   * @return number of bytes read; -1 if the inode is invalid
  */
 public: // Debugging purposes
  ssize_t read(size_t i_number, std::vector<std::byte> &data, size_t offset);

 private:

//  ssize_t write(size_t inumber, std::vector<std::byte> &data, int length, size_t offset);

  //  Helper functions for Base Layer

  /**
   * @brief loads inode corresponding to i_number into node
   * @param i_number index into inode table
   * @param node pointer to inode
   * @return boolean value indicative of success of the load operation
  */
  bool load_inode(size_t i_number, Inode &node);
//
//  ssize_t allocate_free_block();
//

  /**
   * @brief Read the block from disk and changes the pointers accordingly
   * @param blocknum index into the free block bitmap
   * @param offset Start reading from index = offset
   * @param length Number of bytes to be read
   * @param data data buffer
   * @param ptr Buffer to store the read data
   * @return Void function; returns nothing
  */

  void read_helper(uint32_t blocknum,
				   int offset,
				   int *length, // Convert to smart pointer
				   std::vector<std::byte> &data,
				   std::vector<std::byte>::iterator &tail_ptr);

  ssize_t write_ret(size_t i_number, Inode &node, int ret);

//  void read_buffer(int offset, int *read, int length, char *data, uint32_t blocknum);
//
//  bool check_allocation(Inode *node,
//                        int read,
//                        int orig_offset,
//                        uint32_t &blocknum,
//                        bool write_indirect,
//                        Block indirect);
//
  uint32_t allocate_block();
//
  Directory curDir;
//
//  Directory add_dir_entry(Directory dir, uint32_t inum, uint32_t type, char name[]);
//
//  void write_dir_back(struct Directory dir);
//
//  int dir_lookup(Directory dir, char name[]);
//
//  Directory read_dir_from_offset(uint32_t offset);
//
//  Directory rmdir_helper(Directory parent, char name[]);
//
//  Directory rm_helper(Directory parent, char name[]);

};

} // namespace fs

#endif //DSFS_DEV_FILE_SYSTEM_HPP
