#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fs.h>

extern fsystem_t* fsd;

/*
 * fs_create - Return SYSERR if not enough space is available
 *     in the directory or the filename already exists.
 *     Otherwise:
 *        1. Find an available block on the block store
 *        2. Create an inode_t for the new file
 *        3. Update the root directory
 *        4. Write the inode and free bitmask back to the block device
 */
syscall fs_create(char* filename) {
	// checck for full dir
	if (fsd->root_dir.numentries == DIR_SIZE) return SYSERR;
	
	// check for duplicate name
	for (int i = 0; i < DIR_SIZE; i++)
		if (strncmp(fsd->root_dir.entry[i].name, filename, FILENAME_LEN) == 0)
			return SYSERR;

	// try to find a free block
	int free_blk_idx = -1;
	int offset = 2; // num bits to offset from initialization
	for (int i = offset; i < fsd->device.nblocks + offset; i++) {
		if (fs_getmaskbit(i) == 0) {
			free_blk_idx = i;
			break;
		}
	}
	// check if free block was found
	if (free_blk_idx == -1) return SYSERR;

	// Set the bit for the blk we're filling
	fs_setmaskbit(free_blk_idx);
	bs_write(BM_BIT, 0, fsd->freemask, fsd->freemasksz);

	// update the dir with the new file info
	int dir_entry_idx = fsd->root_dir.numentries;
	fsd->root_dir.numentries++;
	strcpy(fsd->root_dir.entry[dir_entry_idx].name, filename);
	fsd->root_dir.entry[dir_entry_idx].inode_block = free_blk_idx;
	
	// Create an inode and write it to the store
	inode_t inode;
	inode.id = free_blk_idx;
	inode.size = 0;
	bs_write(free_blk_idx, 0, (void*)&inode, sizeof(inode));
	
	return OK;
}
