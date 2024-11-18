#ifndef CISH_UTILS_C
#define CISH_UTILS_C

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "cish_constants.c"

#define dbg(fmt, ...)                                                          \
  do {                                                                         \
    if (DEBUG)                                                                 \
      fprintf(stderr, fmt, __VA_ARGS__);                                       \
  } while (0)

void *calloc_or_die(size_t nmemb, size_t size) {
  void *x = calloc(nmemb, sizeof(Token));
  if (x == NULL) {
    fprintf(stderr, "Failed to allocate %lu tokens", nmemb);
    exit(ALLOCATION_ERROR);
  }
  return x;
}

#endif // !CISH_UTILS_C
