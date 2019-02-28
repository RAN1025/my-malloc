#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct _information_t information_t;
//information block
struct _information_t{
  size_t size;
  information_t *next;
};

//check free list
void *ff_checklist(size_t size);
void *bf_checklist(size_t size);

//malloc with sbrk
void *sbrk_malloc(size_t size);

//split helper                                                                                                          
void *split_helper(information_t *free,size_t size);

//first fit malloc and free
void *ff_malloc(size_t size);
void ff_free(void *ptr);

//add to free list
void add_helper(information_t *block);

//merge adjacent free blocks
void merge_helper(information_t *block);

//best fit malloc and free
void *bf_malloc(size_t size);
void bf_free(void *ptr);

//test
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();

#endif
