#include "memalloc.h"
#include <pthread.h>
#include <bits/pthreadtypes.h>
#include <unistd.h>
#include <cstdio>

#include <iostream>
#include <source_location>
#include <string>

/*
Calling sbrk(0) gives the current address of program break.
Calling sbrk(x) with a positive value increments brk by x bytes, as a result allocating memory.
Calling sbrk(-x) with a negative value decrements brk by x bytes, as a result releasing memory.
*/

typedef char ALIGN[16];

union header {
    struct {
        size_t size;
        unsigned is_free;
        union header *next;
    } s;
    ALIGN stub;
};
typedef union header header_t;

header_t *get_free_block(size_t size);

header_t *head, *tail;

pthread_mutex_t global_malloc_lock;

void *malloc1(size_t size, const std::source_location location) {

    std::cout << location.function_name() << " allocated " << size << " bytes.\n";   

    size_t total_size;
    void *block;
    header_t *header;

    // If invalid amount of memory is requested return nothing.
    if (!size)
    {
        return NULL;
    }
    pthread_mutex_lock(&global_malloc_lock);
    header = get_free_block(size);
    if (header)
    {
        header->s.is_free = 0;
        pthread_mutex_unlock(&global_malloc_lock);
		return (void*)(header + 1);
    }
    total_size = sizeof(header_t) + size;
    block = sbrk(total_size);
    if (block == (void*) -1)
    {
		pthread_mutex_unlock(&global_malloc_lock);
		return NULL;
	}

    header = (header_t *) block;
    header->s.size = size;
    header->s.is_free = 0;
    header->s.next = NULL;

    if (!head)
    {
		head = header;
    }
    if (tail)
    {
		tail->s.next = header;
    }

    tail = header;
	pthread_mutex_unlock(&global_malloc_lock);
    // header + 1 points to the first byte after the header
    // ie the first byte of actual memory.
	return (void*)(header + 1);

}


/*
    We begin at the head of the linked list then walk through
    and keep checking until we get a block that is free and
    has a large enough size for what we wish to allocate.

*/
header_t *get_free_block(size_t size)
{
    header_t *curr = head;
    while(curr)
    {

        if (curr->s.is_free && curr->s.size >= size)
        {
            return curr;
        }
        curr = curr->s.next;
    }
    return NULL;
}

void free1(void *block)
{
    header_t *header, *temp;
    void *programbreak;

    if (!block)
    {
        return;
    }

    pthread_mutex_lock(&global_malloc_lock);
	header = (header_t*)block - 1;

    // Calling sbrk(0) gives the current address of program break.
    programbreak = sbrk(0);

    if ((char*) block + header->s.size == programbreak)
    {
        if (head == tail)
        {
            head = tail = NULL;
        } else {
            temp = head;
            while (temp)
            {
                if (temp->s.next == tail)
                {
                    temp->s.next = NULL;
                    tail = temp;
                }
                temp = temp->s.next;
            }
        }
        sbrk(0 - sizeof(header_t) - header->s.size);
        pthread_mutex_unlock(&global_malloc_lock);
		return;
    }
    header->s.is_free = 1;
	pthread_mutex_unlock(&global_malloc_lock);
}


// void global_free()
// {
//     header_t *temp = head;
//     printf("Attemping to start global free.\n");
//     printf("Attemping to start global free%s.\n", head);
//     while (head)
//     {
//         printf("Attemping to start global free loop.\n");
//         if (!head->s.is_free)
//         {
//             void* block = (void*)(head + 1);
//             printf("Attemping to free.\n");
//             free(block);
//             head->s.is_free = 1;
//             if (head == tail) {
//                 head = NULL;
//             } else {
//                 head = head->s.next;
//             }
            
//         }
//     }
// }

void global_free(void)
{
    header_t *current = head;

    while (current != NULL)
    {
        header_t *next = current->s.next;

        if (!current->s.is_free)
        {
            free1((void *)(current + 1));
        }

        current = next;
    }
}