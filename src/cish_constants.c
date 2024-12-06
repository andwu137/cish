#ifndef CISH_CONSTANTS_C
#define CISH_CONSTANTS_C

#include <stddef.h>

enum token_type {
  TOKEN_FN,
  TOKEN_DF,

  TOKEN_IDENT,
  TOKEN_INT,
  TOKEN_FLOAT,
  TOKEN_CHAR,
  /*TOKEN_STRING,*/
  /*TOKEN_LIST,*/

  TOKEN_OPEN_PAREN,
  TOKEN_CLOSE_PAREN,
  TOKEN_OPEN_BRACE,
  TOKEN_CLOSE_BRACE,
  TOKEN_OPEN_BRACKET,
  TOKEN_CLOSE_BRACKET,
};

struct token {
  enum token_type type;
  size_t col;
  size_t row;
  char *string;
  size_t string_size;
};

#endif // !CISH_CONSTANTS_C
