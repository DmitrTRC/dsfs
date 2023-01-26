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

