#ifndef CISH_TOKEN_ARRAY_C
#define CISH_TOKEN_ARRAY_C

#include "cish_constants.c"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define ELEM_TYPE enum token

#define ARRAY_DEBUG_MODE
#ifdef ARRAY_DEBUG_MODE
#define invariants(__array__)                                                  \
  {                                                                            \
    assert((__array__) != NULL);                                               \
    assert((__array__)->size >= 0);                                            \
    assert((__array__)->capacity >= 0);                                        \
    assert((__array__)->size <= (__array__)->capacity);                        \
  }
#else // Normal
#define invariants(__array__)
#endif

/* TYPES */
typedef struct array {
  size_t size;
  size_t capacity;
  ELEM_TYPE *buffer;
} Array;

/* PROTOTYPES */
Array token_array_new(size_t size);
void token_array_delete(Array *arr);

void token_array_push(Array *arr, ELEM_TYPE x);
void token_array_pop(Array *arr);

void token_array_map(Array *arr, ELEM_TYPE (*f)(ELEM_TYPE));
void token_array_print(const Array *arr);

/* FUNCTIONS */
Array token_array_new(size_t size) {
  Array arr = {.size = 0, .capacity = 8};
  arr.buffer = calloc(arr.capacity, sizeof(*arr.buffer));
  invariants(&arr);
  return arr;
}

void token_array_delete(Array *arr) {
  free(arr->buffer);
  arr->buffer = NULL;
}

void token_array_push(Array *arr, ELEM_TYPE x) {
  invariants(arr);

  if (arr->size == arr->capacity) {
    arr->capacity *= 2;
    arr->buffer = reallocarray(arr->buffer, arr->capacity, sizeof(*arr));
    if (arr->buffer == NULL) {
      perror("reallocarray: failed to realloc");
      abort();
    }
  }

  arr->buffer[arr->size++] = x;

  invariants(arr);
}

void token_array_pop(Array *arr) {
  invariants(arr);

  if (3 * arr->size <= arr->capacity) {
    arr->capacity /= 2;
    arr->buffer = reallocarray(arr->buffer, arr->capacity, sizeof(*arr));
    if (arr->buffer == NULL) {
      perror("reallocarray: failed to realloc");
      abort();
    }
  }

  --arr->size;

  invariants(arr);
}

void token_array_map(Array *arr, ELEM_TYPE (*f)(ELEM_TYPE)) {
  for (size_t i = 0; i < arr->size; i++) {
    arr->buffer[i] = f(arr->buffer[i]);
  }
}

void token_array_print(const Array *arr) {
  putc('[', stdout);
  for (size_t i = 0; i < arr->size; i++) {
    printf("%d, ", arr->buffer[i]);
  }
  puts("]");
}

#endif // !CISH_TOKEN_ARRAY_C
