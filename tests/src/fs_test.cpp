//
// Created by Dmitry Morozov on 27/1/23.
//

#include <gtest/gtest.h>

#include "File_System.hpp"

#include <array>
#include <thread>

TEST(FS, Format_Test) {

  FileSystem fs;

  FileSystem::format(fs.get_cur_disk());

  EXPECT_EQ(fs.get_cur_disk()->mounted(), true);

  EXPECT_EQ(fs.get_cur_disk()->size(), 2);
  EXPECT_EQ(fs.get_cur_disk()->mounted(), true);
  EXPECT_EQ(fs.mounted(), true);

  //Check each block

//  std::array<std::byte, Disk::BLOCK_SIZE> data = {};
//
//  EXPECT_NO_THROW(fs.get_cur_disk()->read(0, data));
//
//  EXPECT_NO_THROW(fs.get_cur_disk()->read(1, data));
//
//  EXPECT_THROW(fs.get_cur_disk()->read(2, data), std::invalid_argument);
//
//  EXPECT_THROW(fs.get_cur_disk()->read(3, data), std::invalid_argument);
//
//  //Check each block for validity
//
//  EXPECT_NO_THROW(fs.get_cur_disk()->isValid(0, data));
//
//  EXPECT_NO_THROW(fs.get_cur_disk()->isValid(1, data));
//
//  EXPECT_THROW(fs.get_cur_disk()->isValid(2, data), std::invalid_argument);
//
//  EXPECT_THROW(fs.get_cur_disk()->isValid(3, data), std::invalid_argument);

}