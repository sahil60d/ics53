#include "icsmm.h"
#include "debug.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * The allocator MUST store the head of its free list in this variable. 
 * Doing so will make it accessible via the extern keyword.
 * This will allow ics_freelist_print to access the value from a different file.
 */
ics_free_header *freelist_head = NULL;
ics_free_header *oh = NULL;
int heap_init = 0;
ics_footer *prologue_ptr;

/*
 * This is your implementation of malloc. It acquires uninitialized memory from  
 * ics_inc_brk() that is 16-byte aligned, as needed.
 *
 * @param size The number of bytes requested to be allocated.
 *
 * @return If successful, the pointer to a valid region of memory of at least the
 * requested size is returned. Otherwise, NULL is returned and errno is set to 
 * ENOMEM - representing failure to allocate space for the request.
 * 
 * If size is 0, then NULL is returned and errno is set to EINVAL - representing
 * an invalid request.
 */
void *ics_malloc(size_t size) { 
    // initizalize custom allocator
    //ics_mem_init();

    // error check
    if (size == 0) {
        errno = EINVAL;
        return NULL;
    }
    if (size > (PAGE_SIZE * 6)) {
        errno = ENOMEM;
        return NULL;
    }

    int mult = my_ceil(size, 16);
    int padding = (mult*16)-size;                           //set padding amount
    if (padding >= 16) {
        padding = 0;
    }

    int block_size;                                         //set total block size
    if (size > 16 ) {
        block_size = (int)size + padding + 16;
    } else {
        block_size = 32;
    }

    /***** FIRST HEAP REQUEST *****/

    if (freelist_head == NULL) {
        unsigned int num_pages;
        if (!heap_init) {
            num_pages = my_ceil(block_size+16, PAGE_SIZE);
        } else {
            num_pages = my_ceil(block_size, PAGE_SIZE);
        }
        //printf("extend heap %d\n", num_pages);
        void* start_add = ics_inc_brk(num_pages); 
        if (errno == ENOMEM) { 
            errno = ENOMEM;
            return NULL;
        }

        if (freelist_head != NULL) {
            oh = freelist_head;
        }

        // set up prologue, epilogue, header, footer
        //prologue
        if (!heap_init) {
            ics_footer *pro_ptr = start_add;
            pro_ptr->block_size = 0;
            pro_ptr->fid = PROLOGUE_MAGIC;
            prologue_ptr = pro_ptr;
        }

        //epilogue
        ics_header *ep_ptr = start_add + ((num_pages*PAGE_SIZE)-8);
        ep_ptr->block_size = 0;
        ep_ptr->hid = EPILOGUE_MAGIC;
        ep_ptr->padding_amount = 0;
        //header
        if (!heap_init) {
            freelist_head = start_add + 8;
            freelist_head->header.block_size = (num_pages * PAGE_SIZE)-16;
        } else {
            freelist_head = start_add - 8 ;
            freelist_head->header.block_size = (num_pages * PAGE_SIZE);
        }

        freelist_head->header.padding_amount = 0;
        freelist_head->header.hid = HEADER_MAGIC;
        /*
        if (oh != NULL) {
            freelist_head->next = oh;
            oh->next = NULL;
            oh->prev = freelist_head;
        } else {
            printf("here\n");
            freelist_head->next = NULL;
        }
        */
        freelist_head->next = NULL;
        freelist_head->prev = NULL;
        //footer
        ics_footer *footer_ptr = start_add + ((num_pages*PAGE_SIZE)-16);
        footer_ptr->fid = FOOTER_MAGIC;
        footer_ptr->block_size = num_pages * PAGE_SIZE;

        heap_init = 1;
    }

    /***** ALLOCATE MEMORY *****/
    oh = NULL;
    ics_free_header *temp_head = freelist_head;
    ics_free_header *find_free = freelist_head;
    while (find_free->header.block_size < block_size) {       //loop through free list until large enough block or request more space
        if (find_free->next == NULL) {                     //request more space
            unsigned int num_pages = my_ceil(block_size, PAGE_SIZE);        //number of pages needed to satisfy
            //printf("extend heap after %d\n", num_pages);
            void* start_add = ics_inc_brk(num_pages);   
            if (errno == ENOMEM) {                      //error - no memory left
                errno = ENOMEM;
                return NULL;
            }
            
            if (freelist_head != NULL) {
                oh = freelist_head;
                //ics_header_print(oh);
            }

            freelist_head = NULL;

            //header
            freelist_head = start_add - 8;
            freelist_head->header.block_size = (num_pages * PAGE_SIZE);
            freelist_head->header.padding_amount = 0;
            freelist_head->header.hid = HEADER_MAGIC;
            if (oh != NULL) {
                freelist_head->next = oh;
                //oh->next = NULL;
                oh->prev = freelist_head;
            } else {
                freelist_head->next = find_free;
            }
            freelist_head->prev = NULL;

            //update old freelist_head
            //find_free->next = NULL;
            //find_free->prev = freelist_head;

            //footer
            ics_footer *footer_ptr = start_add + ((num_pages * PAGE_SIZE)-16);
            footer_ptr->fid = FOOTER_MAGIC;
            footer_ptr->block_size = num_pages * PAGE_SIZE;
            
            //epilogue
            ics_header *ep_ptr = start_add + ((num_pages * PAGE_SIZE)-8);
            ep_ptr->block_size = 0;
            ep_ptr->hid = HEADER_MAGIC;
            ep_ptr->padding_amount = 0;

            //temp_head = freelist_head;
            find_free = freelist_head;

            //ics_header_print(oh);
            //ics_header_print(freelist_head);

            break;

        }
        else {
            find_free = find_free->next;
        }
    }
    //printf("start\n");
    //ics_freelist_print();
    //printf("end\n");
    //splintering
    int pad = find_free->header.block_size - block_size;
    //printf("%d\n", block_size);
    //printf("%d\n", pad);
    if ((pad > 0) && (pad < 32)) {
        pad = my_ceil(pad, 16) * 16;
        block_size += pad;
        padding += pad;
        //printf("%d\n", block_size);
    }

    //satisfy malloc call
    //remove free block from list 
    oh = find_free;
    //printf("%p\n", temp_head);
    freelist_head = NULL;
    if (find_free->prev != NULL) {
        find_free->prev->next = find_free->next;
    }
    if (find_free->next != NULL) {
        find_free->next->prev = find_free->prev;
    }
    //find_free->next, find_free->prev = NULL;

    int old_block_size = find_free->header.block_size;
    int real_block_size = block_size;

    //create new header
    ics_header *header_ptr = (ics_header*)find_free;                    
    block_size |= 1UL << 0;
    header_ptr->block_size = block_size;
    header_ptr->hid = HEADER_MAGIC;
    header_ptr->padding_amount = padding;

    //create new footer 
    ics_footer *footer_ptr = (ics_footer*)find_free + (size+padding+8)/8;  
    footer_ptr->fid = FOOTER_MAGIC;
    footer_ptr->block_size = block_size;

    //create new free header
    if (pad == 0) {
        if (find_free->next == NULL && find_free->prev != NULL) { //end of list
            freelist_head = temp_head;
        }
        else {
            freelist_head = find_free->next;
            if (freelist_head != NULL) {
                freelist_head->prev = NULL;
            }   
        }
    } else {
        if (old_block_size - real_block_size < 32) {
            freelist_head = NULL;
        } else {
            footer_ptr += 1;
            freelist_head = (ics_free_header*)footer_ptr;              
            freelist_head->header.block_size = old_block_size - real_block_size;
            freelist_head->header.padding_amount = 0;
            freelist_head->header.hid = HEADER_MAGIC;
            if (temp_head == find_free) {
                freelist_head->next = find_free->next;
                if (find_free->next != NULL) {
                    find_free->next->prev = freelist_head;
                }
            } else {
                freelist_head->next = temp_head;
                if (temp_head != NULL) {
                    //printf("%p\n", temp_head);
                    temp_head->prev = freelist_head;
                    //ics_header_print(freelist_head);
                    }
            }
            freelist_head->prev = NULL;
        }

        //update free footer
        if (freelist_head != NULL) {
            //ics_footer *free_footer_ptr = brk - 16;
            char *temp_free_footer_ptr = (char*)freelist_head + (freelist_head->header.block_size-8);
            ics_footer *free_footer_ptr = (ics_footer*)temp_free_footer_ptr;
            free_footer_ptr->block_size = old_block_size - real_block_size;
            free_footer_ptr->fid = FOOTER_MAGIC;
        }
    }
    //return payload address
    void* r_val = (void*)find_free + 8;
    return r_val;
}

/*
 * Marks a dynamically allocated block as no longer in use and coalesces with 
 * adjacent free blocks (as specified by Homework Document). 
 * Adds the block to the appropriate bucket according to the block placement policy.
 *
 * @param ptr Address of dynamically allocated memory returned by the function
 * ics_malloc.
 * 
 * @return 0 upon success, -1 if error and set errno accordingly.
 */
int ics_free(void *ptr) { 
    if (ptr == NULL) {
        errno = EINVAL;
        return -1;
    }
    void* brk = ics_get_brk();                //break pointer
    ics_header *ep_ptr = brk - 8;               //epilogue  

    /***** CHECK IF VALID *****/
    int not_valid = 0;                  // valid pointer flag: 0 = valid, 1 = not valid

    if (!(ptr >= ((void*)prologue_ptr + 8) && ptr <= ((void*)ep_ptr - 8))) {      // ptr is in between the prologue and epilogue
        errno = EINVAL;
        return -1;
    }
    
    ics_header *header_ptr = ptr-8;               //header
    int block_size = header_ptr->block_size;
    block_size &= ~(1UL << 0);
    char *temp_footer_ptr = (char*)header_ptr + (block_size-8);
    ics_footer *footer_ptr = (ics_footer*)temp_footer_ptr;  //footer
    if (!((header_ptr->block_size & 1) && (footer_ptr->block_size & 1))) {      // allocated bit in header and footer are set
        not_valid = 1;
    } else if (((header_ptr->hid) != HEADER_MAGIC) || ((footer_ptr->fid) != FOOTER_MAGIC)) {      // hid and fid are properly set
        not_valid = 1;
    } else if (header_ptr->block_size != footer_ptr->block_size) {      // block size are not equal in header and footer
        not_valid = 1;
    } else if (ptr == NULL) {
        not_valid = 1;
    }

    if (not_valid == 1) {
         errno = EINVAL;
         return -1;
    }

    /***** FREE BLOCK *****/
    //update old header
    ics_free_header *old_free_head = freelist_head;
    if (freelist_head != NULL) {
        old_free_head->prev = ptr-8;
        freelist_head = NULL;
    }

    //create new free header
    freelist_head = ptr-8;
    freelist_head->header.padding_amount = 0;
    freelist_head->header.hid = HEADER_MAGIC;
    freelist_head->header.block_size = block_size;
    freelist_head->next = old_free_head;
    freelist_head->prev = NULL;

    //update footer
    footer_ptr->block_size = block_size;

    /***** COALESCE *****/
    //check adjacent footer 
    char *temp_next_header = (char*)freelist_head + freelist_head->header.block_size;
    ics_free_header *next_free_header = (ics_free_header*)temp_next_header;

    if (!(next_free_header->header.block_size & 1) && (next_free_header->header.block_size != 0)) {         //COALESCE
        printf("testing\n");
        ics_freelist_print();
        char *temp_footer = (char*)next_free_header + (next_free_header->header.block_size - 8);        //new freelist footer
        ics_footer *freelist_footer = (ics_footer*)temp_footer;
        
        //update block size
        int block_size = freelist_head->header.block_size + (next_free_header->header.block_size);
        freelist_head->header.block_size = block_size;
        freelist_footer->block_size = block_size;

        //update next and prev
        if (freelist_head->next == next_free_header) {
            freelist_head->next = next_free_header->next;//NULL;
        }
        if (next_free_header->prev != NULL) {
            next_free_header->prev->next = next_free_header->next;//NULL; //freelist_head;
        } 
        if (next_free_header->next != NULL) {
            next_free_header->next->prev = next_free_header->prev;
            //freelist_head->next = next_free_header->next;
            //freelist_head->next->prev = freelist_head;
        }
        if (next_free_header->prev == freelist_head) {
            next_free_header->prev = NULL;
        }


    }
    return 0;
}

/********************** EXTRA CREDIT ***************************************/

/*
 * Resizes the dynamically allocated memory, pointed to by ptr, to at least size 
 * bytes. See Homework Document for specific description.
 *
 * @param ptr Address of the previously allocated memory region.
 * @param size The minimum size to resize the allocated memory to.
 * @return If successful, the pointer to the block of allocated memory is
 * returned. Else, NULL is returned and errno is set appropriately.
 *
 * If there is no memory available ics_malloc will set errno to ENOMEM. 
 *
 * If ics_realloc is called with an invalid pointer, set errno to EINVAL. See ics_free
 * for more details.
 *
 * If ics_realloc is called with a valid pointer and a size of 0, the allocated     
 * block is free'd and return NULL.
 */
void *ics_realloc(void *ptr, size_t size) {
    return NULL;
}
