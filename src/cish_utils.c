#ifndef CISH_UTILS_C
#define CISH_UTILS_C

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

/* DEFINES */
#define max(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) __max_a__ = (a);                                             \
    __typeof__(b) __max_b__ = (b);                                             \
    __max_a__ > __max_b__ ? __max_a__ : __max_b__;                             \
  })

#define min(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) __min_a__ = (a);                                             \
    __typeof__(b) __min_b__ = (b);                                             \
    __min_a__ < __min_b__ ? __min_a__ : __min_b__;                             \
  })

#define free_null(x)                                                           \
  ({                                                                           \
    free(x);                                                                   \
    x = NULL;                                                                  \
  })

/* PROTOTYPES */
char get_file_size(const char *filename, long *size);
char read_file(const char *filename, char **outBuf, long *outBufSize);

/* FUNCTIONS */
char get_file_size(const char *filename, long *size) {
  struct stat st;
  if (stat(filename, &st)) {
    return 0;
  }

  *size = st.st_size;
  return 1;
}

char read_file(const char *filename, char **outBuf, long *outBufSize) {
  char retval = 0;

  if (!get_file_size(filename, outBufSize)) {
    goto EXIT;
  }
  *outBuf = calloc(*outBufSize, sizeof(**outBuf));

  FILE *fptr = fopen(filename, "r"); // NOTE: this is buffered
  if (fptr == NULL) {
    goto FILE_EXIT;
  }

  if (!fread(*outBuf, *outBufSize, sizeof(**outBuf), fptr)) {
    free_null(*outBuf);
    goto FILE_EXIT;
  }

  retval = 1;

FILE_EXIT:
  fclose(fptr);
EXIT:
  return retval;
}

#endif // !CISH_UTILS_C
