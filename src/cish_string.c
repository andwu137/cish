#ifndef CISH_STRING_C
#define CISH_STRING_C

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define ELEM_TYPE char

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
struct string {
  size_t size;
  size_t capacity;
  ELEM_TYPE *buffer;
};

/* PROTOTYPES */
struct string string_new(size_t cap);
void string_delete(struct string *arr);

void string_push(struct string *arr, ELEM_TYPE x);
void string_pop(struct string *arr);

void string_map(struct string *arr, ELEM_TYPE (*f)(ELEM_TYPE));
void string_print(const struct string *arr);

/* FUNCTIONS */
struct string string_new(size_t cap) {
  struct string arr = {.size = 0, .capacity = cap};
  arr.buffer = calloc(arr.capacity, sizeof(*arr.buffer));
  invariants(&arr);
  return arr;
}

void string_delete(struct string *arr) {
  free(arr->buffer);
  arr->buffer = NULL;
}

void string_push(struct string *arr, ELEM_TYPE x) {
  invariants(arr);

  if (arr->size == arr->capacity) {
    arr->capacity *= 2;
    arr->buffer =
        reallocarray(arr->buffer, arr->capacity, sizeof(*arr->buffer));
    if (arr->buffer == NULL) {
      perror("reallocarray: failed to realloc");
      abort();
    }
  }

  arr->buffer[arr->size++] = x;

  invariants(arr);
}

void string_pop(struct string *arr) {
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

void string_map(struct string *arr, ELEM_TYPE (*f)(ELEM_TYPE)) {
  for (size_t i = 0; i < arr->size; i++) {
    arr->buffer[i] = f(arr->buffer[i]);
  }
}

void string_print(const struct string *arr) {
  putc('"', stdout);
  fwrite(arr->buffer, arr->size, sizeof(char), stdout);
  puts("\"");
}

#undef ELEM_TYPE

#endif // !CISH_STRING_C
