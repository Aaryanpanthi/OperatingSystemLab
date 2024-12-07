#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "list.h"
#include "util.h"

void TOUPPER(char * arr){
  
    for(int i=0;i<strlen(arr);i++){
        arr[i] = toupper(arr[i]);
    }
}

void get_input(char *args[], int input[][2], int *n, int *size, int *policy) 
{
  	FILE *input_file = fopen(args[1], "r");
	  if (!input_file) {
		    fprintf(stderr, "Error: Invalid filepath\n");
		    fflush(stdout);
		    exit(0);
	  }

    parse_file(input_file, input, n, size);
  
    fclose(input_file);
  
    TOUPPER(args[2]);
  
    if((strcmp(args[2],"-F") == 0) || (strcmp(args[2],"-FIFO") == 0))
        *policy = 1;
    else if((strcmp(args[2],"-B") == 0) || (strcmp(args[2],"-BESTFIT") == 0))
        *policy = 2;
    else if((strcmp(args[2],"-W") == 0) || (strcmp(args[2],"-WORSTFIT") == 0))
        *policy = 3;
    else {
       printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
       exit(1);
    }
        
}

void allocate_memory(list_t * freelist, list_t * alloclist, int pid, int blocksize, int policy) {
  
    /* if policy == 1 -> FIFO
     *              2 -> BESTFIT 
     *              3 -> WORSTFIT
     * 
     * blocksize - size of the block to allocate_memory
     * pid - process the block belongs to
     * alloclist - list of allocated memory blocksize
     * freelist - list of free memory blocks
     * 
    * 1. Check if a node is in the FREE_LIST with a blk(end - start) >= blocksize
    * 2. if so, remove it and go to #3, if not print ""Error: Memory Allocation <blocksize> blocks\n""
    * 3. set the blk.pid = pid
    * 4. set the blk.end = blk->start + blocksize - 1
    * 5. add the blk to the ALLOC_LIST in ascending order by address.
    * 6. Deal with the remaining left over memory (fragment).
    *     a. dynamically allocate a new block_t called fragment [use malloc]
    *     b. set the fragment->pid = 0 
    *     c. set the fragment->start = the blk.end + 1
    *     d. set the fragment->end = original blk.end before you changed it in #4
    *     e. add the fragment to the FREE_LIST based on policy
    */


  
    // 1. Check if there is a block in freelist with enough space (end - start + 1 >= blocksize)
    node_t *prev = NULL;
    node_t *current = freelist->head;
    bool found = false;

    while (current != NULL) {
        int curr_blocksize = (current->blk->end - current->blk->start) + 1;
        if (curr_blocksize >= blocksize) {
            found = true;
            break;
        }
        prev = current;
        current = current->next;
    }

    if (!found) {
        // no suitable block found
        printf("Error: Memory Allocation %d blocks\n", blocksize);
        return;
    }

    // 2. Remove the chosen block from the freelist
    block_t *orig_block = current->blk;
    if (prev == NULL) {
        // removing head
        freelist->head = current->next;
    } else {
        prev->next = current->next;
    }
    free(current); // free the node, but keep orig_block

    // Keep track of the original end before we modify
    int original_end = orig_block->end;

    // 3. Set the pid of the chosen block
    orig_block->pid = pid;

    // 4. Adjust the end address to allocate exactly 'blocksize' memory
    orig_block->end = orig_block->start + blocksize - 1;

    // 5. Add this allocated block to the alloclist in ascending order by address
    list_add_ascending_by_address(alloclist, orig_block);

    // 6. Deal with leftover fragment, if any
    int allocated_end = orig_block->end;
    if (allocated_end < original_end) {
        // There is leftover space
        block_t *fragment = malloc(sizeof(block_t));
        fragment->pid = 0;
        fragment->start = allocated_end + 1;
        fragment->end = original_end;

        // Insert the fragment back into the free list based on policy
        if (policy == 1) { 
            // FIFO (First Fit)
            list_add_to_back(freelist, fragment);
        } else if (policy == 2) { 
            // BEST FIT
            list_add_ascending_by_blocksize(freelist, fragment);
        } else if (policy == 3) { 
            // WORST FIT
            list_add_descending_by_blocksize(freelist, fragment);
        }
    }
}

void deallocate_memory(list_t * alloclist, list_t * freelist, int pid, int policy) {

       /* if policy == 1 -> FIFO
     *              2 -> BESTFIT 
     *              3 -> WORSTFIT
     * 
     * pid - process id of the block to deallocate 
     * alloclist - list of allocated memory blocksize
     * freelist - list of free memory blocks
     * 
     * 
    * 1. Check if a node is in the ALLOC_LIST with a blk.pid = pid
    * 2. if so, remove it and go to #3, if not print "Error: Can't locate Memory Used by PID: <pid>"
    * 3. set the blk.pid back to 0
    * 4. add the blk back to the FREE_LIST based on policy.
    */

    // 1. Check if block with this pid exists in alloclist
    node_t *prev = NULL;
    node_t *current = alloclist->head;
    bool found = false;

    while (current != NULL) {
        if (current->blk->pid == pid) {
            found = true;
            break;
        }
        prev = current;
        current = current->next;
    }

    if (!found) {
        printf("Error: Can't locate Memory Used by PID: %d\n", pid);
        return;
    }

    // 2. Remove the block from alloclist
    block_t *blk = current->blk;
    if (prev == NULL) {
        alloclist->head = current->next;
    } else {
        prev->next = current->next;
    }
    free(current); // free the node, keep the block

    // 3. Set the blk.pid back to 0
    blk->pid = 0;

    // 4. Add this block back to the freelist based on policy
    if (policy == 1) { 
        // FIFO
        list_add_to_back(freelist, blk);
    } else if (policy == 2) { 
        // BEST FIT
        list_add_ascending_by_blocksize(freelist, blk);
    } else if (policy == 3) { 
        // WORST FIT
        list_add_descending_by_blocksize(freelist, blk);
    }
}



list_t* coalese_memory(list_t * list){
  list_t *temp_list = list_alloc();
  block_t *blk;
  
  while((blk = list_remove_from_front(list)) != NULL) {  // sort the list in ascending order by address
        list_add_ascending_by_address(temp_list, blk);
  }
  
  // try to combine physically adjacent blocks
  
  list_coalese_nodes(temp_list);
        
  return temp_list;
}

void print_list(list_t * list, char * message){
    node_t *current = list->head;
    block_t *blk;
    int i = 0;
  
    printf("%s:\n", message);
  
    while(current != NULL){
        blk = current->blk;
        printf("Block %d:\t START: %d\t END: %d", i, blk->start, blk->end);
      
        if(blk->pid != 0)
            printf("\t PID: %d\n", blk->pid);
        else  
            printf("\n");
      
        current = current->next;
        i += 1;
    }
}

/* DO NOT MODIFY */
int main(int argc, char *argv[]) 
{
   int PARTITION_SIZE, inputdata[200][2], N = 0, Memory_Mgt_Policy;
  
   list_t *FREE_LIST = list_alloc();   // list that holds all free blocks (PID is always zero)
   list_t *ALLOC_LIST = list_alloc();  // list that holds all allocated blocks
   int i;
  
   if(argc != 3) {
       printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
       exit(1);
   }
  
   get_input(argv, inputdata, &N, &PARTITION_SIZE, &Memory_Mgt_Policy);
  
   // Allocated the initial partition of size PARTITION_SIZE
   
   block_t * partition = malloc(sizeof(block_t));   // create the partition meta data
   partition->start = 0;
   partition->end = PARTITION_SIZE + partition->start - 1;
                                   
   list_add_to_front(FREE_LIST, partition);          // add partition to free list
                                   
   for(i = 0; i < N; i++) // loop through all the input data and simulate a memory management policy
   {
       printf("************************\n");
       if(inputdata[i][0] != -99999 && inputdata[i][0] > 0) {
             printf("ALLOCATE: %d FROM PID: %d\n", inputdata[i][1], inputdata[i][0]);
             allocate_memory(FREE_LIST, ALLOC_LIST, inputdata[i][0], inputdata[i][1], Memory_Mgt_Policy);
       }
       else if (inputdata[i][0] != -99999 && inputdata[i][0] < 0) {
             printf("DEALLOCATE MEM: PID %d\n", abs(inputdata[i][0]));
             deallocate_memory(ALLOC_LIST, FREE_LIST, abs(inputdata[i][0]), Memory_Mgt_Policy);
       }
       else {
             printf("COALESCE/COMPACT\n");
             FREE_LIST = coalese_memory(FREE_LIST);
       }   
     
       printf("************************\n");
       print_list(FREE_LIST, "Free Memory");
       print_list(ALLOC_LIST,"\nAllocated Memory");
       printf("\n\n");
   }
  
   list_free(FREE_LIST);
   list_free(ALLOC_LIST);
  
   return 0;
}