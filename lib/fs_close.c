#include <xinu.h>
#include <fs.h>

extern fsystem_t* fsd;
extern filetable_t oft[NUM_FD];


/*
 * fs_close - Return SYSERR if the file is already closed
 *     otherwise, write the file's inode back to the block
 *     device and set the entry in the oft to FSTATE_CLOSED
 */
syscall fs_close(int fd) {
	int oft_entry_idx = -1;
	for (int i = 0; i < NUM_FD; i++) {
		if (fd == oft[i].de && oft[i].state == FSTATE_CLOSED) return SYSERR;
		else if (fd == oft[i].de) {
			oft_entry_idx = i;
			break;
		}
	}
	if (oft_entry_idx == -1) return SYSERR;
	
	filetable_t oft_entry = oft[oft_entry_idx];
	oft_entry.state = FSTATE_CLOSED;
	bs_write(oft_entry.fileptr, 0, (void*)&oft_entry.in, sizeof(inode_t)); 

	oft[oft_entry_idx] = oft_entry;

	return OK;
}
