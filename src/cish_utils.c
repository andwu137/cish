#ifndef CISH_UTILS_C
#define CISH_UTILS_C

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* PROTOTYPES */
long getFileSize(FILE *fptr);
int readFile(const char *filename, char **outBuf, long *outBufSize);

/* FUNCTIONS */
long getFileSize(FILE *fptr) {
  fseek(fptr, 0, SEEK_END);
  long size = ftell(fptr);
  fseek(fptr, 0, SEEK_SET);
  return size;
}

int readFile(const char *filename, char **outBuf, long *outBufSize) {
  FILE *fptr = fopen(filename, "r"); // NOTE: this is buffered
  if (fptr == NULL) {
    return -1;
  }

  *outBufSize = getFileSize(fptr);
  *outBuf = calloc(*outBufSize, sizeof(**outBuf));
  fread(*outBuf, *outBufSize, sizeof(**outBuf), fptr);

  fclose(fptr);
  return 0;
}

#endif // !CISH_UTILS_C
