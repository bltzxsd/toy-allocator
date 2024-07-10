#include "alloc.h"

#include <stdio.h>
#include <unistd.h>

header_t *head = NULL, *tail = NULL;
pthread_mutex_t global_malloc_lock;

header_t* get_free_block(size_t size) {
    header_t* curr = head;
    while (curr) {
        if (curr->s.is_free && curr->s.size >= size) return curr;
        curr = curr->s.next;
    }
    return NULL;
}

void* vmalloc(size_t size) {
    size_t total_size;
    void* block;
    header_t* header;

    if (!size) return NULL;

    pthread_mutex_lock(&global_malloc_lock);
    header = get_free_block(size);

    if (header) {
        header->s.is_free = 0;
        pthread_mutex_unlock(&global_malloc_lock);
        return (void*)(header + 1);
    }

    total_size = sizeof(header_t) + size;
    block = sbrk(total_size);

    if (block == (void*)(-1)) {
        pthread_mutex_unlock(&global_malloc_lock);
        return NULL;
    }

    header = (header_t*)block;
    header->s.size = size;
    header->s.is_free = 0;
    header->s.next = NULL;

    if (!head) head = header;
    if (tail) tail->s.next = header;

    tail = header;
    pthread_mutex_unlock(&global_malloc_lock);

    return (void*)(header + 1);
}

void vfree(void* block) {
    header_t *header, *tmp;
    void* programbreak;

    if (!block) return;
    pthread_mutex_lock(&global_malloc_lock);
    header = (header_t*)block - 1;

    programbreak = sbrk(0);

    char* current_block_end = (char*)block + header->s.size;

    if (current_block_end == programbreak) {
        if (head == tail) {
            head = tail = NULL;
        } else {
            tmp = head;
            while (tmp) {
                if (tmp->s.next == tail) {
                    tmp->s.next = NULL;
                    tail = tmp;
                }
                tmp = tmp->s.next;
            }
        }

        sbrk(0 - sizeof(header_t) - header->s.size);
        pthread_mutex_unlock(&global_malloc_lock);
        return;
    }
    header->s.is_free = 1;
    pthread_mutex_unlock(&global_malloc_lock);
}

void* vcalloc(size_t num, size_t nsize) {
    size_t size;
    void* block;

    if (!num || !nsize) return NULL;

    size = num * nsize;

    if (nsize != size / num) return NULL;

    block = vmalloc(size);
    if (!block) return NULL;

    memset(block, 0, size);
    return block;
}

void* vrealloc(void* block, size_t size) {
    header_t* header;
    void* ret;

    if (!block || !size) return vmalloc(size);

    header = (header_t*)block - 1;
    if (header->s.size >= size) return block;

    ret = vmalloc(size);
    if (ret) {
        memcpy(ret, block, header->s.size);
        vfree(block);
    }

    return ret;
}
