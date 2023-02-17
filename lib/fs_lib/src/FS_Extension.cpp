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

bool FileSystem::change_password() {

	if (not mounted_) {
		std::cout << "File system is not mounted" << std::endl;
		return false;
	}

	if (meta_data_.Protected) {

		SHA256 hash_f;
		std::string pass;

		std::cout << ("Enter current password: ");
		std::getline(std::cin, pass);

		if (hash_f(pass)!=std::string(meta_data_.PasswordHash.data())) {
			std::cout << "Old password incorrect." << std::endl;
			return false;
		}

		meta_data_.Protected = 0;

	}

	return FileSystem::set_password();

}

bool FileSystem::remove_password() {

	if (not mounted_) {
		std::cout << "File system is not mounted" << std::endl;
		return false;
	}

	if (meta_data_.Protected) {

		SHA256 hash_f;
		std::string pass;

		std::cout << ("Enter current password: ");
		std::getline(std::cin, pass);

		if (hash_f(pass)!=std::string(meta_data_.PasswordHash.data())) {
			std::cout << "Old password incorrect." << std::endl;
			return false;
		}

		meta_data_.Protected = 0;

		Block block;

		block.Super = meta_data_;
		fs_disk->write(0, block.Data);
		std::cout << "Password removed successfully." << std::endl;

		return true;
	}

	return false;
}

FileSystem::Directory FileSystem::add_dir_entry(FileSystem::Directory &dir,
												uint32_t i_num,
												uint32_t type,
												std::string name) {
	Directory temp_dir = dir;

	uint32_t idx = 0;

	for (auto &entry : temp_dir.TableOfEntries) {
		if (entry.valid==0) {
			break;
		}
		idx++;
	}

	if (idx==FileSystem::ENTRIES_PER_DIR) {
		std::cout << "Directory entry limit reached. Exiting..." << std::endl;
		temp_dir.Valid = 0;
		return temp_dir;
	}

	temp_dir.TableOfEntries[idx].i_num = i_num;
	temp_dir.TableOfEntries[idx].type = type;
	temp_dir.TableOfEntries[idx].valid = 1;

	std::strcpy(temp_dir.TableOfEntries[idx].name.data(), name.c_str());

	return temp_dir;

}

FileSystem::Directory FileSystem::read_dir_from_offset(uint32_t offset) {

	if (offset >= FileSystem::ENTRIES_PER_DIR || curDir.TableOfEntries[offset].valid==0
		|| curDir.TableOfEntries[offset].type!=0) {

		Directory temp_dir;
		temp_dir.Valid = 0;
		return temp_dir;
	}

	uint32_t i_num = curDir.TableOfEntries[offset].i_num;
	uint32_t block_idx = (i_num/FileSystem::DIR_PER_BLOCK);
	uint32_t block_offset = (i_num%FileSystem::DIR_PER_BLOCK);

	Block block;

	fs_disk->read(static_cast<int>(meta_data_.Blocks - 1 - block_idx), block.Data);

	return (block.Directories[block_offset]);
}

void FileSystem::write_dir_back(Directory &dir) {

	uint32_t block_idx = (dir.I_num/FileSystem::DIR_PER_BLOCK);
	uint32_t block_offset = (dir.I_num%FileSystem::DIR_PER_BLOCK);

	Block block;

	fs_disk->read(static_cast<int>(meta_data_.Blocks - 1 - block_idx), block.Data);
	block.Directories[block_offset] = dir;

	fs_disk->write(static_cast<int>(meta_data_.Blocks - 1 - block_idx), block.Data);
}

int FileSystem::dir_lookup(Directory &dir, const std::string &name) {

	uint32_t offset = 0;

	for (auto &entry : dir.TableOfEntries) {
		if ((entry.valid==1) && (std::string(entry.name.data())==name)) {
			break;
		}
		offset++;

	}

	if (offset==FileSystem::ENTRIES_PER_DIR) {
		return -1;
	}

	return static_cast<int>(offset);

}

bool FileSystem::ls_dir(const std::string &name) {

	if (not mounted_) {
		std::cout << "File system is not mounted" << std::endl;
		return false;
	}

	if (name.empty()) {
		std::cout << "No directory name provided" << std::endl;
		return false;
	}

	int offset = dir_lookup(curDir, name);

	if (offset==-1) {
		std::cout << "No such directory" << std::endl;
		return false;
	}

	Directory dir = read_dir_from_offset(static_cast<uint32_t>(offset));

	if (dir.Valid==0) {
		std::cout << "Directory invalid" << std::endl;
		return false;
	}

	std::cout << "   inum    |       name       | type" << std::endl;

	for (auto &entry : dir.TableOfEntries) {
		if (entry.valid==1) {
			if (entry.type==1) {
				std::cout << std::setw(10) << entry.i_num << " | " << std::setw(16) << entry.name.data() << " | "
						  << std::setw(5) << "file" << std::endl;
			} else {
				std::cout << std::setw(10) << entry.i_num << " | " << std::setw(16) << entry.name.data() << " | "
						  << std::setw(5) << "dir" << std::endl;
			}
		}
	}

	return true;
}

bool FileSystem::mkdir(const std::string &name) {

	if (not mounted_) {
		std::cerr << "File system is not mounted" << std::endl;
		return false;
	}

	if (name.empty() || name=="." || name==".." || name=="/" || name.length() > FileSystem::NAME_SIZE) { //TODO: Move
		// this to a function or const array of constraints
		std::cerr << "No valid directory name provided" << std::endl;
		return false;
	}

	uint32_t block_idx = 0;

	for (auto &dir : dir_counter_) {
		if (dir < FileSystem::DIR_PER_BLOCK) {
			break;
		}
		block_idx++;
	}

	if (block_idx==meta_data_.DirBlocks) {
		std::cerr << "Directory limit reached" << std::endl;
		return false;
	}

	Block block;

	fs_disk->read(static_cast<int>(meta_data_.Blocks - 1 - block_idx), block.Data);

	uint32_t offset = 0;

	for (auto &dir : block.Directories) {
		if (dir.Valid==0) {
			break;
		}
		offset++;
	}

	if (offset==FileSystem::DIR_PER_BLOCK) {
		std::cerr << "Directory limit reached" << std::endl;

		return false;
	}

	Directory new_dir, temp_dir;

	new_dir.Valid = 1;

	new_dir.I_num = block_idx*FileSystem::DIR_PER_BLOCK + offset;

	std::copy(name.begin(), name.end(), new_dir.Name.begin());

	new_dir.Name[name.length()] = '\0';
	std::string cur_str = ".", up_str = "..";
	temp_dir = new_dir;

	temp_dir = add_dir_entry(temp_dir, temp_dir.I_num, 0, cur_str);
	temp_dir = add_dir_entry(temp_dir, curDir.I_num, 0, up_str);

	if (temp_dir.Valid==0) {
		std::cerr << "Error creating new directory" << std::endl;
		return false;
	}

	new_dir = temp_dir;
	temp_dir = add_dir_entry(curDir, new_dir.I_num, 0, new_dir.Name.data());

	if (temp_dir.Valid==0) {
		std::cerr << "Error adding new directory" << std::endl;
		return false;
	}

	curDir = temp_dir;

	write_dir_back(new_dir);
	write_dir_back(curDir);

	dir_counter_[block_idx]++;

	return true;

}
FileSystem::Directory FileSystem::rmdir_helper(Directory parent, std::string &name) {

	Directory dir, temp;
	Block block;

	uint32_t i_num;
	uint32_t block_idx;
	uint32_t block_off;

	if (not mounted_) {
		std::cerr << "File system is not mounted" << std::endl;
		dir.Valid = 0;
		return dir;
	}

	if (name.empty() || name=="." || name==".." || name=="/" || name.length() > FileSystem::NAME_SIZE) { //TODO: Move
		// this to a function or const array of constraints
		std::cerr << "No valid directory name provided" << std::endl;
		dir.Valid = 0;
		return dir;
	}

	int offset = dir_lookup(parent, name);

	if (offset==-1) {
		std::cerr << "No such directory" << std::endl;
		dir.Valid = 0;
		return dir;
	}

	i_num = parent.TableOfEntries[offset].i_num;

	block_idx = i_num/FileSystem::DIR_PER_BLOCK;
	block_off = i_num%FileSystem::DIR_PER_BLOCK;

	fs_disk->read(static_cast<int>(meta_data_.Blocks - 1 - block_idx), block.Data);

	dir = block.Directories[block_off];

	if (dir.Valid==0) {
		std::cerr << "Directory invalid" << std::endl;
		return dir;
	}

	if (dir.Name==curDir.Name) {
		std::cerr << "Current directory cannot be removed" << std::endl;
		dir.Valid = 0;
		return dir;
	}

	for (auto &entry : dir.TableOfEntries) {

		if (entry.valid==1) {
			temp = rmdir_helper(dir, entry.name.data());

			if (temp.Valid==0) {
				return temp;
			}

			dir = temp;
		}

		dir.TableOfEntries[offset].valid = 0;
	}

	fs_disk->read(static_cast<int>(meta_data_.Blocks - 1 - block_idx), block.Data);

	dir.Valid = 0;
	block.Directories[block_off] = dir;

	fs_disk->write(static_cast<int>(meta_data_.Blocks - 1 - block_idx), block.Data);

	parent.TableOfEntries[offset].valid = 0;

	write_dir_back(parent);

	dir_counter_[block_idx]--;

	return dir;

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