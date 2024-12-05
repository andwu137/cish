#ifndef CISH_LEXER_C
#define CISH_LEXER_C

#include "cish_string.c"
#include "cish_token_array.c"
#include "cish_utils.c"
#include <ctype.h>
#include <string.h>

/* PROTOTYPES */
void lex(const char *filename);
char ident(const char *buf, long fileSize, long *pos, struct token *token);
char valid_ident_char(char c);

/* FUNCTIONS */
void lex(const char *filename) {
  long fileSize = 0;
  char *fileBuf;
  if (!readFile(filename, &fileBuf, &fileSize)) {
    // TODO: handle error
    perror("lex: failed to read file");
    abort();
  }

  puts("<<FILE READ>>");
  fwrite(fileBuf, sizeof(*fileBuf), fileSize, stdout);
  puts("\n");

  struct token_array tokens = token_array_new(1 << 17);
  struct token bufTok;
  size_t col = 1;
  size_t row = 1;

  for (long i = 0; i < fileSize; i++) {
    if (fileBuf[i] == '\n') {
      col = 1;
      row++;
    } else {
      col++;
    }
    bufTok.col = col;
    bufTok.row = row;
    bufTok.string = NULL;

    switch (fileBuf[i]) {
    case '(': {
      bufTok.type = TOKEN_OPEN_PAREN;
      token_array_push(&tokens, &bufTok);
    } break;
    case ')': {
      bufTok.type = TOKEN_CLOSE_PAREN;
      token_array_push(&tokens, &bufTok);
    } break;

    case '{': {
      bufTok.type = TOKEN_OPEN_BRACE;
      token_array_push(&tokens, &bufTok);
    } break;
    case '}': {
      bufTok.type = TOKEN_CLOSE_BRACE;
      token_array_push(&tokens, &bufTok);
    } break;

    case '\'': {
      // char
    } break;

    default: {
      if (ident(fileBuf, fileSize, &i, &bufTok)) {
        if (strncmp(bufTok.string, "fn", min(2, bufTok.string_size)) == 0) {
          free(bufTok.string);
          bufTok.string = NULL;
          bufTok.type = TOKEN_FN;
        } else if (strncmp(bufTok.string, "df", min(2, bufTok.string_size)) ==
                   0) {
          free(bufTok.string);
          bufTok.string = NULL;
          bufTok.type = TOKEN_DF;
        }

        token_array_push(&tokens, &bufTok);
      }

      // int
      // float
      // char
    } break;
    }
  }

  puts("<<TOKENIZE>>");
  for (size_t i = 0; i < tokens.size; i++) {
    printf("(%lu, %lu): ", tokens.buffer[i].col, tokens.buffer[i].row);

    switch (tokens.buffer[i].type) {
    case TOKEN_FN: {
      printf("FN");
    } break;
    case TOKEN_DF: {
      printf("DF");
    } break;

    case TOKEN_OPEN_PAREN: {
      printf("(");
    } break;
    case TOKEN_CLOSE_PAREN: {
      printf(")");
    } break;

    case TOKEN_OPEN_BRACE: {
      printf("{");
    } break;
    case TOKEN_CLOSE_BRACE: {
      printf("}");
    } break;

    case TOKEN_IDENT: {
      printf("Ident: ");
    } break;

    case TOKEN_INT: {
      printf("Int: ");
    } break;
    case TOKEN_FLOAT: {
      printf("Float: ");
    } break;
    case TOKEN_CHAR: {
      printf("Char: ");
    } break;
    }

    if (tokens.buffer[i].string != NULL) {
      fwrite(tokens.buffer[i].string, tokens.buffer[i].string_size,
             sizeof(char), stdout);
      free(tokens.buffer[i].string);
    }

    puts("");
  }
  puts("\n");

  token_array_delete(&tokens);
  free(fileBuf);
}

char valid_ident_char(char c) {
  return isalnum(c) || c == '-' || c == '+' || c == '=' || c == '_' ||
         c == '!' || c == '@' || c == '#' || c == '$' || c == '%' || c == '%' ||
         c == '^' || c == '&' || c == '*' || c == '/' || c == '~';
}

char ident(const char *buf, long fileSize, long *pos, struct token *token) {
  if (!valid_ident_char(buf[*pos])) {
    return 0;
  }

  struct string str = string_new(1 << 5);
  string_push(&str, buf[*pos]);

  while ((*pos + 1) < fileSize && valid_ident_char(buf[++*pos])) {
    string_push(&str, buf[*pos]);
  }
  --*pos;

  token->type = TOKEN_IDENT;
  token->string_size = str.size;
  token->string = str.buffer;

  return 1;
}

#endif // !CISH_LEXER_C
