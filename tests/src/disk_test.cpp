//
// Created by Dmitry Morozov on 27/1/23.
//

#include <gtest/gtest.h>

#include "Disk.hpp"

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

  std::shared_ptr<char> data = std::make_shared<char>(disk.BLOCK_SIZE);

  EXPECT_THROW(disk.isValid(-1, data), std::invalid_argument);
  EXPECT_THROW(disk.isValid(2, data), std::invalid_argument);
  EXPECT_THROW(disk.isValid(0, nullptr), std::invalid_argument);
  EXPECT_THROW(disk.isValid(3, data), std::invalid_argument);

  EXPECT_NO_THROW(disk.isValid(0, data));
  EXPECT_NO_THROW(disk.isValid(1, data));

}

TEST(DISK, Read_Test) {

  Disk disk;

  disk.open("test.dat", 2);
  disk.mount();

  std::shared_ptr<char> data = std::make_shared<char>(disk.BLOCK_SIZE);

  EXPECT_THROW(disk.read(-1, data), std::invalid_argument);
  EXPECT_THROW(disk.read(2, data), std::invalid_argument);
  EXPECT_THROW(disk.read(0, nullptr), std::invalid_argument);

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

