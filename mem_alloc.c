//Delaney Harwell
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define HEAP_SIZE 127
#define HEADER_SIZE 1

unsigned char heap[HEAP_SIZE];

//Set allocation status of a block in its header
void set_allocated(int header, bool allocated){
    if (allocated){
        heap[header] |= 1;
    }else{
        heap[header] &= ~1;
    }
}

//Initialize heap values to 0 and set first allocated bit to 0
void heap_initialization(){
    memset(heap, 0, HEAP_SIZE);
    heap[0] = HEAP_SIZE << 1;
    set_allocated(0, false);
}

// Get the allocation status of a block from its header
bool is_allocated(int header) {
    return (heap[header] & 1) == 1;
}

// Get the size of a block from its header
int get_block_size(int header) {
    return heap[header]  >> 1;
}

// Set the size of a block in its header
void set_block_size(int header, int size) {
    heap[header] = (size << 1)  | (heap[header] & 1);
}

//Find the best fit for the size of the word
int find_best_fit(int size) {
    int current = 0;
    int best_fit = -1;
    int best_fit_size = HEAP_SIZE+1;
    while (current < HEAP_SIZE) {
        int current_size = get_block_size(current);
        bool alloc = is_allocated(current);
        if (!alloc && current_size >= size && current_size < best_fit_size) {
            best_fit = current;
            best_fit_size = current_size;
        }

        current += current_size;
    }
    return best_fit;
}

// Perform splitting to create a new block of the appropriate size
void split_block(int block, int size) {
    int remaining_size = get_block_size(block) - size;// - HEADER_SIZE;

    set_block_size(block, size);
    set_allocated(block, true);

    int next_block = block + size;// + HEADER_SIZE;
    set_block_size(next_block, remaining_size);
    set_allocated(next_block, false);
}

// Coalesce freed block with the next block if it is free
void coalesce(int block) {
    int next_block = block + get_block_size(block);// + HEADER_SIZE;
    if (next_block < HEAP_SIZE && !is_allocated(next_block)) {
        set_block_size(block, get_block_size(block) + get_block_size(next_block));// + HEADER_SIZE);
    }
}

// Allocate a block of memory from the heap
int my_malloc(int size) {
    int best_fit_block = find_best_fit(size+1);

    if (best_fit_block == -1) {
        return -1; // Not enough space
    }
    if (get_block_size(best_fit_block) > size + HEADER_SIZE) {
        split_block(best_fit_block, size+1);
    } else {
        set_allocated(best_fit_block, true);
    }
    return best_fit_block + 1; // Return the pointer to the payload
}

// Free a block of memory
void my_free(int ptr) {
    int block = ptr - 1;

    if (!is_allocated(block)) {
        return; // Invalid pointer
    }

    set_allocated(block, false);
    coalesce(block);
}

// Resize a previously allocated block of memory
int my_realloc(int ptr, int size) {
    size+=1;
    int block = ptr - 1;
    if (!is_allocated(block)) {
        return -1; // Invalid pointer
    }
    int current_size = get_block_size(block);
    if (current_size == size) {
        return ptr; // No need to resize
    } else if (current_size > size) {
        split_block(block, size);
        int next_block = block + size;
        coalesce(next_block);
        return ptr;
    } else {
        int next_block = block + current_size;
        if (next_block < HEAP_SIZE && !is_allocated(next_block) && get_block_size(next_block) + current_size >= size) {
            coalesce(block);
            split_block(block, size);
            return ptr;
        } else {
            int new_ptr = my_malloc(size-1);
            if (new_ptr == -1) {
                return -1; // Not enough space
            }

            memcpy(heap + new_ptr, heap + ptr, current_size);
            my_free(ptr);
            return new_ptr;
        }
    }
}


// Print information about all blocks in the heap
void blocklist() {
    int current = 0;

    while (current < HEAP_SIZE) {
        int size = get_block_size(current);
        bool allocated = is_allocated(current);
        printf("%d, %d, %s\n", current + 1, size-1, allocated ? "allocated" : "free");

        current += size;
    }
}

// Write alpha-numeric characters into memory
void writemem(int ptr, char* data) {
    int current = ptr;
    int length = strlen(data);

    for (int i = 0; i < length; ++i) {
        heap[current + i] = data[i];
    }
}

// Print a segment of memory in hexadecimal
void printmem(int ptr, int num_addresses) {
    for (int i = 0; i < num_addresses; ++i) {
        printf("%02X ", heap[ptr + i]);
    }
    printf("\n");
}

//Main runner
int main() {
    heap_initialization();
    while (1) {
        printf(">");
        char command[20];
        scanf("%s", command);
        if (strcmp(command, "malloc") == 0) {
            int size;
            scanf("%d", &size);
            int ptr = my_malloc(size);
            printf("%d\n", ptr);
        } else if (strcmp(command, "realloc") == 0) {
            int ptr, size;
            scanf("%d %d", &ptr, &size);
            int new_ptr = my_realloc(ptr, size);
            printf("%d\n", new_ptr);
        } else if (strcmp(command, "free") == 0) {
            int ptr;
            scanf("%d", &ptr);
            my_free(ptr);
        } else if (strcmp(command, "blocklist") == 0) {
            blocklist();
        } else if (strcmp(command, "writemem") == 0) {
            int ptr;
            char data[20];
            scanf("%d %s", &ptr, data);
            writemem(ptr, data);
        } else if (strcmp(command, "printmem") == 0) {
            int ptr, num_addresses;
            scanf("%d %d", &ptr, &num_addresses);
            printmem(ptr, num_addresses);
        } else if (strcmp(command, "quit") == 0) {
            break;
        }
    }
    return 0;
}
