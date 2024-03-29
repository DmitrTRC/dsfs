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

TEST(FS, Format5k_Test) {
	std::shared_ptr<Disk> disk = std::make_shared<Disk>();
	FileSystem fs;

	disk->open("image.5.img", 5);

	fs.set_cur_disk(disk);

	EXPECT_EQ(fs.get_cur_disk()->size(), 5);
	EXPECT_EQ(fs.get_cur_disk()->mounted(), false);

	EXPECT_NO_THROW(FileSystem::format(fs.get_cur_disk()));


	//Check each block

	std::array<std::byte, Disk::BLOCK_SIZE> data = {};

}

TEST(FS, Format20k_Test) {
	std::shared_ptr<Disk> disk = std::make_shared<Disk>();
	FileSystem fs;

	disk->open("image.5.img", 20);

	fs.set_cur_disk(disk);

	EXPECT_EQ(fs.get_cur_disk()->size(), 20);
	EXPECT_EQ(fs.get_cur_disk()->mounted(), false);

	EXPECT_NO_THROW(FileSystem::format(fs.get_cur_disk()));


	//Check each block

	std::array<std::byte, Disk::BLOCK_SIZE> data = {};

}

TEST(FS, Mount5_Test) {
// Using image.5.img as a test disk
	EXPECT_EQ(true, true);
	std::shared_ptr<Disk> disk = std::make_shared<Disk>();
	disk->open("image.5.img", 5);

	FileSystem::debug(disk);

	FileSystem fs;

	fs.set_cur_disk(disk);
	fs.mount(disk);

	EXPECT_EQ(fs.get_cur_disk()->mounted(), true);

}

TEST(FS, Mount20_Test) {
// Using image.20.img as a test disk
	EXPECT_EQ(true, true);
	std::shared_ptr<Disk> disk = std::make_shared<Disk>();
	disk->open("image.20.img", 20);

	FileSystem::debug(disk);

	FileSystem fs;

	fs.set_cur_disk(disk);
	fs.mount(disk);

	EXPECT_EQ(fs.get_cur_disk()->mounted(), true);

}

TEST(FS, Mount200_Test) {
// Using image.200.img as a test disk
	EXPECT_EQ(true, true);
	std::shared_ptr<Disk> disk = std::make_shared<Disk>();
	disk->open("image.200.img", 200);

	FileSystem::debug(disk);

	FileSystem fs;

	fs.set_cur_disk(disk);
	fs.mount(disk);

	EXPECT_EQ(fs.get_cur_disk()->mounted(), true);

}

TEST(FS, Debug_Test) {

	std::shared_ptr<Disk> disk = std::make_shared<Disk>();

	disk->open("image.5.img", 5);

	EXPECT_NO_THROW(fs::FileSystem::debug(disk));

}

TEST(FS, Read_Test) {
	std::shared_ptr<Disk> disk = std::make_shared<Disk>();
	disk->open("image.5.img", 5);

	FileSystem::debug(disk);

	FileSystem fs;

	fs.set_cur_disk(disk);
	fs.mount(disk);

	EXPECT_EQ(fs.get_cur_disk()->mounted(), true);

	// Test reading from a file , read first inode

	std::vector<std::byte> data;

//	fs.read(1, data, 4096);
//
//	EXPECT_EQ(data.size(), 4096);

}

//	LS test
TEST(FS, LS_Test) {
	std::shared_ptr<Disk> disk = std::make_shared<Disk>();
	disk->open("image.5.img", 5);

	FileSystem::debug(disk);

	FileSystem fs;

	fs.set_cur_disk(disk);
	fs.mount(disk);

	EXPECT_EQ(fs.get_cur_disk()->mounted(), true);
	EXPECT_EQ(fs.get_cur_disk()->size(), 20);

	// Test reading from a file , read first inode

	std::vector<std::byte> data;

	std::cout << "LS test" << std::endl;

	fs.ls();

	std::cout << "LS test end" << std::endl;

}


