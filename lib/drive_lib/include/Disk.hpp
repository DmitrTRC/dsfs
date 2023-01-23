//
// Created by Dmitry Morozov on 4/12/22.
//
// disk.h: Disk emulator

#ifndef DSFS_MAIN_DISK_H
#define DSFS_MAIN_DISK_H

#include <cinttypes>
#include <cstddef>

#include <string>

class Disk {
 private:
  int file_descriptor_; /// File descriptor of disk image
  std::size_t blocks_;        /// Number of blocks in disk image
  std::size_t reads_;        /// Number of reads performed
  std::size_t writes_;        /// Number of writes performed
  std::size_t mounts_;        /// Number of mounts

  /** Check parameters
   @param	block_num    Block to operate on
   @param	data	    Buffer to operate on
   Throws invalid_argument exception on error.
   */
  void HealthCheck(int block_num, const char *data) const;

 public:
  /// Number of bytes per block
  const static std::size_t Block_Size = 4096; /// 4K blocks

  /// Default constructor
  Disk() : file_descriptor_(0), blocks_(0), reads_(0), writes_(0), mounts_(0) {}

  /// Destructor
  ~Disk();

  /** Open disk image
  * @param	path	    Path to disk image
  * @param	n_blocks	    Number of blocks in disk image
  * Throws runtime_error exception on error.
  */
  void open(const std::string &path, std::size_t n_blocks);

  /// Return size of disk (in terms of blocks)
  [[nodiscard]] inline std::size_t size() const { return blocks_; }

  /// Return whether or not disk is mounted
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
  void read(int block_num, char *data);

  /**
   * @brief
   * @param block_num
   * @param data
   */
  void write(int block_num, char *data);

  [[nodiscard]] std::uint32_t blocks() const;
};

#endif //DSFS_MAIN_DISK_H
