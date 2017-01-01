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

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

/* rounds up to the nearest multiple of mem_pagesize() */
#define PAGE_ALIGN(size) (((size) + (mem_pagesize()-1)) & ~(mem_pagesize()-1))
#define WSIZE       16
#define DSIZE       32
#define CHUNK_SIZE  (1<<14)
#define CHUNK_ALIGN(size) (((size)+(CHUNK_SIZE-1)) & ~(CHUNK_SIZE -1))
#define CHUNKOVERHEAD (sizeof(chunk_header))
#define BLOCKOVERHEAD (sizeof(block_header))
#define MAX(x, y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)       (*(size_t *)(p))
#define PUT(p, val)  (*(size_t *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0xF)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)  ((char *)(bp) - WSIZE)
#define FTRP(bp)  ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* Given free blockptr bp, compute address of the next and previous free blocks */
#define NEXT_FREEP(bp)  (*(void **)(bp + DSIZE))
#define PREV_FREEP(bp)  (*(void **)(bp))

/* Given chunkheader* cp, compute address of the first block payload of the chunk*/
#define FIRST_PAYLOAD(cp)  ((char*)(cp) + 4*WSIZE)


//typedef int block_header;
//typedef int block_footer;
typedef struct block_header{
	struct block_header *next;
	struct block_header *prev;
	size_t size; //
	void* payload;
}block_header;

/*
 * linked list of pointers and sizes to
 * all the chunks allocateds by mem_map
 */
typedef struct chunk_header {
 struct chunk_header *next;
 struct chunker_header *prev;
 //list of blocks in chunk
 void* filler;
 size_t available_size;
 //void* filler;
 block_header first_block;
}chunk_header;

chunk_header* first_chunk;
block_header* first_block_in_first_chunk;
static char* free_list;
static char* last_free_node;

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{

  //printf("%ld\n",sizeof(chunk_header));
  //printf("%ld\n", sizeof(size_t));
	printf("size of block header: %ld\n",sizeof(block_header));
	printf("size of chunk_header: %ld\n",sizeof(chunk_header));
  size_t allocate_size = CHUNK_ALIGN((2*PAGE_ALIGN(1)) + CHUNKOVERHEAD+BLOCKOVERHEAD);
  first_chunk = (chunk_header*)mem_map(allocate_size);
  first_chunk->available_size = allocate_size - BLOCKOVERHEAD - CHUNKOVERHEAD;
  first_chunk->next = NULL;
  first_chunk->prev = NULL;
  first_chunk->first_block.next = NULL;
  first_chunk->first_block.prev = NULL;
  first_chunk->first_block.size = first_chunk->available_size;
  void* bp = &(first_chunk->first_block.size);// + 0x10;
  bp += 0x10;
  first_chunk->first_block.payload = bp;
//  first_block_in_first_chunk = (void*)&(first_chunk->first_block);
//  first_block_in_first_chunk->payload = &(first_block_in_first_chunk) + 16;
//  first_block_in_first_chunk->next = NULL;
//  first_block_in_first_chunk->prev = NULL;
//  first_block_in_first_chunk->size = first_chunk->available_size;
  return 0;
}
char get_allocated(block_header b){
	return 0x1&(b.size);
}
void set_allocated(block_header b, char toAllocate){
	if(toAllocate){
		b.size |= 0x1;
	}else{
		b.size &= ~0x1;
	}
	//b.size |= 0x1;

}

/* 
 * mm_malloc - Allocate a block by using bytes from current_avail,
 *     grabbing a new page if necessary.
 */
void *mm_malloc(size_t size)
{
	int new_size = ALIGN(size + BLOCKOVERHEAD);
	chunk_header* chunk = first_chunk;
	while(chunk != NULL){
		if(chunk->available_size >= new_size){

			break;
		}
		chunk= chunk->next;
	}
	if(chunk != NULL){
		block_header* bh = &(chunk->first_block);
		while(bh != NULL){
			if((!(bh->size & 0x1)) && (bh->size >= new_size)){
				//void* hdrval = &(bh->payload)
				block_header* newp = (block_header*)((bh->payload) + bh->size);
				block_header new_header = (block_header) *newp;
				new_header.next = 0;
				new_header.prev = 0;
				new_header.payload = 0;
				new_header.size =0;
				bh->next = newp;
				bh->size = new_size;
				bh->size = bh->size | 0x1;
				new_header.prev = bh;
				new_header.next = NULL;
				new_header.size = chunk->available_size - BLOCKOVERHEAD - new_size;
				new_header.payload = &(newp->size);
				new_header.payload += 0x10;
				//set_allocated(*bh,1);
				new_header.size = new_header.size & ~0x1;
				//set_allocated(*new_header,0);
				*newp = new_header;
				chunk->available_size = ALIGN(chunk->available_size - new_size);
				return bh->payload;
				//int extra_space = (int)chunk->available_size;
			}
			bh = bh->next;
		}
		//extend
		//void* bp = chunk->
	}else{/*get another chunk*/}
	return 0;
}


/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
}
