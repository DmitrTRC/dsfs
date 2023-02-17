//
// Created by Dmitry Morozov on 14/2/23.
//
#include "File_System.hpp"
#include "SHA256.h"

#include <cinttypes>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace fs {

bool FileSystem::set_password() {

	if (not mounted_) {
		std::cout << "File system is not mounted" << std::endl;
		return false;
	}

	if (meta_data_.Protected) {

		return change_password();
	}

	SHA256 hash_f;
	std::string pass;

	std::cout << ("Enter new password: ");
	std::getline(std::cin, pass);

	meta_data_.Protected = 1;
	std::strcpy(meta_data_.PasswordHash.data(), hash_f(pass).c_str());

	Block block;

	block.Super = meta_data_;
	fs_disk->write(0, block.Data);
	std::cout << "New password set" << std::endl;

	return true;

}


//bool fs::FileSystem::copyin(const std::string path, const std::string name) {
//
//	if (!mounted_) {
//		std::cout << "File system is not mounted" << std::endl;
//		return false;
//	}
//
//	touch(name);
//
//	int dir_found = dir_lookup(curDir, name);
//
//	if (dir_found==-1) {
//		std::cout << "File not found" << std::endl;
//		return false;
//	}
//
//	uint32_t i_number = curDir.TableOfEntries[offset].i_num;
//
//// open file fo read-only
//	std::stringstream in_stream;
//
//	std::fstream file(path, std::ios::in | std::ios::binary);
//
//	if (!file.is_open()) {
//		std::cout << "File not found" << std::endl;
//		return false;
//	}
//// Output file data
//	std::vector<std::byte> data = {};
//
//// Transform char to std::byte
//	std::transform(in_stream.str().begin(), in_stream.str().end(), std::back_inserter(data), [](char c) {
//	  return std::byte(c);
//	});
//
//	file.close();
//
//// Write data to file
//	int offset{0};
//
//	ssize_t actual_written = write(i_number, data.data(), offset);
//
//	if (actual_written < 0) {
//		std::cerr << "Error writing to file" << std::endl;
//		std::cerr << "Error code: " << actual_written << " bytes actually written" << std::endl;
//		return false;
//	}
//
//	offset += static_cast<int>(actual_written);
//
//	if (offset!=data.size()) {
//		std::cerr << "Error writing to file" << std::endl;
//		std::cerr << "Error code: " << "<SIZE ERROR>" << actual_written << " bytes actually written" << std::endl;
//		return false;
//	}
//
//	std::cout << "File copied successfully" << std::endl;
//	std::cout << "File size: " << actual_written << " bytes" << std::endl;
//
//	return true;
//}
//
//bool fs::FileSystem::touch(const std::string name) {
//
//	if (!mounted_) {
//		std::cout << "File system is not mounted" << std::endl;
//		return false;
//	}
//
//	if (name.size() > NAME_SIZE) {
//		std::cout << "File name is too long" << std::endl;
//		return false;
//	}
//
//	if (dir_lookup(curDir, name)!=-1) {
//		std::cout << "File already exists" << std::endl;
//		return false;
//	}
//
//	int i_number = create();
//
//	if (i_number==-1) {
//		std::cout << "Error creating file" << std::endl;
//		return false;
//	}
//
//	{
//		return false; // TODO: implement
//	}
//	return true;
//
//}
//
//}
//}

}