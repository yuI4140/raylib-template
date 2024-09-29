#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>

#define TRACE_FAILED ((void *)-7)
#define TRACE_INIT ((void *)7)

typedef struct Mtrace {
    size_t count;
    size_t capacity;
    uintptr_t *data;
} Mtrace;

void *trace_alloc(size_t size);
void *trace_realloc(void *ptr, size_t new_size);
void *trace_reallocarray(void *ptr, size_t nmemb, size_t size);
void trace_free(void *ptr);
int trace_spread(void);

#ifdef MTRACE_ERRMSG
#include <stdio.h>
#define mtrace_err(ptr)                                                           \
  do {                                                                            \
    if ((ptr) == NULL) {                                                          \
      fprintf(stderr, "%s:%u --> requested memory failure or NULL value\n", __FILE__, __LINE__); \
      exit(EXIT_FAILURE);                                                         \
    }                                                                             \
  } while (0)
#endif // MTRACE_ERRMSG
       

#ifdef MTRACE_IMP
static Mtrace *mt = NULL;
static int trace_init(void) {

    mt = (Mtrace *)malloc(sizeof(Mtrace));
    if (mt == NULL) {
        return -1;
    }
    mt->data = (uintptr_t *)malloc(sizeof(uintptr_t) * 10);
    if (mt->data == NULL) {
        free(mt);
        mt = NULL;
        return -1;
    }
    mt->count = 0;
    mt->capacity = 10;
    return 0;
}

static int trace_resize_data(void) {
    mt->capacity *= 2;
    uintptr_t *new_data = (uintptr_t *)realloc(mt->data, sizeof(uintptr_t) * mt->capacity);
    if (new_data == NULL) {
        return -1;
    }
    mt->data = new_data;
    return 0;
}

void *trace_alloc(size_t size) {
    if (mt == NULL) {
        if (trace_init() < 0) {
            return TRACE_FAILED;
        }
        return TRACE_INIT;
    }

    if (mt->count >= mt->capacity) {
        if (trace_resize_data() < 0) {
            return TRACE_FAILED;
        }
    }

    void *ptr = malloc(size);
    if (ptr == NULL) {
        return TRACE_FAILED;
    }
    mt->data[mt->count++] = (uintptr_t)ptr;
    return ptr;
}

static int trace_find_ptr(void *ptr, size_t *index) {
    for (size_t i = 0; i < mt->count; ++i) {
        if ((void *)mt->data[i] == ptr) {
            if (index) {
                *index = i;
            }
            return 0;
        }
    }
    return -1;
}

void *trace_realloc(void *ptr, size_t new_size) {
    if (ptr == NULL) {
        return TRACE_FAILED;
    }

    if (mt == NULL) {
        if (trace_init() < 0) {
            return TRACE_FAILED;
        }
        return TRACE_INIT;
    }

    size_t idx;
    if (trace_find_ptr(ptr, &idx) < 0) {
        return TRACE_FAILED;
    }

    void *new_ptr = realloc(ptr, new_size);
    if (new_ptr == NULL) {
        return TRACE_FAILED;
    }

    mt->data[idx] = (uintptr_t)new_ptr;
    return new_ptr;
}

void *trace_reallocarray(void *ptr, size_t nmemb, size_t size) {
    if (nmemb != 0 && size > SIZE_MAX / nmemb) {
        return TRACE_FAILED;
    }
    return trace_realloc(ptr, nmemb * size);
}

int trace_spread(void) {
    if (mt == NULL) {
        return -1;
    }

    for (size_t i = 0; i < mt->count; ++i) {
        free((void *)mt->data[i]);
    }

    free(mt->data);
    free(mt);
    mt = NULL;
    return 0;
}
void trace_free(void *ptr) {
    if (mt == NULL || ptr == NULL) {
        return;
    }
    size_t idx = 0;
    int found = 0;

    for (size_t i = 0; i < mt->count; ++i) {
        if ((void *)mt->data[i] == ptr) {
            idx = i;
            found = 1;
            break;
        }
    }
    if (!found) {
        return;
    }

    free(ptr);

    for (size_t i = idx; i < mt->count - 1; ++i) {
        mt->data[i] = mt->data[i + 1];
    }
    --mt->count;

    // Optionally, shrink the capacity of the tracking array if needed
    if (mt->count < mt->capacity / 2 && mt->capacity > 10) {
        mt->capacity /= 2;
        uintptr_t *new_data = (uintptr_t *)realloc(mt->data, sizeof(uintptr_t) * mt->capacity);
        if (new_data != NULL) {
            mt->data = new_data;
        }
    }
}
#endif // MTRACE_IMP
