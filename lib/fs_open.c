#include <xinu.h>
#include <fs.h>

extern fsystem_t* fsd;
extern filetable_t oft[NUM_FD];

/*
 * fs_open - Return SYSERR if the file is already open or does not exist
 *     otherwise, add a record to the oft in the first current FSTATE_CLOSED
 *     slot corresponding to the opened file and copy the inode from the block
 *     device to the oft record and return the index into the oft table for the
 *     newly opened file.
 */
int fs_open(char* filename, int flags) {
	int fd = -1; // dir entry idx
	for (int i = 0; i < fsd->root_dir.numentries; i++) {
		if (strncmp(fsd->root_dir.entry[i].name, filename, FILENAME_LEN) == 0) {
			fd = i;
			break;
		}
	}
	if (fd == -1) return SYSERR;
	
	int oft_entry_idx = -1;
	for (int i = 0; i < NUM_FD; i++) {
		// see if file is open
		if (fd == oft[i].de && oft[i].state == FSTATE_OPEN) return SYSERR;
		// find the first closed oft entry
		if (oft_entry_idx == -1 && oft[i].state == FSTATE_CLOSED) oft_entry_idx = i;
	}

	dirent_t dirent = fsd->root_dir.entry[fd];
	inode_t inode;
	bs_read(dirent.inode_block, 0, (void*)&inode, sizeof(inode_t));
	
	filetable_t oft_entry = oft[oft_entry_idx];
	oft_entry.state = FSTATE_OPEN;
	oft_entry.fileptr = dirent.inode_block;
	oft_entry.de = fd;
	bs_read(dirent.inode_block, 0, (void*)&oft_entry.in, sizeof(inode_t));
	oft_entry.flag = flags;

	oft[oft_entry_idx] = oft_entry;
	return oft_entry_idx;
}
