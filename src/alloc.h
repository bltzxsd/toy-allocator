#pragma once

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef char ALIGN[16];

union header {
    struct {
        size_t size;
        unsigned is_free;
        union header* next;
    } s;

    ALIGN stub;  // ensures header is always 16-byte aligned
};

typedef union header header_t;

extern header_t *head, *tail;
extern pthread_mutex_t global_malloc_lock;

void* vmalloc(size_t size);
void vfree(void* block);
void* vcalloc(size_t num, size_t nsize);
void* vrealloc(void* block, size_t size);

header_t* get_free_block(size_t size);
