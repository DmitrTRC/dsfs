//
// Created by Dmitry Morozov on 27/1/23.
//

#include <gtest/gtest.h>

#include "Disk.hpp"
#include <iostream>
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

  std::shared_ptr<char> data(new char[disk.BLOCK_SIZE], std::default_delete<char[]>());

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

  std::shared_ptr<char> data(new char[disk.BLOCK_SIZE], std::default_delete<char[]>());

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

TEST(DISK, Write_Test) {

  Disk disk;

  disk.open("test.dat", 2);
  disk.mount();

  std::shared_ptr<char> data(new char[disk.BLOCK_SIZE], std::default_delete<char[]>());

  EXPECT_THROW(disk.write(-1, data), std::invalid_argument);
  EXPECT_THROW(disk.write(2, data), std::invalid_argument);
  EXPECT_THROW(disk.write(0, nullptr), std::invalid_argument);

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

  std::shared_ptr<char> Z_data(new char[disk.BLOCK_SIZE], std::default_delete<char[]>());
  std::fill(Z_data.get(), Z_data.get() + disk.BLOCK_SIZE, 'Z');

  std::shared_ptr<char> X_data(new char[disk.BLOCK_SIZE], std::default_delete<char[]>());
  std::fill(X_data.get(), X_data.get() + disk.BLOCK_SIZE, 'X');

  std::shared_ptr<char> data(new char[disk.BLOCK_SIZE], std::default_delete<char[]>());

//Delays the execution of the current thread for the specified duration.
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  EXPECT_NO_THROW(disk.write(0, Z_data));
  EXPECT_NO_THROW(disk.write(1, X_data));
  EXPECT_NO_THROW(disk.read(0, data));
  EXPECT_EQ(disk.getReads(), 1);
  EXPECT_EQ(disk.getWrites(), 2);
  EXPECT_EQ(std::memcmp(data.get(), Z_data.get(), disk.BLOCK_SIZE), 0);

  EXPECT_NO_THROW(disk.read(1, data));
  EXPECT_EQ(disk.getReads(), 2);
  EXPECT_EQ(disk.getWrites(), 2);
  EXPECT_EQ(std::memcmp(data.get(), X_data.get(), disk.BLOCK_SIZE), 0);
  EXPECT_NO_THROW(disk.write(0, X_data));
  EXPECT_EQ(disk.getReads(), 2);
  EXPECT_EQ(disk.getWrites(), 3);
  EXPECT_NO_THROW(disk.read(0, data));
  EXPECT_EQ(disk.getReads(), 3);
  EXPECT_EQ(disk.getWrites(), 3);
  EXPECT_EQ(std::memcmp(data.get(), X_data.get(), disk.BLOCK_SIZE), 0);

}

