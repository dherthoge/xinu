#include <xinu.h>
#include <heap.h>

extern heap_t heaptab[];

void initheap(char* startaddr, uint32 size) {
	/* This function is called when the heap is constructed */
	/*   Use as needed to setup the free list               */
       	if (size == 0) return SYSERR;
	size = (uint32) roundmb(size);

	if (startaddr == NULL) return SYSERR;

	//heaptab[getpid()].freelist = startaddr;

	struct memblk* initblock = (struct memblk*) startaddr;
	initblock->mnext = NULL;
	initblock->mlength = size;
	initblock->mnext == NULL;
	return;
}

void* malloc(uint32 size) {
	/* Implement memory allocation within the process heap here       */
	/*   Your implementation MUST NOT use getmem                      */
	/*   Your implementation SHOULD use explicit in-memory free lists */
	if (size == 0) return (char*) SYSERR;
	
	struct memblk* startaddr = (struct memblk*) heaptab[getpid()].startaddr;
	struct memblk* maxaddr = (struct memblk*) (heaptab[getpid()].startaddr + heaptab[getpid()].size);
//printf("startaddr: %d\n", startaddr);
//printf("maxaddr: %d\n", maxaddr);

//printf("size: %d\n", size);
	size = (uint32) roundmb(size);
	int bytes = size/sizeof(struct memblk);
	
//printf("heaptab[getpid()].freelist: %d\n", heaptab[getpid()].freelist);  	
	struct memblk* freeblk = (struct memblk*) heaptab[getpid()].freelist;	
	struct memblk* prevblk = NULL;
//printf("freeblk->mnext: %d\n", freeblk->mnext);
//printf("freeblk->mlength: %d\n", freeblk->mlength);

	while (freeblk >= startaddr && freeblk < maxaddr && freeblk->mlength < size) {    	
//printf("freeblk: %d\n", freeblk);
		prevblk = freeblk;
	       	freeblk = freeblk->mnext;
	}
//printf("freeblk: %d\n", freeblk);

	if (freeblk == NULL) return (char*) SYSERR;
	
	if (prevblk == NULL) heaptab[getpid()].freelist = freeblk + bytes;
		
	struct memblk* newblk = freeblk + bytes;
	newblk->mlength = freeblk->mlength - size;
	newblk->mnext = freeblk->mnext;
//printf("newblk: %d\n", newblk);
//printf("newblk->mlength: %d\n", newblk->mlength);	

	if (prevblk != NULL) prevblk->mnext = newblk;
//printf("prevblk: %d\n", prevblk);
//printf("prevblk->mnext: %d\n", prevblk->mnext);

	freeblk->mlength = size;
	freeblk->mnext = NULL;

	return (void*) freeblk;
}

void free(char* block, uint32 size) {
	/* Implement memory free within process heap here */
	/*   Your implementation MUST implement coalesing */
	 
	
	if (size == 0) return (char*) SYSERR;
	
	struct memblk* newblk = (struct memblk*) block;

	struct memblk* startaddr = (struct memblk*) heaptab[getpid()].startaddr;
	struct memblk* maxaddr = (struct memblk*) (heaptab[getpid()].startaddr + heaptab[getpid()].size);
printf("startaddr: %d\n", startaddr);
printf("maxaddr: %d\n", maxaddr);
	size = (uint32) roundmb(size);
	
//printf("heaptab[getpid()].freelist: %d\n", heaptab[getpid()].freelist);  	
	struct memblk* currblk = (struct memblk*) heaptab[getpid()].freelist;	
	struct memblk* prevblk = NULL;

	// Find the position newblk should be added to
	while (currblk >= startaddr && currblk < maxaddr && newblk < currblk) {
		prevblk = currblk;
		currblk = currblk->mnext;
	}

	// Add newblk to the freelist
	newblk->mlength = size;
	if (newblk == heaptab[getpid()].freelist) {
		newblk->mnext = heaptab[getpid()].freelist;
	       	heaptab[getpid()].freelist = newblk;
	} else {
                newblk->mnext = prevblk->mnext;
		prevblk->mnext = newblk;

	}

	// Coalesce
	while (currblk >= startaddr && currblk < maxaddr) {
		if (((char*) currblk) + currblk->mlength == (char*) newblk) {
			currblk->mlength = currblk->mlength + newblk->mlength;
			currblk->mnext = newblk->mnext;
		}

		if (((char*) newblk) + newblk->mlength == (char*) currblk) {
			newblk->mlength = newblk->mlength + currblk->mlength;
                        newblk->mnext = currblk->mnext;
		}
	}
	

	return;

}
