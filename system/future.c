#include <xinu.h>
#include <stdbool.h>
#include <future.h>

future_t* future_alloc(future_mode_t mode, uint size, uint nelem) {  
	intmask mask = disable();
	if (mode != FUTURE_EXCLUSIVE) {
  		restore(mask);
		return SYSERR;
	}
	
	future_t* fut;

	fut = (future_t*) getmem(sizeof(future_t));//GETMEM WILL ALLOCATE MEMORY OR RETURN syserr
	fut->state = FUTURE_EMPTY;
	fut->size = size;
	fut->data = getmem(size);
	/*
 	int fut_idx = 0; //index of free future
	bool fut_is_free = false;
       	for (fut_idx = 0; fut_idx < NFUT; fut++) {
		if (futtab[fut_idx].state == FUTURE_EMPTY) {
			futtab[fut_idx].state = FUTURE_WAITING;
			fut_is_free = true;
			break;
		}
	}	
	if (!fut_is_free) return SYSERR;
	*/

  	restore(mask);
  	return fut;
}

syscall future_free(future_t* f) {
	intmask mask = disable();
	
	if (f->state == FUTURE_WAITING) kill(f->pid);
	
	freemem((char*)f->data, f->size);
	restore(mask);
	return freemem((char*)f, sizeof(future_t));
}

syscall future_get(future_t* f, void* out) {
	intmask mask = disable();
	if (f->state == FUTURE_WAITING) {
  		restore(mask);
		return SYSERR;
	}

	if (f->state == FUTURE_EMPTY) {
		f->state = FUTURE_WAITING;
		f->pid = getpid();
		suspend(f->pid);
	}

	memcpy(out, f->data, f->size);

	f->state = FUTURE_EMPTY;
	restore(mask);
        return OK;
}

syscall future_set(future_t* f, void* in) {
  	printf("fset: f->state%d\n", f->state);
	intmask mask = disable();
	if (f->state == FUTURE_READY) {
  		restore(mask);
		return SYSERR;
	}

	memcpy(f->data, in, f->size);
	if (f->state == FUTURE_WAITING) resume(f->pid);	
	f->state = FUTURE_READY;

	restore(mask);
        return OK;
}
