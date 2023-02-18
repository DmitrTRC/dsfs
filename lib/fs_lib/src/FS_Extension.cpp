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
FileSystem::Directory FileSystem::rmdir_helper(Directory parent, std::array<char, NAME_SIZE> name) {

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

	int offset = dir_lookup(parent, static_cast<const std::string &>(name.data()));

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
			temp = rm_helper(dir, entry.name);

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

FileSystem::Directory FileSystem::rm_helper(Directory dir, std::array<char, NAME_SIZE> name) {

	if (not mounted_) {
		std::cerr << "File system is not mounted" << std::endl;
		dir.Valid = 0;
		return dir;
	}

	int offset = dir_lookup(dir, static_cast<const std::string &>(name.data()));

	if (offset==-1) {
		std::cerr << "No such file/directory" << std::endl;
		dir.Valid = 0;
		return dir;
	}

	if (dir.TableOfEntries[offset].type==0) {
		return rmdir_helper(dir, name);
	}

	uint32_t i_num = dir.TableOfEntries[offset].i_num;

	std::cout << "Removing Inode " << i_num << std::endl;

	if (not remove(i_num)) {
		std::cerr << "Failed to remove Inode" << std::endl;
		dir.Valid = 0;
		return dir;
	}

	dir.TableOfEntries[offset].valid = 0;

	write_dir_back(dir);

	return dir;

}

bool FileSystem::rmdir(const std::array<char, NAME_SIZE> &name) {

	if (!mounted_) {
		std::cout << "File system is not mounted" << std::endl;
		return false;
	}

	Directory temp_dir = rmdir_helper(curDir, name);
	if (temp_dir.Valid==0) {
		curDir = temp_dir;
		return true;
	}
	return false;
}
bool FileSystem::touch(const std::array<char, NAME_SIZE> &name) {

	if (!mounted_) {
		std::cout << "File system is not mounted" << std::endl;
		return false;
	}

	for (auto &entry : curDir.TableOfEntries) {
		if (entry.valid==1) {
			if (entry.name==name) {
				std::cout << "File already exists" << std::endl;
				return false;
			}
		}
	}

	ssize_t new_node_idx = FileSystem::create();

	if (new_node_idx==-1) {
		std::cout << "Error creating new inode" << std::endl;
		return false;
	}

	Directory temp_dir = add_dir_entry(curDir, new_node_idx, 1, name.data());

	if (temp_dir.Valid==0) {
		std::cout << "Error adding new file" << std::endl;
		return false;
	}

	curDir = temp_dir;

	write_dir_back(curDir);

	return true;
}
bool FileSystem::cd(const std::array<char, NAME_SIZE> &name) {

	if (not mounted_) {
		std::cout << "File system is not mounted" << std::endl;
		return false;
	}

	int offset = dir_lookup(curDir, static_cast<const std::string &>(name.data()));

	if (offset==-1) {
		std::cout << "No such directory" << std::endl;
		return false;
	}

	Directory temp_dir = read_dir_from_offset(offset);

	if (temp_dir.Valid==0) {
		std::cout << "Directory invalid" << std::endl;
		return false;
	}

	curDir = temp_dir;

	return true;

}
bool FileSystem::ls() {

	if (not mounted_) {
		std::cout << "File system is not mounted" << std::endl;
		return false;
	}

	auto name = ".";

	return ls_dir(name);

}
bool FileSystem::rm(const std::array<char, NAME_SIZE> &name) {

	if (not mounted_) {
		std::cout << "File system is not mounted" << std::endl;
		return false;
	}

	Directory temp_dir = rm_helper(curDir, name);

	if (temp_dir.Valid==1) {
		curDir = temp_dir;
		return true;
	}

	return false;
}
void FileSystem::exit() {

	if (not mounted_) {
		std::cout << "File system is not mounted" << std::endl;
		return;
	}

	fs_disk->unmount();

	mounted_ = false;

}
bool FileSystem::copyout(const std::string &path, const std::string &name) {

	if (!mounted_) {
		std::cout << "File system is not mounted" << std::endl;
		return false;
	}

	int offset = dir_lookup(curDir, name);

	if (offset==-1) {
		std::cout << "No such file" << std::endl;
		return false;
	}

	if (curDir.TableOfEntries[offset].type==0) {
		std::cout << "Not a file" << std::endl;
		return false;
	}

	uint32_t i_num = curDir.TableOfEntries[offset].i_num;

	std::fstream stream(name, std::ios::binary | std::ios::out);

	if (not stream.is_open()) {
		std::cout << "Error opening file" << std::endl;
		std::cout << "Error: " << strerror(errno) << std::endl;
		return false;
	}

	offset = 0;
	std::vector<std::byte> buffer(Disk::BLOCK_SIZE, std::byte(0));

	do {
		ssize_t result = read(i_num, buffer, offset);

		if (result <= 0) {
			break;
		}

		stream.write(reinterpret_cast<const char *>(buffer.data()), static_cast<int>(buffer.size()));
		offset += static_cast<int>(result);

	} while (not buffer.empty());

	stream.close();
	std::cout << "Bytes copied: " << offset << std::endl;

	return true;
}
bool FileSystem::copyin(const std::string path, const std::string name) {

	if (not mounted_) {
		std::cerr << "File system is not mounted" << std::endl;
		return false;
	}

	std::array<char, NAME_SIZE> name_array = {0};

	std::copy(name.begin(), name.end(), name_array.begin());
	touch(name_array);

	int offset = dir_lookup(curDir, name);

	if (offset==-1) {
		std::cerr << "Error adding new file" << std::endl;
		return false;
	}

	if (curDir.TableOfEntries[offset].type==0) {
		std::cerr << "Not a file" << std::endl;
		return false;
	}

	uint32_t i_num = curDir.TableOfEntries[offset].i_num;

	std::fstream external_file(path, std::ios::binary | std::ios::in);

	if (not external_file.is_open() || not external_file.good()) {
		std::cerr << "Error opening file" << std::endl;
		std::cerr << "Error: " << strerror(errno) << std::endl;
		return false;
	}

	offset = 0;
	std::vector<std::byte> buffer(Disk::BLOCK_SIZE, std::byte(0));

	do {
		external_file.read(reinterpret_cast<char *>(buffer.data()), static_cast<int>(buffer.size()));
		ssize_t result = external_file.gcount();

		if (result <= 0) {
			break;
		}

		ssize_t actual = write(i_num, buffer, result, offset);

		if (actual < 0) {
			std::cerr << "fs.write returned invalid result " << actual << std::endl;
			break;
		}

		offset += actual;

		if (actual!=result) {
			std::cerr << "fs.write only wrote " << actual << " bytes, not " << result << " bytes" << std::endl;
			break;
		}

	} while (not buffer.empty());

	external_file.close();
	std::cout << "Bytes copied: " << offset << std::endl;

	return true;

}

}