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


typedef int block_header;
typedef int block_footer;

typedef struct chunk_header {
 struct chunk_header *next;
 struct chunker_header *prev;
 size_t size;
 size_t available_size;
} chunk_header;

static char* free_list;
static chunk_header* first_chunk;
/* $end mallocmacros */
void *current_avail = NULL;
int current_avail_size = 0;

void write_cause_mem_map_sux(chunk_header* first_chunk){
	void* firstPtr =0;void* secondPtr = 0; void*thirdPtr = 0; void* payload = 0;
	int i = 0;
	for(; i <100000; i++){}
	firstPtr = first_chunk;
	secondPtr = (firstPtr+DSIZE);
	//secondPtr -= 0x400;
	thirdPtr = secondPtr + WSIZE;
	payload = thirdPtr + WSIZE;

	PUT(secondPtr /*+ (1*WSIZE)*/,0);//the previous pointer
	int size = first_chunk->available_size;
	block_header first_header= PACK(size,0);
	PUT(thirdPtr,first_header);//block header
	free_list = (void*)(secondPtr) /*+WSIZE*/;
	return;
}

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{

  //printf("%ld\n",sizeof(chunk_header));
  //printf("%ld\n", sizeof(size_t));
  size_t allocate_size = CHUNK_ALIGN((2*PAGE_ALIGN(1)) + CHUNKOVERHEAD+BLOCKOVERHEAD);
  first_chunk = (chunk_header*)mem_map(allocate_size);
  void* firstPtr =0;void* secondPtr = 0; void*thirdPtr = 0; void* payload = 0;
  while(!first_chunk);
  int i = 0;
  first_chunk->next = NULL;
  first_chunk->prev = NULL;
  first_chunk->available_size = ALIGN(allocate_size - BLOCKOVERHEAD - CHUNKOVERHEAD);
  first_chunk->size = first_chunk->available_size;
  write_cause_mem_map_sux(first_chunk);
  return 0;
  //block_header header;
/*
  //PUT(first_chunk+CHUNKOVERHEAD,0);//alignment
  /*firstPtr = first_chunk;
  secondPtr = (first_chunk+DSIZE);
  secondPtr -= 0x400;
  thirdPtr = secondPtr + WSIZE;
  payload = thirdPtr + WSIZE;

//  PUT(secondPtr /*+ (1*WSIZE)*///,0);//the previous pointer
//  int size = first_chunk->available_size;
//  block_header first_header= PACK(size,0);
//  PUT(thirdPtr,first_header);//block header
//  free_list = (void*)(secondPtr) /*+WSIZE*/  ;

  //first_chunk = 0;
  //free_list = 0;
  //current_avail = mem_map;
  //current_avail_size = 0;
 // return 0;
}
void set_allocated(void*bp,size_t size){
	size_t extra_size = GET_SIZE(HDRP(bp))-size;
	if(extra_size > ALIGN(BLOCKOVERHEAD+ 1)){
		PUT(HDRP(bp),size);
		void*nextptr = NEXT_BLKP(bp);
		PUT(HDRP(nextptr),PACK(extra_size,0));
	}
	PUT(HDRP(bp),PACK(size,1));
}
void sub_extend(chunk_header* chunk_head){
    void* firstPtr = 0; void * secondPtr = 0; void*thirdPtr = 0; void* bp = 0;
    firstPtr = (void*) chunk_head;
    secondPtr = (firstPtr + WSIZE);
    thirdPtr = secondPtr + WSIZE;
    bp = thirdPtr + WSIZE;
    PUT(HDRP(bp),PACK(GET_SIZE(HDRP(bp)),0));
    //PUT(HDRP(NEXT_BLKP(bp)),PACK(GET_SIZE(HDRP(bp)),1));
}
void extend(size_t new_size){
	size_t chunk_size = CHUNK_ALIGN(new_size);
	size_t allocate_size = CHUNK_ALIGN((2*PAGE_ALIGN(new_size)) + CHUNKOVERHEAD+BLOCKOVERHEAD);
	//first_chunk = (chunk_header*)mem_map(allocate_size);
	chunk_header *new_chunk = mem_map(chunk_size);
	void* firstPtr =0;void* secondPtr = 0; void*thirdPtr = 0; void* payload = 0;
	while(!new_chunk);
	int i = 0;
	for(; i < 100000; i ++){}
	//find last chunk in linked list;
	chunk_header* chunk = first_chunk;
	while(chunk->next != NULL){
		chunk = chunk->next;
	}
	chunk->next = new_chunk;
	new_chunk->next = NULL;
	new_chunk->prev = chunk;
	new_chunk->available_size = ALIGN(allocate_size - BLOCKOVERHEAD - CHUNKOVERHEAD);
	new_chunk->size = first_chunk->available_size;
	firstPtr = new_chunk;
	secondPtr = (firstPtr+DSIZE);
	//secondPtr -= 0x400;
	thirdPtr = secondPtr + WSIZE;
	payload = thirdPtr + WSIZE;
	//write_cause_mem_map_sux(first_chunk);
	//while(!bp);
	//int i = 0;
	//for(; i < 100000;i++);
	sub_extend(payload);
//	PUT(HDRP(bp),PACK(GET_SIZE(HDRP(bp)),0));
//	PUT(HDRP(NEXT_BLKP(bp)),PACK(GET_SIZE(HDRP(bp)),1));
}
/* 
 * mm_malloc - Allocate a block by using bytes from current_avail,
 *     grabbing a new page if necessary.
 */
void *mm_malloc(size_t size)
{
  block_header newsize = ALIGN(size + BLOCKOVERHEAD);
  //search chunks for one with enough available size.
  //first fit strategy
  chunk_header* chunk = first_chunk;
  while(chunk != NULL){
	  if(chunk->available_size >= newsize){
		  break;
	  }
	  chunk = chunk->next;
  }
  if(chunk != NULL){
	  void* bp =/*(block_header*)*/FIRST_PAYLOAD(chunk);
	  void * hdrp = HDRP(bp);
      block_header bh_size = GET_SIZE(hdrp);
	  while(bh_size != 0){
		  int allocated = GET_ALLOC(HDRP(bp));
		  int bpsize = GET_SIZE(HDRP(bp));
		  if(!allocated && bpsize >= newsize){
			//int available = chunk->available_size;
			chunk->available_size = ALIGN(chunk->available_size - newsize);
			//int size_avail = chunk->available_size;
			//set_allocated(bp,newsize);
            int extra_space = (int)chunk->available_size;;
			//int extra_size = GET_SIZE(HDRP(bp))-newsize;
            if(extra_space > ALIGN(BLOCKOVERHEAD+ 1)){
            	int bhnew = ALIGN(newsize);
            	void* new_p = HDRP(bp);
				PUT(new_p,bhnew);
				void*nextptr = NEXT_BLKP(bp);
				int space = PACK(ALIGN(extra_space),0);
                PUT(HDRP(nextptr),space);
			}
			PUT(HDRP(bp),PACK(newsize,1));
			return bp;
		  }
		  bp =(block_header*) NEXT_BLKP(bp);
		  void* nextheader = HDRP(bp);
		  bh_size = GET_SIZE(nextheader);
	  }
	  extend(newsize);
	 // set_allocated(bp,newsize);
      block_header extra_size = GET_SIZE(HDRP(bp))-size;
      if(extra_size > ALIGN(BLOCKOVERHEAD+ 1)){
        block_header new_hdr = extra_size | 1;
        PUT(HDRP(bp),size);
        void*nextptr = NEXT_BLKP(bp);
        PUT(HDRP(nextptr),new_hdr);
      }
      PUT(HDRP(bp),PACK(size,1));
	  return bp;
  }
//  if(!first_chunk){
//	  void* payload = m_init(newsize);
//
//  }
//  if (current_avail_size < newsize) {
//    current_avail_size = PAGE_ALIGN(newsize);
//    current_avail = mem_map(current_avail_size);
//    if (current_avail == NULL)
//      return NULL;
//  }
//
//  p = current_avail;
//  current_avail += newsize;
//  current_avail_size -= newsize;
//
  return NULL;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
}
