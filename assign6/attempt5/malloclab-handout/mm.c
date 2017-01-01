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
#define HDRP(bp) ((char *)(bp) - sizeof(heap_list_node))
#define GET_SIZE(p) ((heap_list_node*)(p))->size
#define GET_ALLOC(p) (((heap_list_node*)(p))->size & 0x1)
#define NEXT_FREE_BLKP(p)((heap_list_node*)(p))->next_free;
#define PREV_FREE_BLKP(p)((heap_list_node*)(p))->prev_free;
#define NEXT_BLKP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)))

//#define GET_SIZE(p) ((node *) (p))->size
#define MAX(x,y) ((x) >(y)?(x):(y))
//#define GET_ALLOC(p) (((node *) p())->allocated)
//#define NEXT_BLKP(bp) bp->next
#define OVERHEAD sizeof(heap_list_node);

typedef struct heap_list_node heap_list_node;
typedef struct free_list_node freenode;
typedef struct chunk_header chunk_header;

void *current_avail = NULL;
int current_avail_size = 0;

heap_list_node* list_head = NULL;
heap_list_node* list_tail = NULL;
heap_list_node* first_free_block;


char print = 1;

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
static void print_heads(){
  printf("base head %p and last head %p ", list_head, list_tail);
}
void insert_heap_list_node(heap_list_node*bp){
	//heap_list_node* first_block = list_head;
	list_head->prev = bp;
	bp->next = list_head;
	list_head = bp;
}
void insert_free_node_front(heap_list_node* bp){
	heap_list_node* first_free = first_free_block;
	if(!first_free_block){
		first_free_block = bp;
		return;
	}
	first_free->prev = bp;
	if(first_free){
		first_free->prev = bp;
		bp->next = first_free;
		bp->prev = 0;
		first_free = bp;
	}
	else{
		first_free = bp;
		first_free->next = 0;
		first_free->prev = 0;
	}
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
		first_free_block = 0;
		bp->next_free = 0;
		bp->prev_free = 0;
		return;
	}
}


static heap_list_node* find_best_fit(size_t new_size){
	heap_list_node* best_block = NULL;
	heap_list_node* bp = first_free_block;
	while(bp!= NULL){
		if((!((bp->size) & 0x1)) && (bp->size >= new_size)){
			if(!best_block || (bp->size < best_block->size)){
				best_block = bp;
			}
		}
		bp = bp->next_free;
	}
	if(best_block){
		//best_block->size = new_size;
		return best_block;
	}
	return NULL;
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
	if(print)
		printf("\ninit\n\n");
	list_head = NULL;
	list_head = mem_map(PAGE_ALIGN(1 /*+sizeof(heap_list_node)*/));
	list_head->size = PAGE_ALIGN(1)-sizeof(heap_list_node);
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
  heap_list_node *best_fit = find_best_fit(offsetandsize) /*+ sizeof(heap_list_node)*/;
  if(best_fit == NULL){
	  if(print)
		  printf("best>fit NULL  %d\n",offsetandsize);
	  size_t size_to_allocate = PAGE_ALIGN(offsetandsize);
	  best_fit = mem_map(size_to_allocate);
	  best_fit->size = offsetandsize - sizeof(heap_list_node);
	  void* newpayload = &(best_fit->payload);
	  newpayload += 0x8;
	  heap_list_node* next_node = (heap_list_node*)((newpayload) + best_fit->size);
	  insert_heap_list_node(best_fit);
	  insert_heap_list_node(next_node);
//	  remove_from_free_list(best_fit);
	  insert_free_node_front(next_node);
	  best_fit->payload = newpayload;
	  best_fit->size |= 1;
	  next_node->payload = ((&(next_node->payload)) + 0x8);
	  size_t subsize2 = size_to_allocate - best_fit->size;
	  next_node->size = subsize2 - sizeof(heap_list_node);
	  return best_fit->payload;
  }else{
	  if(print)
		  printf("best fit %p \t %d |\n",best_fit,offsetandsize);
	  void*payload = best_fit->payload;
	  size_t total_size = best_fit->size&~0xf;
	  heap_list_node* new_node = (heap_list_node*)(payload + (offsetandsize));
	  best_fit->size = offsetandsize;
	  new_node->size = (total_size - offsetandsize) - sizeof(heap_list_node);
	  best_fit->size |= 1;
	  remove_from_free_list(best_fit);
	  insert_free_node_front(new_node);
	  void* new = &(new_node->payload);
	  new_node->payload = new + 0x8;
	  return payload;
  }
  //return (void *)best_fit + NODE_SIZE;
  return NULL;
}


void mm_free(void *ptr)
{
	if(print)
		printf("free %p\n",ptr-sizeof(heap_list_node));
	heap_list_node* to_free = (heap_list_node*)(ptr - sizeof(heap_list_node));
	to_free->size &= ~0xf;
	insert_free_node_front(to_free);
}
