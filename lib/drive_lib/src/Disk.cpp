//
// Created by Dmitry Morozov on 4/12/22.
//

#include "Disk.hpp"

#include <array>
#include <iostream>
#include <stdexcept>
#include <sstream>

void Disk::open(const std::string &path, size_t n_blocks) {

  if (n_blocks < 2) throw std::runtime_error("Disk size must be at least 2 blocks");

  FileDescriptor_.open(path, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

  if ((!FileDescriptor_.is_open()) or (!FileDescriptor_.good())) {
    std::stringstream ss;
    ss << "Failed to open disk image: " << path << " : " << strerror(errno);
    throw std::runtime_error(ss.str());
  }

  //Reserve space for the disk
  FileDescriptor_.seekp(static_cast<long>(n_blocks * BLOCK_SIZE), std::ios::beg);
  FileDescriptor_.write("", 1);
  FileDescriptor_.seekp(0, std::ios::beg);

  blocks_ = n_blocks;
  reads_ = 0;
  writes_ = 0;

}

Disk::~Disk() {

  if (FileDescriptor_.is_open()) {

    std::cout << "Disk: " << reads_ << " reads, " << writes_ << " writes" << std::endl;

    FileDescriptor_.close();

  }

}

void Disk::ValidCheck(int block_num, const std::array<std::byte, Disk::BLOCK_SIZE> &data) const {

  std::stringstream ss;

  if (block_num < 0) {
    ss << "Invalid block number: " << block_num;
    throw std::invalid_argument(ss.str());
  }

  if (block_num >= blocks_) {
    ss << "Block number " << block_num << " is out of range";
    throw std::invalid_argument(ss.str());
  }

  if (data.size() != BLOCK_SIZE or data.empty()) {
    ss << "Invalid data buffer";
    throw std::invalid_argument(ss.str());
  }

  if (!mounted()) {
    ss << "Disk is not mounted_";
    throw std::invalid_argument(ss.str());
  }

}

void Disk::read(int block_num, std::array<std::byte, Disk::BLOCK_SIZE> &data) {

  ValidCheck(block_num, data);

  if (FileDescriptor_.seekg(block_num * static_cast<int>(BLOCK_SIZE), std::ios::beg).fail()) {

    std::stringstream ss;
    ss << "Unable to seek " << block_num << ": " << strerror(errno);
    throw std::runtime_error(ss.str());

  }

  if (FileDescriptor_.eof()) {
    std::stringstream ss;
    ss << "Unable to read (EOF Position) " << block_num << ": " << strerror(errno);
    throw std::runtime_error(ss.str());
  }

  if (FileDescriptor_.bad()) {
    std::stringstream ss;
    ss << "Unable to read (BAD descriptor) " << block_num << ": " << strerror(errno);
    throw std::runtime_error(ss.str());
  }
//Read the data from the disk
  FileDescriptor_.read(reinterpret_cast<char *>(data.data()), Disk::BLOCK_SIZE);
//FIXME:
  if (FileDescriptor_.fail()) {
    std::stringstream ss;
    ss << "Unable to read " << block_num << ": " << strerror(errno);
    throw std::runtime_error(ss.str());
  }

  ++reads_;

}

void Disk::write(int block_num, const std::array<std::byte, Disk::BLOCK_SIZE> &data) {

  // FIXME: Exception  is thrown here
  ValidCheck(block_num, data);

  if (FileDescriptor_.seekp(block_num * static_cast<int>(BLOCK_SIZE), std::ios::beg).fail()) {

    std::stringstream ss;
    ss << "Unable to seek " << block_num << ": " << strerror(errno);
    throw std::runtime_error(ss.str());

  }

  if (FileDescriptor_.bad()) {
    std::stringstream ss;
    ss << "Unable to write (BAD descriptor) " << block_num << ": " << strerror(errno);
    throw std::runtime_error(ss.str());
  }

  FileDescriptor_.write(reinterpret_cast<const char *>(data.data()), Disk::BLOCK_SIZE);

  if (FileDescriptor_.fail()) {
    std::stringstream ss;
    ss << "Unable to write " << block_num << ": " << strerror(errno);
    throw std::runtime_error(ss.str());
  }
  //Flush the buffer to the disk
  FileDescriptor_.flush();

  ++writes_;

}

void Disk::close() {

  if (FileDescriptor_.is_open()) {

    std::cout << "Disk: " << reads_ << " reads, " << writes_ << " writes" << std::endl;

    reads_ = 0;
    writes_ = 0;

    FileDescriptor_.close();

  }

}
Disk::Disk() : FileDescriptor_(nullptr), blocks_(0), reads_(0), writes_(0), mounts_(0) {}

Disk::Disk(std::string path, std::size_t n_blocks) {

  open(path, n_blocks);

}
