#include <stdio.h>

#include "cish_constants.c"
#include "cish_lexer.c"
#include "cish_parser.c"

void test_read(FILE *fptr) {
  size_t i = 0;
  size_t readSize;
  char buf[FILE_READ_BUF_SIZE];
  while ((readSize = fread(buf, 1, FILE_READ_BUF_SIZE - 1, fptr)) != 0) {
    buf[readSize] = 0;
    dbg("%lu: %s\n", ++i, buf);
  }
}

int main(int argc, char **argv) {
  ReturnType err = RETURN_SUCCESS;
  char *filename = "test/samples/simple.c";
  FILE *fptr = fopen(filename, "r");
  if (fptr == NULL) {
    fprintf(stderr, "Unable to read file '%s', file might not exist", filename);
    return FILE_OPEN_ERROR;
  }
  dbg("Opened file '%s'\n", filename);

  /*test_read(fptr);*/
  Token *tokens;
  size_t tokensLen;
  err = lex(fptr, &tokens, &tokensLen);
  fclose(fptr);
  if (err != RETURN_SUCCESS) {
    exit(err);
  }

  dbg("%s", "Finished lex\n");

  for (size_t i = 0; i < tokensLen; i++) {
    switch (tokens[i].type) {
    case TOKEN_TYPE_NAME: {
      dbg("%s\n", "TYPE_NAME");
    } break;
    case TOKEN_IDENTIFIER: {
      dbg("%s\n", "IDENTIFIER");
    } break;
    case TOKEN_CHAR: {
      dbg("%s\n", "CHAR");
    } break;
    case TOKEN_STRING: {
      dbg("%s\n", "STRING");
    } break;
    case TOKEN_INTEGER: {
      dbg("%s\n", "INTEGER");
    } break;
    case TOKEN_FLOAT: {
      dbg("%s\n", "FLOAT");
    } break;

    case TOKEN_ASSIGN: {
      dbg("%s\n", "ASSIGN");
    } break;

    case TOKEN_NOT: {
      dbg("%s\n", "NOT");
    } break;
    case TOKEN_EQUAL: {
      dbg("%s\n", "EQUAL");
    } break;
    case TOKEN_NOT_EQUAL: {
      dbg("%s\n", "NOT_EQUAL");
    } break;
    case TOKEN_LESS_THAN: {
      dbg("%s\n", "LESS_THAN");
    } break;
    case TOKEN_LESS_THAN_EQUAL: {
      dbg("%s\n", "LESS_THAN_EQUAL");
    } break;
    case TOKEN_GREATER_THAN: {
      dbg("%s\n", "GREATER_THAN");
    } break;
    case TOKEN_GREATER_THAN_EQUAL: {
      dbg("%s\n", "GREATER_THAN_EQUAL");
    } break;

    case TOKEN_SLASH: {
      dbg("%s\n", "SLASH");
    } break;
    case TOKEN_DASH: {
      dbg("%s\n", "DASH");
    } break;
    case TOKEN_PLUS: {
      dbg("%s\n", "PLUS\n");
    } break;

    case TOKEN_ASTERISK: {
      dbg("%s\n", "ASTERISK");
    } break;
    case TOKEN_AMPERSAND: {
      dbg("%s\n", "AMPERSAND");
    } break;

    case TOKEN_BIT_SHIFT_RIGHT: {
      dbg("%s\n", "BIT_SHIFT_RIGHT");
    } break;
    case TOKEN_BIT_SHIFT_LEFT: {
      dbg("%s\n", "BIT_SHIFT_LEFT");
    } break;

    case TOKEN_OPEN_PAREN: {
      dbg("%s\n", "OPEN_PAREN");
    } break;
    case TOKEN_CLOSE_PAREN: {
      dbg("%s\n", "CLOSE_PAREN");
    } break;
    case TOKEN_OPEN_BRACKET: {
      dbg("%s\n", "OPEN_BRACKET");
    } break;
    case TOKEN_CLOSE_BRACKET: {
      dbg("%s\n", "CLOSE_BRACKET");
    } break;
    case TOKEN_OPEN_BRACE: {
      dbg("%s\n", "OPEN_BRACE");
    } break;
    case TOKEN_CLOSE_BRACE: {
      dbg("%s\n", "CLOSE_BRACE");
    } break;

    case TOKEN_COMMA: {
      dbg("%s\n", "COMMA");
    } break;
    case TOKEN_SEMICOLON: {
      dbg("%s\n", "SEMICOLON");
    } break;
    }

    if (tokens[i].identifier != NULL) {
      // NOTE: temp for valgrind
      free(tokens[i].identifier);
    }
  }

  free(tokens);

  return 0;
}
