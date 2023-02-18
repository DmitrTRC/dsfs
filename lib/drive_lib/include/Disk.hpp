/**
* Created by Dmitry Morozov on 4/12/22.
*
* disk.h: Disk emulator
* @brief Contains the implementation of disk layer to support File System.
*/

#ifndef DSFS_MAIN_DISK_H
#define DSFS_MAIN_DISK_H

#include <cinttypes>
#include <cstddef>
#include <fstream>
#include <string>

/**
 * * @brief Disk class
 * Implements Disk abstraction that enables emulation of a disk image.
 * Used by file system to access and make changes to the disk.
 */
class Disk {

 public:

  /**
   * @brief Block size
   */
  static const std::size_t BLOCK_SIZE = 4096; /// 4K blocks


  /**
   * @brief Default constructor
   */
  Disk();

  /**
   * @brief Constructor
   * @param path Path to disk image
   * @param n_blocks Number of blocks in disk image
   */
  Disk(const std::string &path, std::size_t n_blocks);

  /**
   * @brief Destructor
   */
  ~Disk();

/**
   * @brief Show file permissions
   * @param path Path to disk image
   */
  static void show_file_permissions(const std::filesystem::path &path);

  /** Open disk image
  * @param		path Path to disk image
  * @param		n_blocks Number of blocks in disk image
  * Throws runtime_error exception on error.
  */
  void open(const std::string &path, std::size_t n_blocks);

  /**
   * @brief 	 	Return number of blocks in disk image
   * @return  		Number of blocks in disk image
   */
  [[nodiscard]] std::size_t size() const { return blocks_; }

  /**
   * @brief 	Return number of mounts
   * @return  	Number of mounts
   */
  [[nodiscard]] std::size_t mounts() const { return mounts_; }

  /**
   * @brief 	Return true if disk is mounted
   * @return 	True if disk is mounted
   */
  [[nodiscard]] inline bool mounted() const { return mounts_ > 0; }

  /**
   * @brief 	Increment mounts by 1
   */
  void mount() { ++mounts_; }

  /**
   * @brief 	Decrement mounts by 1
   */
  void unmount() { if (mounts_ > 0) --mounts_; }

  /**
   * @brief 	Read a block from the disk
   * @param block_num	 Block to operate on
   * @param	data 		 Buffer to operate on
   */
  void read(int block_num, std::array<std::byte, Disk::BLOCK_SIZE> &data);

  /**
   * @brief 	 Get number of reads
   * @return	 Number of reads performed
   */
  size_t getReads() const {
	  return reads_;
  }

  /**
   * @brief 	Get number of writes
   * @return 	Number of writes performed
   */
  size_t getWrites() const {
	  return writes_;
  }

  /**
   * @brief 			Write a block to the disk
   * @param block_num 	Block to operate on
   * @param data 		Buffer to operate on
   */
  void write(int block_num, const std::array<std::byte, Disk::BLOCK_SIZE> &data);

  /**
   * @brief Close disk image ( Debugging purposes )
   */
  void close();

  /**
   * @brief 			Check if disk is valid
   * @param block_num 	Block to operate on
   * @param data 		Buffer to operate on
   */
  void isValid(int block_num, const std::array<std::byte, Disk::BLOCK_SIZE> &data) const {
	  ValidCheck(block_num, data);
  }

  /**
   * @brief 			Get block size
   * @return 			Block size
   */
  static size_t getBlockSize() {
	  return BLOCK_SIZE;
  }

 private:
  std::fstream FileDescriptor_;    /// File descriptor for disk image
  std::size_t blocks_{};        /// Number of blocks in disk image
  std::size_t reads_{};        /// Number of reads performed
  std::size_t writes_{};        /// Number of writes performed
  std::size_t mounts_{};        /// Number of mounts

  /** Check parameters
   @param	block_num    Block to operate on
   @param	data	    Buffer to operate on
   Throws invalid_argument exception on error.
   */
  void ValidCheck(int block_num, const std::array<std::byte, Disk::BLOCK_SIZE> &data) const;

};

#endif //DSFS_MAIN_DISK_H
