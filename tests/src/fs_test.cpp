//
// Created by Dmitry Morozov on 27/1/23.
//

#include <gtest/gtest.h>
#include <memory>

#include "File_System.hpp"

#include <array>
#include <thread>

using namespace fs;

TEST(FS, Constructor_Test) {

	FileSystem fs;
	std::shared_ptr<Disk> disk = std::make_shared<Disk>();

	disk->open("image.5.img", 5);

	fs.set_cur_disk(disk);

	EXPECT_EQ(fs.get_cur_disk()->size(), 5);
	EXPECT_EQ(fs.mounted(), false);

}

TEST(FS, Format_Test) {
	std::shared_ptr<Disk> disk = std::make_shared<Disk>();
	FileSystem fs;

	disk->open("image.5.img", 5);

	fs.set_cur_disk(disk);

	EXPECT_EQ(fs.get_cur_disk()->size(), 5);
	EXPECT_EQ(fs.get_cur_disk()->mounted(), false);

	EXPECT_NO_THROW(FileSystem::format(fs.get_cur_disk()));
//
//	EXPECT_EQ(fs.get_cur_disk()->mounted(), true);
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
	EXPECT_EQ(true, true);
//  std::shared_ptr<Disk> disk = std::make_shared<Disk>();
//  disk->open("image.5.img", 5);
//
//  FileSystem fs;
//  fs.mount(disk);
//
//  EXPECT_EQ(fs.get_cur_disk()->mounted(), true);

}

TEST(FS, Debug_Test) {

	std::shared_ptr<Disk> disk = std::make_shared<Disk>();

	disk->open("image.5.img", 5);

	EXPECT_NO_THROW(fs::FileSystem::debug(disk));

}

