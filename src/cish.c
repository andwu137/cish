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
      dbg("%s", "TYPE_NAME");
    } break;
    case TOKEN_IDENTIFIER: {
      dbg("%s", "IDENTIFIER");
    } break;

    case TOKEN_IF: {
      dbg("%s", "if");
    } break;
    case TOKEN_ELSE: {
      dbg("%s", "else");
    } break;
    case TOKEN_FOR: {
      dbg("%s", "for");
    } break;
    case TOKEN_RETURN: {
      dbg("%s", "return");
    } break;
    case TOKEN_WHILE: {
      dbg("%s", "while");
    } break;

    case TOKEN_CHAR: {
      dbg("%s", "CHAR");
    } break;
    case TOKEN_STRING: {
      dbg("%s", "STRING");
    } break;
    case TOKEN_INTEGER: {
      dbg("%s", "INTEGER");
    } break;
    case TOKEN_FLOAT: {
      dbg("%s", "FLOAT");
    } break;

    case TOKEN_EQUAL: {
      dbg("%s", "=");
    } break;

    case TOKEN_BANG: {
      dbg("%s", "!");
    } break;
    case TOKEN_EQUAL_EQUAL: {
      dbg("%s", "==");
    } break;
    case TOKEN_BANG_EQUAL: {
      dbg("%s", "!=");
    } break;
    case TOKEN_LESS_THAN: {
      dbg("%s", "<");
    } break;
    case TOKEN_LESS_THAN_EQUAL: {
      dbg("%s", "<=");
    } break;
    case TOKEN_GREATER_THAN: {
      dbg("%s", ">");
    } break;
    case TOKEN_GREATER_THAN_EQUAL: {
      dbg("%s", ">=");
    } break;

    case TOKEN_SLASH: {
      dbg("%s", "/");
    } break;
    case TOKEN_DASH: {
      dbg("%s", "-");
    } break;
    case TOKEN_PLUS: {
      dbg("%s", "+");
    } break;

    case TOKEN_ASTERISK: {
      dbg("%s", "*");
    } break;
    case TOKEN_AMPERSAND: {
      dbg("%s", "&");
    } break;

    case TOKEN_RIGHT_RIGHT: {
      dbg("%s", ">>");
    } break;
    case TOKEN_LEFT_LEFT: {
      dbg("%s", "<<");
    } break;

    case TOKEN_OPEN_PAREN: {
      dbg("%s", "(");
    } break;
    case TOKEN_CLOSE_PAREN: {
      dbg("%s", ")");
    } break;
    case TOKEN_OPEN_BRACKET: {
      dbg("%s", "[");
    } break;
    case TOKEN_CLOSE_BRACKET: {
      dbg("%s", "]");
    } break;
    case TOKEN_OPEN_BRACE: {
      dbg("%s", "{");
    } break;
    case TOKEN_CLOSE_BRACE: {
      dbg("%s", "}");
    } break;

    case TOKEN_COMMA: {
      dbg("%s", ",");
    } break;
    case TOKEN_SEMICOLON: {
      dbg("%s", ";");
    } break;
    }

    if (tokens[i].identifier != NULL) {
      // NOTE: temp for valgrind
      free(tokens[i].identifier);
    }
  }

  free(tokens);

  puts("");

  return 0;
}
