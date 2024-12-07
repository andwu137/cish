#ifndef CISH_LEXER_C
#define CISH_LEXER_C

#include "cish_string.c"
#include "cish_token_array.c"
#include "cish_utils.c"
#include <ctype.h>
#include <string.h>

/* PROTOTYPES */
void lex(const char *filename);

int lex_many(int (*predicate)(int), long initSize, const char *buf,
             long fileSize, struct source_pos *sp, struct token *token);
int lex_try(int (*parser)(const char *buf, long fileSize, struct source_pos *sp,
                          struct token *token),
            const char *buf, long fileSize, struct source_pos *sp,
            struct token *token);

int lex_valid_ident_char(int c);

void lex_next_pos(char c, long *col, long *row);

int lex_ident(const char *buf, long fileSize, struct source_pos *sp,
              struct token *token);
int lex_float(const char *buf, long fileSize, struct source_pos *sp,
              struct token *token);
int lex_int(const char *buf, long fileSize, struct source_pos *sp,
            struct token *token);

void print_tokens(struct token_array *tokens);

/* FUNCTIONS */
void lex(const char *filename) {
  long fileSize = 0;
  char *fileBuf;
  if (!read_file(filename, &fileBuf, &fileSize)) {
    // TODO: handle error
    perror("lex: failed to read file");
    abort();
  }

  puts("<<FILE READ>>");
  fwrite(fileBuf, sizeof(*fileBuf), fileSize, stdout);
  puts("\n");

  struct token_array tokens = token_array_new(1 << 17);
  struct token bufTok;
  struct source_pos sp = {.pos = 0, .col = 0, .row = 1};

  while (sp.pos < fileSize) {
    lex_next_pos(fileBuf[sp.pos], &sp.col, &sp.row);
    bufTok.col = sp.col;
    bufTok.row = sp.row;
    bufTok.string = NULL;

    switch (fileBuf[sp.pos]) {
    case '[': {
      bufTok.type = TOKEN_OPEN_BRACKET;
      token_array_push(&tokens, &bufTok);
      sp.pos++;
    } break;
    case ']': {
      bufTok.type = TOKEN_CLOSE_BRACKET;
      token_array_push(&tokens, &bufTok);
      sp.pos++;
    } break;

    case '(': {
      bufTok.type = TOKEN_OPEN_PAREN;
      token_array_push(&tokens, &bufTok);
      sp.pos++;
    } break;
    case ')': {
      bufTok.type = TOKEN_CLOSE_PAREN;
      token_array_push(&tokens, &bufTok);
      sp.pos++;
    } break;

    case '{': {
      bufTok.type = TOKEN_OPEN_BRACE;
      token_array_push(&tokens, &bufTok);
      sp.pos++;
    } break;
    case '}': {
      bufTok.type = TOKEN_CLOSE_BRACE;
      token_array_push(&tokens, &bufTok);
      sp.pos++;
    } break;

    case '\'': {
      // char
      sp.pos++;
    } break;

    default: {
      if (lex_ident(fileBuf, fileSize, &sp, &bufTok)) {
        if (strncmp(bufTok.string, "fn", min(2, bufTok.string_size)) == 0) {
          free_null(bufTok.string);
          bufTok.type = TOKEN_FN;
        } else if (strncmp(bufTok.string, "df", min(2, bufTok.string_size)) ==
                   0) {
          free_null(bufTok.string);
          bufTok.string = NULL;
          bufTok.type = TOKEN_DF;
        }

        token_array_push(&tokens, &bufTok);
      } else if (lex_try(lex_float, fileBuf, fileSize, &sp, &bufTok) ||
                 lex_int(fileBuf, fileSize, &sp, &bufTok)) {
        token_array_push(&tokens, &bufTok);
      } else {
        sp.pos++;
      }
    } break;
    }
  }

  puts("<<TOKENIZE>>");
  print_tokens(&tokens);
  puts("\n");

  for (size_t i = 0; i < tokens.size; i++) {
    if (tokens.buffer[i].string != NULL) {
      free_null(tokens.buffer[i].string);
    }
  }

  token_array_delete(&tokens);
  free(fileBuf);
}

void print_tokens(struct token_array *tokens) {
  for (size_t i = 0; i < tokens->size; i++) {
    printf("(%lu, %lu): ", tokens->buffer[i].col, tokens->buffer[i].row);

    switch (tokens->buffer[i].type) {
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

    case TOKEN_OPEN_BRACKET: {
      printf("[");
    } break;
    case TOKEN_CLOSE_BRACKET: {
      printf("]");
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

    if (tokens->buffer[i].string != NULL) {
      fwrite(tokens->buffer[i].string, tokens->buffer[i].string_size,
             sizeof(char), stdout);
    }

    puts("");
  }
}

int lex_valid_ident_char(int c) {
  return isalnum(c) || c == '-' || c == '+' || c == '=' || c == '_' ||
         c == '!' || c == '@' || c == '#' || c == '$' || c == '%' || c == '%' ||
         c == '^' || c == '&' || c == '*' || c == '/' || c == '~';
}

void lex_next_pos(char c, long *col, long *row) {
  if (c == '\n') {
    *col = 0;
    ++*row;
  } else {
    ++*col;
  }
}

int lex_try(int (*parser)(const char *buf, long fileSize, struct source_pos *sp,
                          struct token *token),
            const char *buf, long fileSize, struct source_pos *sp,
            struct token *token) {
  long p = sp->pos;
  size_t c = sp->col;
  size_t r = sp->row;
  if (!parser(buf, fileSize, sp, token)) {
    if (token->string != NULL) {
      free_null(token->string);
    }

    sp->pos = p;
    sp->col = c;
    sp->row = r;
    return 0;
  }

  return 1;
}

int lex_many(int (*predicate)(int), long initSize, const char *buf,
             long fileSize, struct source_pos *sp, struct token *token) {
  struct string str = string_new(initSize > 0 ? 1 << 5 : initSize);

  for (; sp->pos < fileSize && predicate(buf[sp->pos]); sp->pos++) {
    lex_next_pos(buf[sp->pos], &sp->col, &sp->row);
    string_push(&str, buf[sp->pos]);
  }

  token->string_size = str.size;
  token->string = str.buffer;

  return 1;
}

int lex_ident(const char *buf, long fileSize, struct source_pos *sp,
              struct token *token) {
  // NOTE: inefficient, checks, then unchecks digit
  if (!lex_valid_ident_char(buf[sp->pos]) || isdigit(buf[sp->pos])) {
    return 0;
  }

  char ret = lex_many(lex_valid_ident_char, 32, buf, fileSize, sp, token);
  token->type = TOKEN_IDENT;
  return ret;
}

int lex_int(const char *buf, long fileSize, struct source_pos *sp,
            struct token *token) {
  if (!isdigit(buf[sp->pos])) {
    return 0;
  }

  char ret = lex_many(isdigit, 8, buf, fileSize, sp, token);
  token->type = TOKEN_INT;
  return ret;
}

int lex_float(const char *buf, long fileSize, struct source_pos *sp,
              struct token *token) {
  size_t tempSize = 0;
  char *tempStr = NULL;

  // Parse
  if (!lex_try(lex_int, buf, fileSize, sp, token)) {
    return 0;
  }
  tempStr = token->string;
  tempSize = token->string_size;

  if (buf[sp->pos] != '.') {
    free_null(token->string);
    return 0;
  }
  sp->pos++;

  if (!lex_try(lex_int, buf, fileSize, sp, token)) {
    free_null(token->string);
    return 0;
  }

  // Merge
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
