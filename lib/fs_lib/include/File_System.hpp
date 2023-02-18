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

/**
 * @brief	  Helper function to print std::array<std::byte, N> to std::ostream
 * @tparam N  Size of the array
 * @param os  Output stream
 * @param v1  Array to print
 * @return	  Output stream
 */
template<std::size_t N>
std::ostream &operator<<(std::ostream &os, std::array<std::byte, N> const &v1) {

	std::for_each(v1.begin(), v1.end(), [&os](std::byte b) {
	  os << std::hex << static_cast<int>(b) << " ";
	});

	return os;
}

/**
 * @brief 	Class for File System.
 * 			Contains all the functions for File System operations
 * 			Used by the Shell to perform File System operations
 */
class FileSystem {
 public:
  const static uint32_t MAGIC_NUMBER = 0xf0f03410; //Magic number helps in checking Validity of the FileSystem on disk
  const static uint32_t INODES_PER_BLOCK = 128; //    Number of Inodes which can be contained in a block of (4kb)
  const static uint32_t POINTERS_PER_INODE = 5; //    Number of Direct block pointers in an Inode Block
  const static uint32_t POINTERS_PER_BLOCK = 1024; //    Number of block pointers in one Indirect pointer
  const static uint32_t NAME_SIZE = 16; //    Max Name size for files/directories
  const static uint32_t ENTRIES_PER_DIR = 7; //    Number of Files/Directory entries within a Directory
  const static uint32_t DIR_PER_BLOCK = 8; //    Number of Directories per 4KB block

  FileSystem() = default; //	Default Constructor

  /**
   * @brief Destructor
   */
  ~FileSystem();

/**
 * @brief		Debug function to print the contents of the FileSystem
 * @param disk  The disk to be debugged
 */
  static void debug(const std::shared_ptr<Disk> &disk);

  /**
   * @brief 		Formats the disk to create a FileSystem
   * @param disk 	the disk to be formatted
   * @return 		true if the format operation was successful; false otherwise
   */
  static bool format(const std::shared_ptr<Disk> &disk);

  /**
   * @brief mounts the file system onto the disk
   * @param disk the disk to be mounted
   * @return true if the mount operation was successful; false otherwise
   */
  bool mount(const std::shared_ptr<Disk> &disk);


  //  Security Functions

  /**
   * @brief Sets the password for the file system
   * @return true if the password was set successfully; false otherwise
   */
  bool set_password();

  /**
   * @brief Changes the password for the file system
   * @return true if the password was changed successfully; false otherwise
   */
  bool change_password();

  /**
   * @brief Removes the password for the file system
   * @return true if the password was removed successfully; false otherwise
   */
  bool remove_password();

  //  Directories and Files

  /**
   * @brief Creates a file with the given name
   * @param name the name of the file to be created
   * @return true if the file was created successfully; false otherwise
   */
  bool touch(const std::array<char, NAME_SIZE> &name);

  /**
   * @brief Creates a directory with the given name
   * @param name the name of the directory to be created
   * @return true if the directory was created successfully; false otherwise
   */
  bool mkdir(const std::string &name);

  /**
   * @brief Removes the directory with the given name
   * @param name the name of the directory to be removed
   * @return true if the directory was removed successfully; false otherwise
   */
  bool rmdir(const std::array<char, NAME_SIZE> &name);

  /**
   * @brief Changes the current working directory to the directory with the given name
   * @param name the name of the directory to be changed to
   * @return true if the directory was changed successfully; false otherwise
   */
  bool cd(const std::array<char, NAME_SIZE> &name);

  /**
   * @brief Prints the contents of the current working directory
   * @return true if the contents were printed successfully; false otherwise
   */
  bool ls();

  /**
   * @brief Removes the file with the given name
   * @param name the name of the file to be removed
   * @return true if the file was removed successfully; false otherwise
   */
  bool rm(const std::array<char, NAME_SIZE> &name);

  /**
   * @brief Copies the file with the given name to the given path
   * @param path the path to copy the file to
   * @param name the name of the file to be copied
   * @return true if the file was copied successfully; false otherwise
   */
  bool copyout(const std::string &path, const std::string &name);

  /**
   * @brief Copies the file with the given name from the given path
   * @param path the path to copy the file from
   * @param name the name of the file to be copied
   * @return true if the file was copied successfully; false otherwise
   */
  bool copyin(std::string path, std::string name);

  /**
   * @brief 	  	Prints the contents of the directory with the given name
   * @param name  	the name of the directory to be printed
   * @return    	true if the contents were printed successfully; false otherwise
   */
  bool ls_dir(const std::string &name);

  /**
   * @brief 	Exits the file system
   */
  void exit();

  /**
   * @brief 	Displays the status of the file system
   */
  void stat();

  // Test Functions

  /**
   * @brief  Returns the current disk
   * @return the current disk
   */
  auto get_cur_disk() { return fs_disk; }

  /**
   * @brief  Checks if the file system is mounted
   * @return true if the file system is mounted; false otherwise
   */
  [[nodiscard]] auto mounted() const { return mounted_; }

  /**
   * @brief Sets the current disk
   * @param disk
   */
  void set_cur_disk(std::shared_ptr<Disk> &disk);
 private:

  /**
   * @brief Structure for the SuperBlock
   * @details The SuperBlock is the first block of the disk and contains information about the file system
   * @details The SuperBlock is 4KB in size
   */
  struct SuperBlock {
	SuperBlock() : MagicNumber(0), Blocks(0), InodeBlocks(0), DirBlocks(0), Inodes(0), PasswordHash(), Protected(0) {}

	u_int32_t MagicNumber;    // Magic Number to identify the file system
	u_int32_t Blocks;        // Number of blocks in the file system
	u_int32_t InodeBlocks;    // Number of Inode blocks in the file system
	u_int32_t Inodes;        // Number of Inodes in the file system
	u_int32_t DirBlocks;    // Number of Directory blocks in the file system

	std::array<char, 257> PasswordHash; // Hash of the password
	u_int32_t Protected;               // 1 if the file system is protected; 0 otherwise
  };

  /**
   * @brief Structure for the Directory Entry
   * @details The Directory Entry contains information about a file or directory
   * @details The Directory Entry is 32 bytes in size
   */
  struct Dirent {
	uint8_t type = 0;        // 0 if the entry is a file; 1 if the entry is a directory
	uint8_t valid = 0;        // 1 if the entry is valid; 0 otherwise
	uint32_t i_num = 0;    // The Inode number of the file or directory

	std::array<char, NAME_SIZE> name{}; // The name of the file or directory
  };

  /**
   * @brief Structure for the Directory
   * @details The Directory contains information about the files and directories in a directory
   * @details The Directory is 4KB in size
   */
  struct Directory {

	/**
	 * @brief Default constructor
	 */
	Directory() : Valid(0), I_num(-1), Name(), TableOfEntries() {}

	/**
	 * @brief Constructor
	 * @param valid 1 if the directory is valid; 0 otherwise
	 * @param i_num the Inode number of the directory
	 * @param name the name of the directory
	 * @param entries the entries in the directory
	 */
	Directory(uint32_t valid,
			  uint32_t i_num,
			  std::array<char, NAME_SIZE> name,
			  std::array<Dirent, ENTRIES_PER_DIR> entries)
		: Valid(valid), I_num(i_num), Name(name), TableOfEntries(entries) {}

	uint16_t Valid;         // 1 if the directory is valid; 0 otherwise
	uint32_t I_num;         // The Inode number of the directory

	std::array<char, NAME_SIZE> Name;     // The name of the directory
	std::array<Dirent, ENTRIES_PER_DIR> TableOfEntries;     // The entries in the directory
  };

  /**
   * @brief Structure for the Inode
   * @details The Inode contains information about a file
   * @details The Inode is 128 bytes in size
   */
  struct Inode {
	uint32_t Valid;      // 1 if the Inode is valid; 0 otherwise
	uint32_t Size;          // The size of the file in bytes
	std::array<uint32_t, FileSystem::POINTERS_PER_INODE> Direct;    // The direct pointers to the data blocks
	uint32_t Indirect;   // The indirect pointer to the data blocks

	/**
	 * @brief Default constructor
	 */
	Inode() : Valid(0), Size(0), Direct(), Indirect(0) {}
  };

  /**
   * @brief Union for the Block
   * @details The Block is the basic unit of storage in the file system
   * @details The Block is 4KB in size
   */
  union Block {

	SuperBlock Super;    // The SuperBlock
	std::array<Inode, FileSystem::INODES_PER_BLOCK> Inodes;        // The Inodes
	std::array<std::uint32_t, FileSystem::POINTERS_PER_BLOCK> Pointers; // The pointers to the data blocks
	std::array<std::byte, Disk::BLOCK_SIZE> Data; // The data in the block
	std::array<Directory, FileSystem::DIR_PER_BLOCK> Directories; // The directories

	/**
	 * @brief Default constructor
	 */
	Block() : Data() {}

  };

  // Internal member variables


  std::shared_ptr<Disk> fs_disk; //  The disk on which the file system is stored

  std::vector<bool> free_blocks_; //  Vector to keep track of free blocks
  std::vector<int> inode_counter_; //  Vector to keep track of the number of inodes in each inode block
  std::vector<uint32_t> dir_counter_; //  Vector to keep track of the number of directories in each directory block
  SuperBlock meta_data_;         //  Caches the SuperBlock to save a disk-read
  bool mounted_ = false;                       //  Boolean to check if the disk is mounted_ and saved

  // Base Layer Core Functions

  /**
   * @brief creates a new inode
   * @return the i_number of the newly created inode
  */
  ssize_t create();

  /**
   * @brief removes the inode
   * @param i_number index into the inode table of the inode to be removed
   * @return true if the remove operation was successful; false otherwise
  */
  bool remove(size_t i_number);

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
   * @param data data buffer
   * @return number of bytes read; -1 if the inode is invalid
  */
 public: // Debugging purposes
  ssize_t read(size_t i_number, std::vector<std::byte> &data, size_t offset);

 private:

  /**
   * @brief writes the data to the inode
   * @param i_number index into the inode table of the inode to be written
   * @param data data buffer
   * @param length Number of bytes to be written
   * @param offset Start writing from index = offset
   * @return number of bytes written; -1 if the inode is invalid
  */
  ssize_t write(size_t i_number, std::vector<std::byte> &data, int length, size_t offset);

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
   * @param block_num index into the free block bitmap
   * @param offset Start reading from index = offset
   * @param length Number of bytes to be read
   * @param data data buffer
   * @param tail_ptr pointer to the tail of the data buffer
   * @return Void function; returns nothing
  */

  void read_helper(uint32_t block_num,
				   int offset,
				   int *length, // TODO: Convert to smart pointer
				   std::vector<std::byte> &data,
				   std::vector<std::byte>::iterator &tail_ptr);

  /**
   * @brief Write the node into the corresponding block and changes the pointers accordingly
   * @param i_number index into the inode table
   * @param node pointer to inode
   * @param ret number of bytes written
   * @return Number of bytes written
  */
  ssize_t write_ret(size_t i_number, Inode &node, int ret);

  /**
   * @brief reads from buffer and writes to a block in the disk
   * @param offset Start reading from index = offset
   * @param read Number of bytes read
   * @param length Number of bytes to be read
   * @param data data buffer
   * @param block_num index into the free block bitmap
   * @return Void function; returns nothing
  */
  void read_buffer(int offset, int *read, int length, std::vector<std::byte> &data, uint32_t block_num);

  /**
   * @brief allocates a block if required; if no block is available in the disk; returns false
   * @param inode pointer to inode
   * @param read Number of bytes read
   * @param orig_offset Start reading from index = offset
   * @param blocknum index into the free block bitmap
   * @param write_indirect boolean value to check if the indirect block is to be written
   * @param indirect pointer to the indirect block
   * @return boolean value indicative of success of the load operation
   */
  bool check_allocation(Inode inode,
						int read,
						int orig_offset,
						uint32_t &blocknum,
						bool write_indirect,
						Block indirect);

  /**
   * @brief Allocates a free block from the free block bitmap
   * @return  index into the free block bitmap
   */
  uint32_t allocate_block();

  Directory curDir; //  Current directory

  /**
   * @brief Adds directory entry to the cached curr_dir. dir_write_back should be done by the caller
   * @param dir directory to which the entry is to be added
   * @param i_num index into the inode table
   * @param type type of the entry
   * @param name name of the entry
   * @return directory with the added entry
   */
  static Directory add_dir_entry(Directory &dir, uint32_t i_num, uint32_t type, const std::string &name);

  /**
   * @brief Writes the directory back to the disk
   * @param dir directory to be written back
   * @return Void function; returns nothing
   */
  void write_dir_back(struct Directory &dir);

  /**
   * @brief Finds a valid entry with the same name. Returns -1 if not found
   * @param dir
   * @param name
   * @return index of the entry if found; -1 otherwise
   */
  static int dir_lookup(Directory &dir, const std::string &name);

  /**
   * @brief Reads Directory from Dirent offset in curr_dir
   * @param offset Dirent offset
   * @return Directory
   */
  Directory read_dir_from_offset(uint32_t offset);

  /**
   * @brief Helper function for removing a directory
   * @param offset Dirent offset
   * @return Directory
   */
  Directory rmdir_helper(Directory parent, std::array<char, NAME_SIZE> name);

  /**
   * @brief Helper function for removing a file
   * @param offset Dirent offset
   * @return Directory
   */
  Directory rm_helper(Directory parent, std::array<char, NAME_SIZE> name);

};

} // namespace fs

#endif //DSFS_DEV_FILE_SYSTEM_HPP
