#ifndef CISH_LEXER_C
#define CISH_LEXER_C

#include "cish_string.c"
#include "cish_token_array.c"
#include "cish_utils.c"
#include <ctype.h>
#include <string.h>

/* PROTOTYPES */
void lex(const char *filename);

int many(int (*predicate)(int), const char *buf, long fileSize, long *pos,
         size_t *col, size_t *row, struct token *token);
int try(int (*parser)(const char *buf, long fileSize, long *pos, size_t *col,
                      size_t *row, struct token *token),
        const char *buf, long fileSize, long *pos, size_t *col, size_t *row,
        struct token *token);

int valid_ident_char(int c);

void next_pos(char c, size_t *col, size_t *row);

int ident_(const char *buf, long fileSize, long *pos, size_t *col, size_t *row,
           struct token *token);
int float_(const char *buf, long fileSize, long *pos, size_t *col, size_t *row,
           struct token *token);
int int_(const char *buf, long fileSize, long *pos, size_t *col, size_t *row,
         struct token *token);

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
  size_t col = 0;
  size_t row = 1;

  for (long i = 0; i < fileSize;) {
    next_pos(fileBuf[i], &col, &row);
    bufTok.col = col;
    bufTok.row = row;
    bufTok.string = NULL;

    switch (fileBuf[i]) {
    case '(': {
      bufTok.type = TOKEN_OPEN_PAREN;
      token_array_push(&tokens, &bufTok);
      i++;
    } break;
    case ')': {
      bufTok.type = TOKEN_CLOSE_PAREN;
      token_array_push(&tokens, &bufTok);
      i++;
    } break;

    case '{': {
      bufTok.type = TOKEN_OPEN_BRACE;
      token_array_push(&tokens, &bufTok);
      i++;
    } break;
    case '}': {
      bufTok.type = TOKEN_CLOSE_BRACE;
      token_array_push(&tokens, &bufTok);
      i++;
    } break;

    case '\'': {
      // char
      i++;
    } break;

    default: {
      if (ident_(fileBuf, fileSize, &i, &col, &row, &bufTok)) {
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
      } else if (try(float_, fileBuf, fileSize, &i, &col, &row, &bufTok) ||
                 int_(fileBuf, fileSize, &i, &col, &row, &bufTok)) {
        token_array_push(&tokens, &bufTok);
      } else {
        i++;
      }
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

int valid_ident_char(int c) {
  return isalnum(c) || c == '-' || c == '+' || c == '=' || c == '_' ||
         c == '!' || c == '@' || c == '#' || c == '$' || c == '%' || c == '%' ||
         c == '^' || c == '&' || c == '*' || c == '/' || c == '~';
}

void next_pos(char c, size_t *col, size_t *row) {
  if (c == '\n') {
    *col = 0;
    ++*row;
  } else {
    ++*col;
  }
}

int try(int (*parser)(const char *buf, long fileSize, long *pos, size_t *col,
                      size_t *row, struct token *token),
        const char *buf, long fileSize, long *pos, size_t *col, size_t *row,
        struct token *token) {
  long p = *pos;
  size_t c = *col;
  size_t r = *row;
  if (!parser(buf, fileSize, pos, col, row, token)) {
    if (token->string != NULL) {
      free(token->string);
      token->string = NULL;
    }

    *pos = p;
    *col = c;
    *row = r;
    return 0;
  }

  return 1;
}

int many(int (*predicate)(int), const char *buf, long fileSize, long *pos,
         size_t *col, size_t *row, struct token *token) {
  struct string str = string_new(1 << 5);
  string_push(&str, buf[*pos]);

  while ((*pos + 1) < fileSize && predicate(buf[++*pos])) {
    next_pos(buf[*pos], col, row);
    string_push(&str, buf[*pos]);
  }

  token->string_size = str.size;
  token->string = str.buffer;

  return 1;
}

int ident_(const char *buf, long fileSize, long *pos, size_t *col, size_t *row,
           struct token *token) {
  // NOTE: inefficient, checks, then unchecks digit
  if (!valid_ident_char(buf[*pos]) || isdigit(buf[*pos])) {
    return 0;
  }

  char ret = many(valid_ident_char, buf, fileSize, pos, col, row, token);
  token->type = TOKEN_IDENT;
  return ret;
}

int int_(const char *buf, long fileSize, long *pos, size_t *col, size_t *row,
         struct token *token) {
  /*puts("here");*/
  if (!isdigit(buf[*pos])) {
    return 0;
  }

  char ret = many(isdigit, buf, fileSize, pos, col, row, token);
  token->type = TOKEN_INT;
  return ret;
}

int float_(const char *buf, long fileSize, long *pos, size_t *col, size_t *row,
           struct token *token) {
  size_t tempSize = 0;
  char *tempStr = NULL;
  if (!try(int_, buf, fileSize, pos, col, row, token)) {
    return 0;
  }
  tempStr = token->string;
  tempSize = token->string_size;

  if (buf[*pos] != '.') {
    return 0;
  }
  ++*pos;

  printf("int: %c\n", buf[*pos]);
  if (!try(int_, buf, fileSize, pos, col, row, token)) {
    return 0;
  }

  puts("here");
  tempStr =
      reallocarray(tempStr, tempSize + 1 + token->string_size, sizeof(char));
  if (tempStr == NULL) {
    perror("float_: failed to realloc string");
    abort();
  }

  tempStr[tempSize] = '.';
  for (long i = 0; i < token->string_size; i++) {
    tempStr[tempSize + 1 + i] = token->string[i];
  }

  token->string_size = tempSize + 1 + token->string_size;
  free(token->string);
  token->string = tempStr;

  token->type = TOKEN_FLOAT;

  return 1;
}

#endif // !CISH_LEXER_C
