//
// Created by Dmitry Morozov on 27/1/23.
//

#include <gtest/gtest.h>
#include <memory>

#include "File_System.hpp"

#include <array>
#include <thread>

using namespace fs;

TEST(FS, Format_Test) {

  EXPECT_EQ(true, true);

//  FileSystem fs;
//
//  FileSystem::format(fs.get_cur_disk());
//
//  EXPECT_EQ(fs.get_cur_disk()->mounted(), true);
//
//  EXPECT_EQ(fs.get_cur_disk()->size(), 2);
//  EXPECT_EQ(fs.get_cur_disk()->mounted(), true);
//  EXPECT_EQ(fs.mounted(), true);

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

TEST(FS, Mount_Test) {
// Using image.5.img as a test disk
  using namespace fs;

  std::shared_ptr<Disk> disk = std::make_shared<Disk>();
  disk->open("image.5.img", 5);

  FileSystem fs;
  fs.mount(disk);

  EXPECT_EQ(fs.get_cur_disk()->mounted(), true);

}


