//
// Created by Dmitry Morozov on 4/12/22.
//
// disk.h: Disk emulator

#ifndef DSFS_MAIN_DISK_H
#define DSFS_MAIN_DISK_H

#include <cinttypes>
#include <cstddef>

#include <fstream>
#include <string>

class Disk {

 public:
  /// Number of bytes per block
  static const std::size_t BLOCK_SIZE = 4096; /// 4K blocks

  /// Default constructor
  Disk();

  Disk(std::string path, std::size_t n_blocks);

  ~Disk();

  /** Open disk image
  * @param	path	    Path to disk image
  * @param	n_blocks	    Number of blocks in disk image
  * Throws runtime_error exception on error.
  */
  void open(const std::string &path, std::size_t n_blocks);

  /// Return size of disk (in terms of blocks)
  [[nodiscard]] inline std::size_t size() const { return blocks_; }

  /// Return whether disk is mounted_
  [[nodiscard]] inline bool mounted() const { return mounts_ > 0; }

  /// Increment mounts
  void mount() { ++mounts_; }

  /// Decrement mounts
  void unmount() { if (mounts_ > 0) --mounts_; }

  /**
   * @brief Read a block from the disk
   * @param block_num
   * @param data
   */
  void read(int block_num, std::array<std::byte, Disk::BLOCK_SIZE> &data);

  size_t getReads() const {
    return reads_;
  }

  size_t getWrites() const {
    return writes_;
  }

  /**
   * @brief
   * @param block_num
   * @param data
   */
  void write(int block_num, const std::array<std::byte, Disk::BLOCK_SIZE> &data);

  //Only for testing
  void close();

  void isValid(int block_num, const std::array<std::byte, Disk::BLOCK_SIZE> &data) const {
    ValidCheck(block_num, data);
  }

  static size_t getBlockSize() {
    return BLOCK_SIZE;
  }

 private:
  std::fstream FileDescriptor_;    /// File descriptor for disk image
  std::size_t blocks_;        /// Number of blocks in disk image
  std::size_t reads_;        /// Number of reads performed
  std::size_t writes_;        /// Number of writes performed
  std::size_t mounts_;        /// Number of mounts

  /** Check parameters
   @param	block_num    Block to operate on
   @param	data	    Buffer to operate on
   Throws invalid_argument exception on error.
   */
  void ValidCheck(int block_num, const std::array<std::byte, Disk::BLOCK_SIZE> &data) const;

};

#endif //DSFS_MAIN_DISK_H
