//
// Created by Dmitry Morozov on 14/2/23.
//
#include "File_System.hpp"

#include <cinttypes>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

bool fs::FileSystem::copyin(const std::string path, const std::string name) {

	if (!mounted_) {
		std::cout << "File system is not mounted" << std::endl;
		return false;
	}

	touch(name);

	int dir_found = dir_lookup(curDir, name);

	if (dir_found==-1) {
		std::cout << "File not found" << std::endl;
		return false;
	}

	uint32_t i_number = curDir.TableOfEntries[offset].i_num;

// open file fo read-only
	std::stringstream in_stream;

	std::fstream file(path, std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		std::cout << "File not found" << std::endl;
		return false;
	}
// Output file data
	std::vector<std::byte> data = {};

// Transform char to std::byte
	std::transform(in_stream.str().begin(), in_stream.str().end(), std::back_inserter(data), [](char c) {
	  return std::byte(c);
	});

	file.close();

// Write data to file
	int offset{0};

	ssize_t actual_written = write(i_number, data.data(), offset);

	if (actual_written < 0) {
		std::cerr << "Error writing to file" << std::endl;
		std::cerr << "Error code: " << actual_written << " bytes actually written" << std::endl;
		return false;
	}

	offset += static_cast<int>(actual_written);

	if (offset!=data.size()) {
		std::cerr << "Error writing to file" << std::endl;
		std::cerr << "Error code: " << "<SIZE ERROR>" << actual_written << " bytes actually written" << std::endl;
		return false;
	}

	std::cout << "File copied successfully" << std::endl;
	std::cout << "File size: " << actual_written << " bytes" << std::endl;

	return true;
}