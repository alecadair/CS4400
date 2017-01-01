/*
 * mm-naive.c - The least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by allocating a
 * new page as needed.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* always use 16-byte alignment */
#define ALIGNMENT 16
//#define WSIZE 4 //word and header / footer size
//#define DSIZE 16 //word size in bytes -- was originally 8
#define CHUNK_SIZE (1 << 14) //extedn heap byt this much
#define CHUNK_ALIGN(size) (((size)+(CHUNK_SIZE-1)) & ~(CHUNK_SIZE-1))
#define PACK(size, alloc) ((size) | (alloc))
/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))
/* rounds up to the nearest multiple of mem_pagesize() */
#define PAGE_ALIGN(size) (((size) + (mem_pagesize()-1)) & ~(mem_pagesize()-1))
#define NODE_SIZE ALIGN(sizeof(heap_list_node))
#define SIZE_SIZE ALIGN(sizeof(size_t))
#define HDRP(bp) ((char *)(bp) - sizeof(node))
#define GET_SIZE(p) ((node *) (p))->size
#define MAX(x,y) ((x) >(y)?(x):(y))
//#define GET_ALLOC(p) (((node *) p())->allocated)
//#define NEXT_BLKP(bp) bp->next
#define OVERHEAD sizeof(node);

typedef struct heap_list_node heap_list_node;
typedef struct free_list_node freenode;
typedef struct chunk_header chunk_header;

void *current_avail = NULL;
int current_avail_size = 0;

heap_list_node* list_head = NULL;
heap_list_node* list_tail = NULL;
heap_list_node* first_free_block;

int line = -1;

struct heap_list_node{
	heap_list_node* next_free;
	heap_list_node* prev_free;
	void* filler1;
	void* filler2;
	size_t size;
	//void* next_free;
	//void* prev_free;
	heap_list_node *next;
	heap_list_node *prev;
	void* payload;
};

struct chunk_header{
	chunk_header *next;
	chunk_header *prev;
};

struct free_list_node{
	freenode *next;
};

static void* get_head(){
	return list_head;
}

static void* get_list_tail(){
	return list_tail;
}
//static void set_allocated(void *bp, size_t size) {
//	size_t extra_size = GET_SIZE(HDRP(bp)) - size;
//	if (extra_size > ALIGN(1 + OVERHEAD)) {
//		GET_SIZE(HDRP(bp)) = size;
//		GET_SIZE(HDRP(NEXT_BLKP(bp))) = extra_size;
//		GET_ALLOC(HDRP(NEXT_BLKP(bp))) = 0;
//	}
//	GET_ALLOC(HDRP(bp)) = 1;
//}
static void print_heads(){
  printf("base head %p and last head %p ", list_head, list_tail);
}

void insert_free_node_front(heap_list_node* bp){
	heap_list_node* first_free = first_free_block;
	first_free->prev_free = bp;
	bp->next = first_free;
	bp->prev = 0;
	first_free_block = bp;
	//bp->next_free = first_free_block;
//	while(first_free->next_free!= NULL){
//		first_free = first_free->next_free;
//	}
//	first_free->next_free = bp;
//	bp->prev_free = first_free;
	//bp->next_free = 0;
	//bp->prev_free = first_free;

	//first_free_block = bp;
	return;
}

void add_to_chunk_list(heap_list_node*bp){

}

void remove_from_free_list(heap_list_node* bp){

	//heap_list_node* temp = first_free_block;
	if((bp->next_free != NULL) && (bp->prev_free != NULL)){//normal node
		bp->prev->next = bp->next;
		bp->next->prev = bp->prev;
		bp->next_free = 0;
		bp->next_free = 0;
		return;
	}
	else if(bp->next_free == NULL && bp->prev_free != NULL){//tail node
		bp->prev_free->next_free = 0;
		bp->prev_free = 0;
		bp->next_free = 0;
	}else if(bp->next_free != NULL && bp->prev_free == NULL){//head node
		first_free_block = bp->next_free;
		bp->next_free->prev_free =0;
		bp->next_free = 0;
		bp->prev_free = 0;
		return;
	}
	else {
		return;
	}
	//	while((temp != NULL) && (temp != bp)){
//		temp = temp->prev;
//	}
//	if(temp){
//		if(temp->prev_free!= 0)
//			temp->prev_free->next_free = temp->next_free;
//		if(temp->next_free != 0)
//			temp->next_free->prev_free = temp->prev_free;
//	}
//	bp->next_free = 0;
//	bp->prev_free = 0;
//	return;

}

static void check_heap(){
	heap_list_node * current = get_head();
	while(current < (heap_list_node *)get_list_tail()){
		printf("the block %s is at %p, and its size is %d\n", (current->size&1)?"1":"0",
				current, (int)(current->size));
		current = (heap_list_node *)((char *)current + (current->size & ~1));
	}
}
static heap_list_node *find_fit(size_t size){
	//check_heap();
	heap_list_node * node;
	node = first_free_block;
	while(node != NULL){
		if(((node->size) >=(size)) && (!(node->size & 1))){
			return node;
		}
		node = node->next_free;
	}
	return NULL;
//	for(i = ((heap_list_node *)get_head())->next; i != get_head() && i->size < size; i = i->next);
//
//	if(i != get_head())
//		return i;
//	else
//		return NULL;
}


/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
	//printf("\ninit\n");
	list_head = NULL;
	list_head = mem_map(8*PAGE_ALIGN(1 /*+sizeof(heap_list_node)*/));
	list_head->size = 8*PAGE_ALIGN(1)-sizeof(heap_list_node);
	//list_head->size  &=  ~0xf;
	list_head->next = NULL;
	list_head->prev = NULL;
	list_head->payload = &(list_head->payload);
	list_head->payload += 0x8;
	list_tail = list_head;
	first_free_block = list_head;
	first_free_block->next_free = 0;
	first_free_block->prev_free = 0;
  return 0;
}

/*
 * mm_malloc - Allocate a block by using bytes from current_avail,
 *     grabbing a new page if necessary.
 */
void *mm_malloc(size_t size)
{
  line++;
  int offsetandsize = MAX(ALIGN(sizeof(heap_list_node)+size),ALIGN(size + NODE_SIZE));
  heap_list_node *best_fit = find_fit(offsetandsize) /*+ sizeof(heap_list_node)*/;

  if(best_fit == NULL){
	 // printf("best_fit NULL  %d\n",offsetandsize);
	  size_t size_to_allocate = 4*PAGE_ALIGN(offsetandsize);
	  best_fit = mem_map(size_to_allocate);
	  best_fit->prev = list_tail;
	  list_tail->next = best_fit;
	  list_tail = best_fit;
	  best_fit->next = 0;
//	  size_t subsize = PAGE_ALIGN(offsetandsize +sizeof(heap_list_node));
//	  size_t newsize = (size_to_allocate - sizeof(heap_list_node));
	  best_fit->size = offsetandsize - sizeof(heap_list_node);
	  void* newpayload = &(best_fit->payload);
	  newpayload += 0x8;
	  best_fit->payload = newpayload;
	  heap_list_node* next_node = (heap_list_node*)((newpayload) + /*newsize*/ best_fit->size);
	  best_fit->size |= 1;
	  next_node->prev = best_fit;
	  best_fit->next = next_node;
	  next_node->next = 0;
	  next_node->payload = ((&(next_node->payload)) + 0x8);
	  size_t subsize2 = size_to_allocate - best_fit->size;
	  next_node->size = subsize2 - sizeof(heap_list_node);
	  //insert_free_node_front(next_node);
	  list_tail = next_node;
	  insert_free_node_front(next_node);
	  remove_from_free_list(best_fit);

	  return best_fit->payload;
  }else{
	 // printf("best fit %p \t %d\n",best_fit,offsetandsize);
	  void*payload = best_fit->payload;
	  size_t total_size = best_fit->size&~0xf;
	  heap_list_node* new_node = (heap_list_node*)(payload + (offsetandsize));
	  new_node->next = best_fit->next;
	  if(best_fit->next != NULL){
		  best_fit->next->prev = new_node;
	  }else{
		  best_fit->next = new_node;
	  }
	  new_node->prev = best_fit;
	 // best_fit->next = new_node;
	  best_fit->size = offsetandsize;
	  new_node->size = (total_size - offsetandsize) - sizeof(heap_list_node);
	 // new_node->size = best_fit->size - sizeof(heap_list_node) - offsetandsize;
	  best_fit->size |= 1;
	  insert_free_node_front(new_node);

	  remove_from_free_list(best_fit);
	 // insert_free_node_front(new_node);
	  void* new = &(new_node->payload);
	  new_node->payload = new + 0x8;
	  return payload;

	 // best_fit->prev->next = best_fit->next;
	  //best_fit->next->prev = best_fit->prev;
  }
  //return (void *)best_fit + NODE_SIZE;
  return NULL;
}


void mm_free(void *ptr)
{
	//printf("free %p\n",ptr-sizeof(heap_list_node));
	line++;
	//remove_from_free_list(ptr);
	heap_list_node* to_free = (heap_list_node*)(ptr - sizeof(heap_list_node));
	to_free->size &= ~0xf;
	insert_free_node_front(to_free);
//	heap_list_node* to_free = (heap_list_node*)(ptr - sizeof(heap_list_node));
//	to_free->size &= ~0xf;
//	insert_free_node_front(to_free);


	//to_free->size = ALIGN(to_free->size);
	//to_free->size &= ~ 0x1;
//	heap_list_node * current = ptr - NODE_SIZE;
//	heap_list_node * first = get_head();
//	current ->size &= ~1;
//	current->next = first ->next;
//	current->prev = first;
//	first->next = current;
//	current->next->prev = current;
}
