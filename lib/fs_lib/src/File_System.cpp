#include "File_System.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>

namespace fs {

void FileSystem::debug(const std::shared_ptr<Disk> &disk) {

	Block block;

	disk->read(0, block.Data);

	std::cout << "SuperBlock:" << std::endl;

	if (block.Super.MagicNumber!=MAGIC_NUMBER) {
		std::cerr << "  Magic Number: " << block.Super.MagicNumber << " Is INVALID! Exiting... " << std::endl;
		return;
	}

	std::cout << "  Magic Number: " << std::hex << block.Super.MagicNumber << std::dec << std::endl;
	std::cout << "  Blocks: " << block.Super.Blocks << std::endl;
	std::cout << "  Inode Blocks: " << block.Super.InodeBlocks << std::endl;
	std::cout << "  Inodes: " << block.Super.Inodes << std::endl;

	//Iterate over InodeBlocks. Print out the Inodes size. directory blocks, indirect blocks, and data blocks

	uint32_t inode_count = 0; //TODO: Refactor to closure

	for (auto i = 1; i <= block.Super.InodeBlocks; ++i) {

		disk->read(i, block.Data);

		for (auto &node : block.Inodes) {

			if (node.Valid) {
				std::cout << "  Inode: " << inode_count << " Size: " << node.Size << std::endl;
				std::cout << "    Direct Blocks: ";

				for (auto &direct : node.Direct) {

					if (direct) {
						std::cout << direct << " ";
					}

				}

				std::cout << std::endl;

				if (node.Indirect) {
					std::cout << "    Indirect Blocks: ";

					disk->read(static_cast<int>(node.Indirect), block.Data);

					for (auto &indirect : block.Pointers) {

						if (indirect) {
							std::cout << indirect << " ";
						}
					}

					std::cout << std::endl;
				}
			}
		}
	}

	std::cout << "  Inode Size: " << sizeof(Inode) << std::endl;
	std::cout << "  Directory Size: " << sizeof(Directory) << std::endl;
	std::cout << "  Indirect Size: " << sizeof(std::uint32_t) << std::endl;
	std::cout << "  Data Size: " << sizeof(std::byte) << std::endl;
	std::cout << "  Directory Blocks: " << block.Super.DirBlocks << std::endl;

	std::cout << "  Protected: " << block.Super.Protected << std::endl;
	std::cout << "  PasswordHash Size: " << block.Super.PasswordHash.size() << std::endl;
}

bool FileSystem::format(const std::shared_ptr<Disk> &disk) {

	if (disk->mounted()) {
		return false;
	}

	Block block;

	block.Super.MagicNumber = MAGIC_NUMBER;
	block.Super.Blocks = static_cast<std::uint32_t>(disk->size());
	block.Super.InodeBlocks = (uint32_t)std::ceil((int(block.Super.Blocks)*1.00)/10);
	block.Super.Inodes = block.Super.InodeBlocks*INODES_PER_BLOCK;
	block.Super.DirBlocks = (uint32_t)std::ceil((int(block.Super.Blocks)*1.00)/100);

	// ------- 1-d Write super block -------
	disk->write(0, block.Data);

//	block.Super.Protected = 0; //Warning: Not saved to disk
//	block.Super.PasswordHash.fill(std::byte(0));

	for (auto i = 1; i <= block.Super.InodeBlocks; ++i) {
		Block inode_block;

		for (auto &inode : inode_block.Inodes) {
			inode.Valid = false;
			inode.Size = 0;
			inode.Direct.fill(0);
			inode.Indirect = 0;
		}

		// ------- 2-d Write inodes -------
		disk->write(i, inode_block.Data);
	}

	// ------- 3-d Write data blocks -------

	for (uint32_t i = block.Super.InodeBlocks + 1; i < block.Super.Blocks - block.Super.DirBlocks; ++i) {
		Block data_block;
		data_block.Data.fill(std::byte(0));
		disk->write(static_cast<int>(i), data_block.Data);
	}

	// ------- 4-d Write directory blocks -------

	for (auto i = block.Super.Blocks - block.Super.DirBlocks; i < block.Super.Blocks; ++i) {

		Block data_block;
		Directory dir;

		dir.I_num = -1;
		dir.Valid = 0;

		//fill dir.TableOfEntries with empty directories

		dir.TableOfEntries.fill(Dirent());

		data_block.Directories.fill(dir);

		disk->write(static_cast<int>(i), data_block.Data);

	}
	// ------- 5-d Write root directory -------


	Directory root;

	// Set root directory name
	root.Name[0] = '/';
	root.Name[1] = '\0';

	root.I_num = 0;
	root.Valid = 1;

	Dirent temp_entry;

	temp_entry.i_num = 0;
	temp_entry.type = 0;
	temp_entry.valid = 1;

	// Set root directory entries

	temp_entry.name[0] = '.';
	temp_entry.name[1] = '\0';

	root.TableOfEntries[0] = temp_entry;

	temp_entry.name[0] = '.';
	temp_entry.name[1] = '.';
	temp_entry.name[2] = '\0';

	root.TableOfEntries[1] = temp_entry;

	Block dir_block;

	dir_block.Directories[0] = root;

	// ------- 6-d Write root directory to disk -------
	disk->write(static_cast<int>(block.Super.Blocks - 1), dir_block.Data);

	return true;
}

FileSystem::~FileSystem() {

	if (not fs_disk) {
		std::cerr << "No disk mounted! FileSystem closing" << std::endl;
		return;
	}

	if (fs_disk->mounted()) {
		fs_disk->unmount();
	}

}

bool FileSystem::mount(const std::shared_ptr<Disk> &disk) {

	if (disk->mounted()) {
		return false;
	}

	Block block;
//FIXME: Directory blocks are not being read
//ERROR: Directory blocks are always empty
	disk->read(0, block.Data);

	if (block.Super.MagicNumber!=MAGIC_NUMBER) {
		return false;
	}

	if (block.Super.InodeBlocks!=std::ceil((block.Super.Blocks*1.00)/10)) {
		return false;
	}

	if (block.Super.Inodes!=block.Super.InodeBlocks*INODES_PER_BLOCK) {
		return false;
	}

	if (block.Super.DirBlocks!=(uint32_t)std::ceil((int(block.Super.Blocks)*1.00)/100)) {
		return false;
	}

//	if (block.Super.Protected) {
//		std::cout << "Enter password: ";
//
//		std::string password;
//		std::cin >> password;
//
//		//TODO : Implement password hashing
//
//		if (password=="admin") return true;
//
//		std::cout << "Wrong password!" << std::endl;
//
//		return false;
//	}

	disk->mount();

	fs_disk = disk;

	meta_data_ = block.Super;

	free_blocks_.resize(meta_data_.Blocks, false);

	inode_counter_.resize(meta_data_.Inodes, 0);

	free_blocks_[0] = true;

	// ------- 1-d Read inodes -------
	for (auto i = 1; i <= meta_data_.InodeBlocks; ++i) {
		disk->read(i, block.Data);

		for (auto &inode : block.Inodes) {

			if (inode.Valid) {
				inode_counter_[i - 1]++;
				free_blocks_[i] = true;

				for (auto &block_num : inode.Direct) {

					if (block_num) {

						if (block_num < meta_data_.Blocks) {
							free_blocks_[block_num] = true;
						} else {
							std::cerr << "Invalid block number in inode" << std::endl;
							return false;
						}

					}
				}

				if (inode.Indirect) {

					if (inode.Indirect < meta_data_.Blocks) {
						free_blocks_[inode.Indirect] = true;

						Block indirect_block;

						fs_disk->read(static_cast<int>(inode.Indirect), indirect_block.Data);

						for (auto &block_num : indirect_block.Pointers) {

							if (block_num) {

								if (block_num < meta_data_.Blocks) {
									free_blocks_[block_num] = true;
								} else {
									std::cerr << "Invalid block number in inode" << std::endl;
									return false;
								}
							}
						}
					} else {
						std::cerr << "Invalid block number in inode" << std::endl;
						return false;
					}
				}
			}
		}
	}

// ------ Resize dir_counter ------

	dir_counter_.resize(meta_data_.DirBlocks, 0);

	// ------- 2-d Read directory blocks -------

	Block dir_block;

	for (auto dirs = 0; dirs < meta_data_.DirBlocks; ++dirs) {
		disk->read(static_cast<int>(meta_data_.Blocks) - dirs - 1, dir_block.Data);

		for (auto &dir : dir_block.Directories) {

			if (dir.Valid) {
				dir_counter_[dirs]++;
				free_blocks_[meta_data_.Blocks - dirs - 1] = true;

				for (auto &entry : dir.TableOfEntries) {

					if (entry.valid) {

						if (entry.i_num < meta_data_.Inodes) {
							inode_counter_[entry.i_num]++;
						} else {
							std::cerr << "Invalid inode number in directory" << std::endl;
							return false;
						}
					}
				}

			}
		}

		if (dirs==0) {
			curDir = dir_block.Directories[0];
		}

	}
	mounted_ = true;

	return true;
}

void FileSystem::set_cur_disk(std::shared_ptr<Disk> &disk) {
	fs_disk = disk;
}

ssize_t FileSystem::create() {

	if (not mounted_) {
		std::cerr << "No disk mounted!" << std::endl;
		return -1;
	}

	Block block;

	fs_disk->read(0, block.Data);

	for (uint32_t i = 1; i <= meta_data_.InodeBlocks; i++) {

		if (inode_counter_[i - 1]==INODES_PER_BLOCK) {
			continue;
		}

		fs_disk->read(static_cast<int>(i), block.Data);

		for (uint32_t j = 0; j < INODES_PER_BLOCK; j++) {

			if (!block.Inodes[j].Valid) {
				block.Inodes[j].Valid = true;
				block.Inodes[j].Size = 0;
				block.Inodes[j].Indirect = 0;

				for (auto &block_num : block.Inodes[j].Direct) {
					block_num = 0;
				}

				free_blocks_[i] = true;
				inode_counter_[i - 1]++;

				fs_disk->write(static_cast<int>(i), block.Data);

				return (((i - 1)*INODES_PER_BLOCK) + j);
			}
		}
	}

	return -1;

}

bool FileSystem::remove(size_t i_number) {

	if (not mounted_) {
		std::cerr << "No disk mounted!" << std::endl;
		return false;
	}

	if ((i_number > meta_data_.Inodes) || (i_number < 1)) {
		std::cerr << "Invalid inode number!" << std::endl;
		return false;
	}

	Inode inode;

	if (not load_inode(i_number, inode)) {
		std::cerr << "Invalid inode number!" << std::endl;
		return false;
	}

	inode.Valid = false;
	inode.Size = 0;

	for (auto &block_num : inode.Direct) {

		if (block_num) {
			free_blocks_[block_num] = false;
			block_num = 0;
		}
	}

	if (inode.Indirect) {
		Block indirect_block;

		fs_disk->read(static_cast<int>(inode.Indirect), indirect_block.Data);

		free_blocks_[inode.Indirect] = false;
		inode.Indirect = 0;

		for (auto &block_num : indirect_block.Pointers) {

			if (block_num) {
				free_blocks_[block_num] = false;
				block_num = 0;
			}
		}

	}

	Block block;

	fs_disk->read(static_cast<int>(i_number/INODES_PER_BLOCK + 1), block.Data);
	block.Inodes[i_number%INODES_PER_BLOCK] = inode;
	fs_disk->write(static_cast<int>(i_number/INODES_PER_BLOCK + 1), block.Data);

	return true;
}

bool fs::FileSystem::load_inode(size_t i_number, FileSystem::Inode &inode) {

	if (not mounted_) {
		std::cerr << "No disk mounted!" << std::endl;
		return false;
	}

	if ((i_number > meta_data_.Inodes) || (i_number < 1)) {
		std::cerr << "Invalid inode number!" << std::endl;
		return false;
	}

	Block block;

	auto i = i_number/INODES_PER_BLOCK;
	auto j = i_number%INODES_PER_BLOCK;

	if (inode_counter_[i]) {
		fs_disk->read(static_cast<int>(i) + 1, block.Data);

		if (block.Inodes[j].Valid) {
			inode = block.Inodes[j];
			return true;
		}
	}

	return false;
}

ssize_t FileSystem::stat(size_t i_number) {

	if (not mounted_) {
		std::cerr << "No disk mounted!" << std::endl;
		return -1;
	}

	if ((i_number > meta_data_.Inodes) || (i_number < 1)) {
		std::cerr << "Invalid inode number!" << std::endl;
		return -1;
	}

	Inode inode;

	if (not load_inode(i_number, inode)) {
		std::cerr << "Invalid inode number!" << std::endl;
		return -1;
	}

	return inode.Size;
}

ssize_t FileSystem::read(size_t i_number, std::vector<std::byte> &data, size_t offset) {

	if (not mounted_) {
		std::cerr << "No disk mounted!" << std::endl;
		return -1;
	}

	auto inode_size = stat(i_number);
	int length = static_cast<int>(data.size());

	if (offset > inode_size) {
		std::cerr << "Offset is greater than file size!" << std::endl;
		return -1;
	}

	if (offset + length > inode_size) {
		length = static_cast<int>(inode_size - offset);
	} else {
		length = static_cast<int>(data.size());
	}

	Inode inode;

	// Tail ptr
	auto tail_ptr = data.begin();

	int to_read = length;

	if (offset < POINTERS_PER_INODE*Disk::BLOCK_SIZE) {
		std::uint32_t direct_node = offset/Disk::BLOCK_SIZE;
		offset = offset%Disk::BLOCK_SIZE;

		if (inode.Direct[direct_node]) {
			read_helper(inode.Direct[direct_node], static_cast<int>(offset), &length, data, tail_ptr);

			while (length > 0 && direct_node < POINTERS_PER_INODE && inode.Direct[direct_node]) {
				direct_node++;
				read_helper(inode.Direct[direct_node], 0, &length, data, tail_ptr);
			}

			if (length <= 0) {
				return to_read;
			}

			if (direct_node==POINTERS_PER_INODE && inode.Indirect) {
				Block indirect_block;

				fs_disk->read(static_cast<int>(inode.Indirect), indirect_block.Data);

				for (auto &block_num : indirect_block.Pointers) {

					if (block_num && length > 0) {
						read_helper(block_num, 0, &length, data, tail_ptr);
					} else {
						break;
					}
				}

				if (length <= 0) {
					return to_read;
				} else {
					return to_read - length;
				}
			} else {
				return to_read - length;
			}

		} else {
			return 0;
		}

	} else {

		if (not inode.Indirect) {
			offset -= POINTERS_PER_INODE*Disk::BLOCK_SIZE;
			std::uint32_t indirect_node = offset/Disk::BLOCK_SIZE;
			offset = offset%Disk::BLOCK_SIZE;

			Block indirect_block;

			fs_disk->read(static_cast<int>(inode.Indirect), indirect_block.Data);

			if (indirect_block.Pointers[indirect_node] && length > 0) {
				indirect_node++;
				read_helper(indirect_block.Pointers[indirect_node], static_cast<int>(offset), &length, data, tail_ptr);
			}

			for (auto &block_num : indirect_block.Pointers) {

				if (block_num && length > 0) {
					read_helper(block_num, 0, &length, data, tail_ptr);
				} else {
					break;
				}

			}

			if (length <= 0) {
				return to_read;
			} else {
				return to_read - length;
			}

		} else {
			return 0;
		}

	}

}

void FileSystem::read_helper(uint32_t block_num,
							 int offset,
							 int *length,
							 std::vector<std::byte> &data, //FIXME: remove
							 std::vector<std::byte>::iterator &tail_ptr) {

	Block block;

	fs_disk->read(static_cast<int>(block_num), block.Data);

	if (offset + (*length) > Disk::BLOCK_SIZE) {
		*length = static_cast<int>(Disk::BLOCK_SIZE - offset);
	}

	std::copy(block.Data.begin() + offset, block.Data.begin() + offset + (*length), tail_ptr);
	tail_ptr += (*length);
	*length -= (*length);

}

uint32_t FileSystem::allocate_block() {

	if (not mounted_) {
		std::cerr << "No disk mounted!" << std::endl;
		return 0;
	}

	for (uint32_t i = meta_data_.InodeBlocks + 1; i < meta_data_.Blocks; ++i) {

		if (free_blocks_[i]==0) {
			free_blocks_[i] = true;

			return static_cast<uint32_t>(i);
		}

	}

	return 0;
}

ssize_t FileSystem::write_ret(size_t i_number, FileSystem::Inode &node, int ret) {

	if (not mounted_) {
		std::cerr << "No disk mounted!" << std::endl;
		return -1;
	}

	int i = static_cast<int>(i_number/INODES_PER_BLOCK);
	int j = static_cast<int>(i_number%INODES_PER_BLOCK);

	Block block;

	fs_disk->read(i + 1, block.Data);
	block.Inodes[j] = node;
	fs_disk->write(i + 1, block.Data);

	return static_cast<ssize_t>(ret);

}

void FileSystem::read_buffer(int offset, int *read, int length, std::vector<std::byte> &data, uint32_t block_num) {
	if (not mounted_) {
		std::cerr << "No disk mounted!" << std::endl;
		return;
	}

	Block block;

	for (int i = offset; i < static_cast<int>(Disk::BLOCK_SIZE) && *read < length; i++) {
		block.Data[i] = data[*read];
		*read = *read + 1;
	}

	fs_disk->write(static_cast<int>(block_num), block.Data);

}
bool FileSystem::check_allocation(Inode inode,
								  int read,
								  int orig_offset,
								  uint32_t &block_num,
								  bool write_indirect,
								  FileSystem::Block indirect) {

	if (not mounted_) {
		std::cerr << "No disk mounted!" << std::endl;
		return false;
	}

	if (!block_num) {
		block_num = allocate_block();

		if (!block_num) {
			std::cerr << "No more blocks available!" << std::endl;
			inode.Size = orig_offset + read;

			if (write_indirect) {
				fs_disk->write(static_cast<int>(inode.Indirect), indirect.Data);
			}

			return false;
		}
	}

	return true;
}

ssize_t FileSystem::write(size_t i_number, std::vector<std::byte> &data, int length, size_t offset) {

	if (not mounted_) {
		std::cerr << "No disk mounted!" << std::endl;
		return -1;
	}

	Inode inode;
	Block indirect_block;
	int read = 0;
	int origin_offset = static_cast<int>(offset);

	if (length + offset > (POINTERS_PER_BLOCK + POINTERS_PER_INODE)*Disk::BLOCK_SIZE) {
		return -1;
	}

	if (!load_inode(i_number, inode)) {
		inode.Valid = true;
		inode.Size = length + offset;

		for (uint32_t i = 0; i < POINTERS_PER_INODE; i++) {
			inode.Direct[i] = 0;
		}

		inode.Indirect = 0;
		inode_counter_[i_number/INODES_PER_BLOCK]++;

		free_blocks_[i_number/INODES_PER_BLOCK + 1] = true;
	} else {
		inode.Size = std::max(static_cast<int>(inode.Size), length + static_cast<int>(offset));
	}

	if (offset < POINTERS_PER_INODE*Disk::BLOCK_SIZE) {

		int direct_node = static_cast<int>(offset/Disk::BLOCK_SIZE);
		offset %= Disk::BLOCK_SIZE;

		if (!check_allocation(inode, read, origin_offset, inode.Direct[direct_node], false, indirect_block)) {
			return write_ret(i_number, inode, read);
		}

		read_buffer(static_cast<int>(offset), &read, length, data, inode.Direct[direct_node++]);

		if (read==length) {
			return write_ret(i_number, inode, length);
		} else {

			for (int i = direct_node; i < static_cast<int>(POINTERS_PER_INODE); i++) {

				if (!check_allocation(inode, read, origin_offset, inode.Direct[direct_node], false, indirect_block)) {
					return write_ret(i_number, inode, read);
				}

				read_buffer(0, &read, length, data, inode.Direct[direct_node++]);

				if (read==length) {
					return write_ret(i_number, inode, length);
				}
			}

			if (inode.Indirect) {
				fs_disk->read(static_cast<int>(inode.Indirect), indirect_block.Data);
			} else {

				if (!check_allocation(inode, read, origin_offset, inode.Indirect, false, indirect_block)) {
					return write_ret(i_number, inode, read);
				}

				fs_disk->read(static_cast<int>(inode.Indirect), indirect_block.Data);

				for (int i = 0; i < static_cast<int>(POINTERS_PER_BLOCK); i++) {
					indirect_block.Pointers[i] = 0;
				}
			}

			for (int j = 0; j < static_cast<int>(POINTERS_PER_BLOCK); j++) {

				if (!check_allocation(inode, read, origin_offset, indirect_block.Pointers[j], true, indirect_block)) {
					return write_ret(i_number, inode, read);
				}

				read_buffer(0, &read, length, data, indirect_block.Pointers[j]);

				if (read==length) {
					fs_disk->write(static_cast<int>(inode.Indirect), indirect_block.Data);
					return write_ret(i_number, inode, length);
				}
			}

			fs_disk->write(static_cast<int>(inode.Indirect), indirect_block.Data);
			return write_ret(i_number, inode, read);
		}

	} else {
		offset -= (Disk::BLOCK_SIZE*POINTERS_PER_INODE);
		int indirect_node = static_cast<int>(offset/Disk::BLOCK_SIZE);
		offset %= Disk::BLOCK_SIZE;

		if (inode.Indirect) {
			fs_disk->read(static_cast<int>(inode.Indirect), indirect_block.Data);
		} else {

			if (!check_allocation(inode, read, origin_offset, inode.Indirect, false, indirect_block)) {
				return write_ret(i_number, inode, read);
			}

			fs_disk->read(static_cast<int>(inode.Indirect), indirect_block.Data);

			for (int i = 0; i < static_cast<int>(POINTERS_PER_BLOCK); i++) {
				indirect_block.Pointers[i] = 0;
			}
		}

		if (!check_allocation(inode,
							  read,
							  origin_offset,
							  indirect_block.Pointers[indirect_node],
							  true,
							  indirect_block)) {

			return write_ret(i_number, inode, read);
		}

		read_buffer(static_cast<int>(offset), &read, length, data, indirect_block.Pointers[indirect_node++]);

		if (read==length) {
			fs_disk->write(static_cast<int>(inode.Indirect), indirect_block.Data);
			return write_ret(i_number, inode, length);
		} else {

			for (int j = indirect_node; j < static_cast<int>(POINTERS_PER_BLOCK); j++) {

				if (!check_allocation(inode, read, origin_offset, indirect_block.Pointers[j], true, indirect_block)) {
					return write_ret(i_number, inode, read);
				}

				read_buffer(0, &read, length, data, indirect_block.Pointers[j]);

				if (read==length) {
					fs_disk->write(static_cast<int>(inode.Indirect), indirect_block.Data);
					return write_ret(i_number, inode, length);
				}
			}

			fs_disk->write(static_cast<int>(inode.Indirect), indirect_block.Data);
			return write_ret(i_number, inode, read);
		}
	}

	return -1; // should never get here
}

}