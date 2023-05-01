#include <xinu.h>
#include <future_prodcons.h>
#include <stddef.h>

sid32 print_sem;

uint future_prod(future_t *fut, int value) {
  int status;
  wait(print_sem);
  printf("Producing %d\n", value);
  signal(print_sem);
  status = (int) future_set(fut, (void*)&value);
  if (status < 1) {
    wait(print_sem);
    printf("future_set failed\n");
    signal(print_sem);
    return -1;
  }
  return OK;
}

uint future_cons(future_t *fut) {
  int i = NULL;
  int status;
  status = (int) future_get(fut, (void*)&i);
  if (status < 1) {
    wait(print_sem);
    printf("future_get failed\n");
    signal(print_sem);
    return -1;
  }
  wait(print_sem);
  printf("Consumed %d\n", i); //*i);
  signal(print_sem);
  return OK;
}

