#ifndef CISH_TOKEN_ARRAY_C
#define CISH_TOKEN_ARRAY_C

#include "cish_constants.c"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define ELEM_TYPE struct token

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
struct token_array {
  size_t size;
  size_t capacity;
  ELEM_TYPE *buffer;
};

/* PROTOTYPES */
struct token_array token_array_new(size_t cap);
void token_array_delete(struct token_array *arr);

void token_array_push(struct token_array *arr, ELEM_TYPE *x);
void token_array_pop(struct token_array *arr);

void token_array_map(struct token_array *arr, ELEM_TYPE (*f)(ELEM_TYPE));
void token_array_print(const struct token_array *arr);

/* FUNCTIONS */
struct token_array token_array_new(size_t cap) {
  struct token_array arr = {.size = 0, .capacity = cap};
  arr.buffer = calloc(arr.capacity, sizeof(*arr.buffer));
  invariants(&arr);
  return arr;
}

void token_array_delete(struct token_array *arr) {
  free(arr->buffer);
  arr->buffer = NULL;
}

void token_array_push(struct token_array *arr, ELEM_TYPE *x) {
  invariants(arr);

  if (arr->size + 1 >= arr->capacity) {
    arr->capacity *= 2;
    arr->buffer =
        reallocarray(arr->buffer, arr->capacity, sizeof(*arr->buffer));
    if (arr->buffer == NULL) {
      perror("reallocarray: failed to realloc");
      abort();
    }
  }

  arr->buffer[arr->size++] = *x;

  invariants(arr);
}

void token_array_pop(struct token_array *arr) {
  invariants(arr);

  if (3 * arr->size <= arr->capacity) {
    arr->capacity /= 2;
    arr->buffer =
        reallocarray(arr->buffer, arr->capacity, sizeof(*arr->buffer));
    if (arr->buffer == NULL) {
      perror("reallocarray: failed to realloc");
      abort();
    }
  }

  --arr->size;

  invariants(arr);
}

void token_array_map(struct token_array *arr, ELEM_TYPE (*f)(ELEM_TYPE)) {
  for (size_t i = 0; i < arr->size; i++) {
    arr->buffer[i] = f(arr->buffer[i]);
  }
}

void token_array_print(const struct token_array *arr) {
  putc('[', stdout);
  for (size_t i = 0; i < arr->size; i++) {
    printf("(%lu, %lu): %d ", arr->buffer[i].col, arr->buffer[i].row,
           arr->buffer[i].type);
    if (arr->buffer[i].string != NULL) {
      fwrite(arr->buffer[i].string, sizeof(char), arr->buffer[i].string_size,
             stdout);
    }
  }
  puts("]");
}

#undef ELEM_TYPE

#endif // !CISH_TOKEN_ARRAY_C
