//
// Created by Dmitry Morozov on 4/12/22.
//

#include "Disk.hpp"

#include <iostream>
#include <stdexcept>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>

void Disk::open(const std::string &path, size_t n_blocks) {

  file_descriptor_ = ::open(path.c_str(), O_RDWR | O_CREAT, 0600);

  if (file_descriptor_ < 0) {

    std::stringstream ss;
    ss << "Failed to open disk image: " << path << " : " << strerror(errno);
    throw std::runtime_error(ss.str());

  }

  if (ftruncate(file_descriptor_, static_cast<int>(n_blocks * BLOCK_SIZE)) < 0) {

    std::stringstream ss;
    ss << "Failed to truncate disk image: " << path << " : " << strerror(errno);
    throw std::runtime_error(ss.str());

  }

  blocks_ = n_blocks;
  reads_ = 0;
  writes_ = 0;

}

Disk::~Disk() {

  if (file_descriptor_ > 0) {

    std::cout << "disk  : " << reads_ << "block reads" << std::endl;
    std::cout << "disk  : " << writes_ << "block writes" << std::endl;
    ::close(file_descriptor_);
    file_descriptor_ = 0;

  }

}

void Disk::HealthCheck(int block_num, const char *data) const {

  std::stringstream ss;

  if (block_num < 0) {

    ss << "Invalid block number ( negative ) : " << block_num;
    throw std::invalid_argument(ss.str());

  }

  if (block_num >= static_cast<int>(blocks_)) {

    ss << "Invalid block number ( too large ) : " << block_num;
    throw std::invalid_argument(ss.str());

  }

  if (data == nullptr) {

    ss << "Invalid data buffer ( null )";
    throw std::invalid_argument(ss.str());

  }

}

void Disk::read(int block_num, char *data) {

  HealthCheck(block_num, data);

  if (lseek(file_descriptor_, block_num * static_cast<int>(BLOCK_SIZE), SEEK_SET) < 0) {

    std::stringstream ss;
    ss << "Unable to seek to block " << block_num << ": " << strerror(errno);
    throw std::runtime_error(ss.str());

  }

  if (::read(file_descriptor_, data, BLOCK_SIZE) != BLOCK_SIZE) {

    std::stringstream ss;
    ss << "Unable to read block " << block_num << ": " << strerror(errno);
    throw std::runtime_error(ss.str());

  }

  ++reads_;

}

void Disk::write(int block_num, char *data) {

  HealthCheck(block_num, data);

  if (lseek(file_descriptor_, block_num * static_cast<int>(BLOCK_SIZE), SEEK_SET) < 0) {

    std::stringstream ss;
    ss << "Unable to lseek " << block_num << ": " << strerror(errno);
    throw std::runtime_error(ss.str());

  }

  if (::write(file_descriptor_, data, BLOCK_SIZE) != BLOCK_SIZE) {

    std::stringstream result_error;
    result_error << "Unable to write " << block_num << ": " << strerror(errno);
    throw std::runtime_error(result_error.str());

  }

  ++writes_;

}

uint32_t Disk::blocks() const {

  return blocks_;

}
