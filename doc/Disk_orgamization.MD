# Lower to hardware

# 1. Disk

## 1.1. Sector

` struct {
char[512] data;
int byte_count;
}
`

## 1.2 struct hdd

`{
int totalSectors;
int usedSectors;
int freeSectors;
struct sector* sectors;
};
`

## 1.3 struct block

` struct block
{
int usedSectors;
struct sector* sectors; //this is the base address of the first sector in block of 8 sectors
};
`

Block is pointing to the same address as the very first sector of hdd.sectors [sec_num / 8] 


