//
// Created by Dmitry Morozov on 27/1/23.
//

#include <gtest/gtest.h>

#include "Disk.hpp"

#include <array>
#include <thread>

TEST(DISK, Constructor_Test) {

  Disk disk;

  EXPECT_EQ(disk.size(), 0);
  EXPECT_EQ(disk.mounted(), false);
  EXPECT_NO_THROW(disk.mount());
  EXPECT_EQ(disk.mounted(), true);

}

TEST(DISK, Open_Test) {

  Disk disk;

  EXPECT_THROW(disk.open("test", 0), std::runtime_error);
  EXPECT_THROW(disk.open("test", 1), std::runtime_error);
  EXPECT_NO_THROW(disk.open("test", 2));
  EXPECT_EQ(disk.size(), 2);
  disk.mount();
  EXPECT_EQ(disk.mounted(), true);

}

TEST(DISK, ValidCheck_Test) {

  Disk disk;

  disk.open("test.dat", 2);
  disk.mount();

  std::array<std::byte, Disk::BLOCK_SIZE> data = {};

  EXPECT_THROW(disk.isValid(-1, data), std::invalid_argument);
  EXPECT_THROW(disk.isValid(2, data), std::invalid_argument);
  EXPECT_THROW(disk.isValid(3, data), std::invalid_argument);

  EXPECT_NO_THROW(disk.isValid(0, data));
  EXPECT_NO_THROW(disk.isValid(1, data));

}

TEST(DISK, Read_Test) {

  Disk disk;

  disk.open("test.dat", 2);
  disk.mount();

  std::array<std::byte, Disk::BLOCK_SIZE> data = {};

  EXPECT_THROW(disk.read(-1, data), std::invalid_argument);
  EXPECT_THROW(disk.read(2, data), std::invalid_argument);

  EXPECT_NO_THROW(disk.read(0, data));
  EXPECT_EQ(disk.getReads(), 1);
  EXPECT_EQ(disk.getWrites(), 0);

  EXPECT_NO_THROW(disk.read(1, data));
  EXPECT_EQ(disk.getReads(), 2);
  EXPECT_EQ(disk.getWrites(), 0);

  EXPECT_NO_THROW(disk.read(0, data));
  EXPECT_EQ(disk.getReads(), 3);
  EXPECT_EQ(disk.getWrites(), 0);

  EXPECT_NO_THROW(disk.read(1, data));
  EXPECT_EQ(disk.getReads(), 4);
  EXPECT_EQ(disk.getWrites(), 0);

  EXPECT_NO_THROW(disk.read(0, data));
  EXPECT_EQ(disk.getReads(), 5);
  EXPECT_EQ(disk.getWrites(), 0);

}

TEST(DISK, Write_Test) {

  Disk disk;

  disk.open("test.dat", 2);
  disk.mount();

  std::array<std::byte, Disk::BLOCK_SIZE> data = {};

  EXPECT_THROW(disk.write(-1, data), std::invalid_argument);
  EXPECT_THROW(disk.write(2, data), std::invalid_argument);

  EXPECT_NO_THROW(disk.write(0, data));
  EXPECT_EQ(disk.getReads(), 0);
  EXPECT_EQ(disk.getWrites(), 1);

  EXPECT_NO_THROW(disk.write(1, data));
  EXPECT_EQ(disk.getReads(), 0);
  EXPECT_EQ(disk.getWrites(), 2);

  EXPECT_NO_THROW(disk.write(0, data));
  EXPECT_EQ(disk.getReads(), 0);
  EXPECT_EQ(disk.getWrites(), 3);

  EXPECT_NO_THROW(disk.write(1, data));
  EXPECT_EQ(disk.getReads(), 0);
  EXPECT_EQ(disk.getWrites(), 4);

  EXPECT_NO_THROW(disk.write(0, data));
  EXPECT_EQ(disk.getReads(), 0);
  EXPECT_EQ(disk.getWrites(), 5);

}

TEST(DISK, ReadWriteData_Test) {

  Disk disk;

  disk.open("test.dat", 2);
  disk.mount();

  std::array<std::byte, Disk::BLOCK_SIZE> Z_data = {};
  std::fill(Z_data.begin(), Z_data.end(), std::byte('Z'));

  std::array<std::byte, Disk::BLOCK_SIZE> X_data = {};
  std::fill(X_data.begin(), X_data.end(), std::byte('X'));

  std::array<std::byte, Disk::BLOCK_SIZE> data = {};

  EXPECT_NO_THROW(disk.write(0, Z_data));
  EXPECT_NO_THROW(disk.write(1, X_data));
  EXPECT_NO_THROW(disk.read(0, data));
  EXPECT_EQ(disk.getReads(), 1);
  EXPECT_EQ(disk.getWrites(), 2);
  EXPECT_EQ(std::memcmp(data.data(), Z_data.data(), disk.BLOCK_SIZE), 0);

  EXPECT_NO_THROW(disk.read(1, data));
  EXPECT_EQ(disk.getReads(), 2);
  EXPECT_EQ(disk.getWrites(), 2);
  EXPECT_EQ(std::memcmp(data.data(), X_data.data(), disk.BLOCK_SIZE), 0);
  EXPECT_NO_THROW(disk.write(0, X_data));
  EXPECT_EQ(disk.getReads(), 2);
  EXPECT_EQ(disk.getWrites(), 3);
  EXPECT_NO_THROW(disk.read(0, data));
  EXPECT_EQ(disk.getReads(), 3);
  EXPECT_EQ(disk.getWrites(), 3);
  EXPECT_EQ(std::memcmp(data.data(), X_data.data(), disk.BLOCK_SIZE), 0);

}

TEST(DISK, test_close) {

  Disk disk;

  disk.open("test.dat", 2);
  disk.mount();

  std::array<std::byte, Disk::BLOCK_SIZE> data = {};

  EXPECT_NO_THROW(disk.write(0, data));
  EXPECT_NO_THROW(disk.write(1, data));
  EXPECT_NO_THROW(disk.read(0, data));
  EXPECT_NO_THROW(disk.read(1, data));
  EXPECT_NO_THROW(disk.write(0, data));
  EXPECT_NO_THROW(disk.write(1, data));
  EXPECT_NO_THROW(disk.read(0, data));
  EXPECT_NO_THROW(disk.read(1, data));

  EXPECT_EQ(disk.getReads(), 4);
  EXPECT_EQ(disk.getWrites(), 4);

  EXPECT_NO_THROW(disk.close());

  EXPECT_EQ(disk.getReads(), 0);
  EXPECT_EQ(disk.getWrites(), 0);

}

TEST(DISK, test_mount) {

  Disk disk;

  disk.open("test.dat", 2);

  EXPECT_NO_THROW(disk.mount());
  EXPECT_NO_THROW(disk.mount());

  EXPECT_NO_THROW(disk.close());

  EXPECT_NO_THROW(disk.mount());

}

TEST(DISK, test_unmount) {

  Disk disk;

  disk.open("test.dat", 2);

  EXPECT_NO_THROW(disk.mount());
  EXPECT_NO_THROW(disk.unmount());
  EXPECT_NO_THROW(disk.unmount());

  EXPECT_NO_THROW(disk.close());

  EXPECT_NO_THROW(disk.unmount());

}

TEST(DISK, test_mounted) {

  Disk disk;

  disk.open("test.dat", 2);

  EXPECT_FALSE(disk.mounted());

  EXPECT_NO_THROW(disk.mount());

  EXPECT_TRUE(disk.mounted());

  EXPECT_NO_THROW(disk.unmount());

  EXPECT_FALSE(disk.mounted());

  EXPECT_NO_THROW(disk.close());

  EXPECT_FALSE(disk.mounted());

}

TEST(DISK, test_get_Block_Size) {

  Disk disk;

  disk.open("test.dat", 2);

  EXPECT_EQ(disk.getBlockSize(), 4096);

  EXPECT_NO_THROW(disk.close());

}

TEST(DISK, test_get_Number_Of_Blocks) {

  Disk disk;

  disk.open("test.dat", 2);

  EXPECT_EQ(disk.size(), 2);

  EXPECT_NO_THROW(disk.close());

}

