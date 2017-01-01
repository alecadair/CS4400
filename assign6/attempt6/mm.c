#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"
/* single word (4) or double word (8) alignment */
#define ALIGNMENT 16

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0xf)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/*Additional Macros defined*/
#define HSIZE 8 //half word
#define WSIZE 16                                                                             //Size of a word
#define DSIZE 32                                                                            //Size of a double word
#define CHUNKSIZE 1<<14                                                                        //Initial heap size
#define OVERHEAD (sizeof(block_header))                                                                         //The minimum block size
#define MAX(x ,y)  ((x) > (y) ? (x) : (y))                                                  //Finds the maximum of two numbers
#define PACK(size, alloc)  ((size) | (alloc))                                               //Put the size and allocated byte into one word
#define GET(p)  (*(size_t *)(p))                                                            //Read the word at address p
#define PUT(p, value)  (*(int *)(p) = (value))                                           //Write the word at address p
#define GET_SIZE(p)  (GET(p) & ~0xf)                                                        //Get the size from header/footer
#define GET_ALLOC(p)  (GET(p) & 0x1)                                                        //Get the allocated bit from header/footer
#define HDRP(bp)  ((void *)(bp) - sizeof(block_header))                                                    //Get the address of the header of a block
#define FTRP(bp)  ((void *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)                               //Get the address of the footer of a block
#define NEXT_BLKP(bp)  ((void *)(bp) + GET_SIZE(HDRP(bp)))                                  //Get the address of the next block
#define PREV_BLKP(bp)  ((void *)(bp) - GET_SIZE(HDRP(bp) - WSIZE))                          //Get the address of the previous block
#define NEXT_FREEP(bp)  (*(void **)(bp + DSIZE))                                            //Get the address of the next free block
#define PREV_FREEP(bp)  (*(void **)(bp - WSIZE))                                                    //Get the address of the previous free block
#define PAGE_ALIGN(size) (((size) + (mem_pagesize() -1)) & ~(mem_pagesize() -1))

typedef int block_header;
typedef int block_footer;

typedef struct chunk_node{
	struct chunk_list* next;
	struct chunk_list* prev;
	//char* free_list;
}chunk_node;

static chunk_node* chunk_list = 0;
//static char *heap_listp = 0;                                                                //Pointer to the first block
static char *free_listp = 0;                                                                //Pointer to the first free block

//Function prototypes for helper routines
static void *extend_heap(size_t words);
static void place(void *bp, size_t size);
static void *find_fit(size_t size);
static void *coalesce(void *bp);
static void insert_at_front(void *bp);
static void remove_block(void *bp);


void sub_mm_init(void* first_chunk){
	chunk_list = first_chunk;
	PUT(chunk_list,0);//next_chunk
	void* secptr = chunk_list;
	secptr += 8;
    PUT(secptr, 0);//previous chunk
    secptr+= WSIZE;
    PUT(secptr, 0);//previous_pointer(free_list)
    secptr += 8;
    PUT(secptr,0);//padding
    block_header first_header = PACK(PAGE_ALIGN(OVERHEAD) - DSIZE,0);
    void* new_add = chunk_list;
    new_add += WSIZE;
    new_add += 12;
    PUT(new_add,first_header);
    void* free = chunk_list;
    free += WSIZE;
    free_listp = (char*)(free);                                                        //Initialize the free list pointer

}
/**
 * @brief mm_init Initializes the malloc
 * @return Return 0 if successful -1 if unsucessful
 */
int mm_init(void)
{
	printf("\n init \n");
	void* first_chunk = mem_map(PAGE_ALIGN(OVERHEAD));
	sub_mm_init(first_chunk);

    return 0;
}

/**
 * @brief mm_malloc Allocates a block with atleast the specified size of payload
 * @param size The payload size
 * @return The pointer to the start of the allocated block
 */
void *mm_malloc(size_t size)
{
    size_t adjustedsize;                                                                    //The size of the adjusted block
    size_t extendedsize;                                                                    //The amount by which heap is extended if no fit is found
    char *bp;                                                                               //Stores the block pointer

    adjustedsize = MAX(ALIGN(size) + WSIZE, OVERHEAD);                                      //Adjust block size to include overhead and alignment requirements

    if((bp = find_fit(adjustedsize))){                                                      //Traverse the free list for the first fit
        place(bp, adjustedsize);                                                            //Place the block in the free list
        return bp;
    }

    extendedsize = MAX(adjustedsize, CHUNKSIZE);                                            //If no fit is found get more memory to extend the heap

    if((bp = extend_heap(extendedsize / WSIZE)) == NULL){                                   //If unable to extend heap space
        return NULL;                                                                        //return null
    }

    place(bp, adjustedsize);                                                                //Place the block in the newly extended space
    return bp;
}

/**
 * @brief mm_free Frees a block
 * @param bp The block to be freed
 */
void mm_free(void *bp)
{
    if(!bp){                                                                                //If block pointer is null
        return;                                                                             //return
    }

    size_t size = GET_SIZE(HDRP(bp));                                                       //Get the total block size

    PUT(HDRP(bp), PACK(size, 0));                                                           //Set the header as unallocated
    PUT(FTRP(bp), PACK(size, 0));                                                           //Set the footer as unallocated
    coalesce(bp);                                                                           //Coalesce and add the block to the free list
}

/**
 * @brief extend_heap Extends the heap with free block
 * @param words The size to extend the heap by
 * @return The block pointer to the frst block in the newly acquired heap space
 */
static void* extend_heap(size_t words){
    char *bp;
    size_t size;

  //  size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;                               //Allocate even number of  words to maintain alignment

    if(size < OVERHEAD){
        size = OVERHEAD;
    }
    bp = mem_map(PAGE_ALIGN(size));

    PUT(HDRP(bp), PACK(size, 0));                                                           //Put the free block header
    PUT(FTRP(bp), PACK(size, 0));                                                           //Put the free block footer
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));                                                   //Put the new epilogue header

    return coalesce(bp);                                                                    //Coalesce if the previous block was free and add the block to the free list
}

/**
 * @brief coalesce Combines the newly freed block with the adjacent free blocks if any
 * @param bp The block pointer to the newly freed block
 * @return The pointer to the coalesced block
 */
static void *coalesce(void *bp){
    size_t previous_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));// || PREV_BLKP(bp) == bp;          //Stores whether the previous block is allocated or not
    size_t next__alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));                                    //Stores whether the next block is allocated or not
    size_t size = GET_SIZE(HDRP(bp));                                                       //Stores the size of the block

    if(previous_alloc && !next__alloc){                                                     //Case 1: The block next to the current block is free
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));                                              //Add the size of the next block to the current block to make it a single block
        remove_block(NEXT_BLKP(bp));                                                        //Remove the next block
        PUT(HDRP(bp), PACK(size, 0));                                                       //Update the new block's header
        PUT(FTRP(bp), PACK(size, 0));                                                       //Update the new block's footer
    }

    else if(!previous_alloc && next__alloc){                                                //Case 2: The block previous to the current block is free
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));                                              //Add the size of the previous block to the current bloxk to make it a single block
        bp = PREV_BLKP(bp);                                                                 //Update the block pointer to the previous block
        remove_block(bp);                                                                   //Remove the previous block
        PUT(HDRP(bp), PACK(size, 0));                                                       //Update the new block's header
        PUT(FTRP(bp), PACK(size, 0));                                                       //Update the new block's footer
    }

    else if(!previous_alloc && !next__alloc){                                               //Case 3: The blocks to the either side of the current block are free
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));              //Add the size of previous and next blocks to the current block to make it single
        remove_block(PREV_BLKP(bp));                                                        //Remove the block previous to the current block
        remove_block(NEXT_BLKP(bp));                                                        //Remove the block next to the current block
        bp = PREV_BLKP(bp);                                                                 //Update the block pointer to the previous block
        PUT(HDRP(bp), PACK(size, 0));                                                       //Update the new block's header
        PUT(FTRP(bp), PACK(size, 0));                                                       //Update the new block's footer
    }
    insert_at_front(bp);                                                                    //Insert the block to the start of free list
    return bp;
}

/**
 * @brief insert_at_front Inserts a block at the front of the free list
 * @param bp The pointer of the block to be added at the front of the free list
 */
static void insert_at_front(void *bp){
    NEXT_FREEP(bp) = free_listp;                                                            //Sets the next pointer to the start of the free list
    PREV_FREEP(free_listp) = bp;                                                            //Sets the current's previous to the new block
    PREV_FREEP(bp) = NULL;                                                                  //Set the previosu free pointer to NULL
    free_listp = bp;                                                                        //Sets the start of the free list as the new block
}

/**
 * @brief remove_block Removes a block from the free list
 * @param bp The pointer to the block to be removed from the free list
 */
static void remove_block(void *bp){
    if(PREV_FREEP(bp)){                                                                     //If there is a previous block
        NEXT_FREEP(PREV_FREEP(bp)) = NEXT_FREEP(bp);                                        //Set the next pointer of the previous block to next block
    }

    else{                                                                                   //If there is no previous block
        free_listp = NEXT_FREEP(bp);                                                        //Set the free list to the next block
    }

    PREV_FREEP(NEXT_FREEP(bp)) = PREV_FREEP(bp);                                            //Set the previous block's pointer of the next block to the previous block
}

/**
 * @brief find_fit Finds a fit for the block of a given size
 * @param size The size of the block to be fit
 * @return The pointer to the block used for allocation
 */
static void *find_fit(size_t size){
    char* free_block = free_listp;
    void* bp = free_block + 16;
    while(free_block != 0){
    	void* hdr = HDRP(bp);

    	int subsize = GET_SIZE(hdr);
    	if(subsize >= size && (subsize & 0xf) == 0 ){
    		return bp;
    	}
    	free_block = *(free_block);
    	bp = free_block + 8;
    }
    return NULL;
}
//    for(bp = free_listp; GET_ALLOC(HDRP(bp)) == 0; bp = NEXT_FREEP(bp)){                    //Traverse the entire free list
//        if(size <= GET_SIZE(HDRP(bp))){                                                     //If size fits in the available free block
//            return bp;                                                                      //Return the block pointer
//        }
//    }

//    return NULL;                                                                            //If no fit is found return NULL
//}

/**
 * @brief place Place a block of specified size to start of free block
 * @param bp The block pointer to the free block
 * @param size The size of the block to be placed
 */
static void place(void *bp, size_t size){
    size_t totalsize = GET_SIZE(HDRP(bp));                                                  //Get the total size of thefree block

    if((totalsize - size) >= OVERHEAD){                                                     //If the difference between the total size and requested size is more than overhead, split the block
        PUT(HDRP(bp), PACK(size, 1));                                                       //Put the header of the allocated block
        PUT(FTRP(bp), PACK(size, 1));                                                       //Put the footer of the allocated block
        remove_block(bp);                                                                   //Remove the allocated block
        bp = NEXT_BLKP(bp);                                                                 //The block pointer of the free block created by the partition
        PUT(HDRP(bp), PACK(totalsize - size, 0));                                           //Put the header of the new unallocated block
        PUT(FTRP(bp), PACK(totalsize - size, 0));                                           //Put the footer of the new unallocated block
        coalesce(bp);                                                                       //Coalesce the new free block with the adjacent free blocks
    }

    else{                                                                                   //If the remaining space is not enough for a free block then donot split the block
        PUT(HDRP(bp), PACK(totalsize, 1));                                                  //Put the header of the block
        PUT(FTRP(bp), PACK(totalsize, 1));                                                  //Put the footer of the block
        remove_block(bp);                                                                   //Remove the allocated block
    }
}

