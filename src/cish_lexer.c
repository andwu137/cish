#ifndef CISH_LEXER_C
#define CISH_LEXER_C

#include "cish_token_array.c"
#include "cish_utils.c"

/* PROTOTYPES */
void lex(const char *filename);

/* FUNCTIONS */
void lex(const char *filename) {
  long fileSize = 0;
  char *fileBuf;
  if (readFile(filename, &fileBuf, &fileSize)) {
    // TODO: handle error
    perror("lex: failed to read file");
    abort();
    return;
  }

  fwrite(fileBuf, sizeof(*fileBuf), fileSize, stdout);
  /*fileBuf[fileSize] = 0;*/
  /*printf("%s", fileBuf);*/

  /*Array tokens = token_array_new(1 << 10);*/

  free(fileBuf);
}

#endif // !CISH_LEXER_C
