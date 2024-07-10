#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "alloc.h"

void test_vmalloc() {
    // Allocate memory and check if the returned pointer is not NULL
    void* ptr = vmalloc(100);
    assert(ptr != NULL);
    fprintf(stderr, "vmalloc test passed!\n");
}

void test_vfree() {
    // Allocate memory and then free it
    void* ptr = vmalloc(100);
    assert(ptr != NULL);
    vfree(ptr);

    // There is no direct way to check if the memory is freed,
    // but we can ensure no crashes occur
    fprintf(stderr, "vfree test passed!\n");
}

void test_vcalloc() {
    // Allocate zero-initialized memory and check if the returned pointer is not NULL
    void* ptr = vcalloc(10, sizeof(int));
    assert(ptr != NULL);

    // Check if the memory is zero-initialized
    for (int i = 0; i < 10; i++) {
        assert(((int*)ptr)[i] == 0);
    }
    fprintf(stderr, "vcalloc test passed!\n");
}

void test_realloc() {
    // Allocate memory
    int* ptr = (int*)vmalloc(10 * sizeof(int));
    assert(ptr != NULL);

    // Initialize memory
    for (int i = 0; i < 10; i++) {
        ptr[i] = i;
    }

    // Reallocate memory
    ptr = (int*)vrealloc(ptr, 20 * sizeof(int));
    assert(ptr != NULL);

    // Check if the old values are still intact
    for (int i = 0; i < 10; i++) {
        assert(ptr[i] == i);
    }
    fprintf(stderr, "realloc test passed!\n");
}

int main() {
    test_vmalloc();
    test_vfree();
    test_vcalloc();
    test_realloc();

    return 0;
}
