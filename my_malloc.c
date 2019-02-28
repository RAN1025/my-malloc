#include "my_malloc.h"

//free list
information_t *listhead=NULL;
//test
size_t data_segment_size=0;


//check free list
//if requires sbrk, return NULL
//else return the pointer to the free block
void *ff_checklist(size_t size){
  information_t *current=listhead;
  //exist free block
  while(current!=NULL){
    //first free block with enough space
    if(current->size >= size){
      return current;
    }
    else{
      current=current->next;
    }
  }
  return NULL;
}

void *bf_checklist(size_t size){
  information_t *current=listhead;
  information_t *bestfit=NULL;
  //exist free block
  while(current!=NULL){
    //best free block with enough space
    if(current->size >= size){
      //return directly
      if(current->size <=(size+sizeof(information_t))){
	bestfit=current;
	return bestfit;
      }
      //record
      if(bestfit==NULL){
	bestfit=current;
      }
      else if(current->size < bestfit->size){
	bestfit=current;
      }
    }
    current=current->next;
  }
  return bestfit;
}

//malloc with sbrk
//set the information block
void *sbrk_malloc(size_t size){
  information_t *newblock=sbrk(0);
  void *realblock=sbrk(size+sizeof(information_t));
  data_segment_size = data_segment_size+size+sizeof(information_t);
  //sbrk fail
  if(realblock==(void*)-1){
    return NULL;
  }
  newblock->size=size;
  newblock->next=NULL;
  return newblock;
}

//split helper
//adjust the free list
//return the answer for the malloc function
void *split_helper(information_t *free,size_t size){
  //check wether requires split
  if(free->size <=(size+sizeof(information_t))){
    //do not need to split, return the pointer
    //locates at head
    if(free==listhead){
      listhead=free->next;
      return free+1;
    }
    else{
      information_t *current = listhead;
      //find the previous block
      while(current->next!=free){
	current=current->next;
      }
      current->next=free->next;
      return free+1;
    }
  }
  else{
    //split
    free->size=free->size-size-sizeof(information_t);
    //new information block
    information_t *answer=(information_t*)((char *)free+sizeof(information_t)+free->size);
    answer->size=size;
    answer->next=NULL;
    return answer+1;
  }
}

//first fit malloc
void *ff_malloc(size_t size){
  if(size<=0){
    return NULL;
  }
  //check free list
  information_t *free =(information_t*)ff_checklist(size);
  //sbrk
  if(free==NULL){
    information_t *answer=(information_t*)sbrk_malloc(size);
    return answer+1;
  }
  else{
    information_t *answer=(information_t*)split_helper(free,size);
    return answer;
  }
}

//best fit malloc
void *bf_malloc(size_t size){
  if(size<=0){
    return NULL;
  }
  //check free list
  information_t *free =(information_t*)bf_checklist(size);
  //sbrk
  if(free==NULL){
    information_t *answer=(information_t*)sbrk_malloc(size);
    return answer+1;
  }
  else{
    information_t *answer=(information_t*)split_helper(free,size);
    return answer;
  }
}

//add helper
//add newly freed block to the free list
void add_helper(information_t *block){
  information_t *current=listhead;
  if(current>block){
    listhead=block;
    block->next=current;
  }
  else{
    while(current->next!=NULL && current->next<block){
	current=current->next;
    }
    block->next=current->next;
    current->next=block;
  }
}

//merge adjacent free blocks
void merge_helper(information_t *block){
  char *current=(char *)block;
  //merge block with the next block
  if(block->next!=NULL){
    if((current+sizeof(information_t)+block->size)==(char*)block->next){
      block->size=block->size+sizeof(information_t)+(block->next)->size;
      block->next=(block->next)->next;
    }
  }
  //merge block with the previous block
  if(block!=listhead){
    information_t *previous=listhead;
    while(previous->next!=block){
      previous=previous->next;
    }
    if((current-previous->size-sizeof(information_t))==(char*)previous){
      previous->size=previous->size+sizeof(information_t)+block->size;
      previous->next=block->next;
    }
  }
}

//first fit free
void ff_free(void *ptr){
  if(ptr==NULL){
    return;
  }
  //empty free list
  if(listhead==NULL){
    information_t *block=(information_t*)((char *)ptr-sizeof(information_t));
    listhead=block;
  }
  else{
    information_t *block=(information_t*)((char *)ptr-sizeof(information_t));
    add_helper(block);
    merge_helper(block);
  }
}

//best fit free
void bf_free(void *ptr){
  if(ptr==NULL){
    return;
  }
  //empty free list                                                                                      
  if(listhead==NULL){
    information_t *block=(information_t*)((char *)ptr-sizeof(information_t));
    listhead=block;
  }
  else{
    information_t *block=(information_t*)((char *)ptr-sizeof(information_t));
    add_helper(block);
    merge_helper(block);
  }
}

//test                                                                                          
unsigned long get_data_segment_size(){
  return (unsigned long)data_segment_size;
}

unsigned long get_data_segment_free_space_size(){
  size_t free_space_size=0;
  information_t *current=listhead;
  while(current!=NULL){
    free_space_size =free_space_size+current->size+sizeof(information_t);
    current=current->next;
  }
  return (unsigned long)free_space_size;
}
