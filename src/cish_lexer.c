#ifndef CISH_LEXER_C
#define CISH_LEXER_C

#include <stdio.h>
#include <stdlib.h>

#include "cish_constants.c"
#include "cish_utils.c"

/* PROTOTYPES */
static inline void next_pos(Pos *pos, char c);
static inline Pos init_pos();
static inline ReturnType unexpected_char(Pos pos, char c);
static inline ReturnType unexpected_eof(Pos pos);
static inline Token *init_tokens(size_t nmemb);
static inline Token new_token(TokenType type, char *identifier);
static inline void push_back_token(Token **ts, size_t *length, size_t *pos,
                                   Token t);
static inline char peek_buf_stream(size_t readSize, size_t pos, char *buf);
ReturnType lex(FILE *fptr, Token **outTokens, size_t *tokensLen);

/* LEXER */
ReturnType lex(FILE *fptr, Token **outTokens, size_t *tokensEndPos) {
  size_t readSize = 0;
  char buf[FILE_READ_BUF_SIZE];
  Pos pos = init_pos();
  *tokensEndPos = 0;
  size_t tokensLen = 1 << 17;
  *outTokens = init_tokens(tokensLen);
  char *tempIdent;

  while (1) {
    readSize = fread(buf, 1, FILE_READ_BUF_SIZE - 1, fptr);
    if (readSize == 0) {
      break;
    }

    for (size_t i = 0; i < readSize; next_pos(&pos, buf[i++])) {
      switch (buf[i]) {
      case ',': {
        push_back_token(outTokens, &tokensLen, tokensEndPos,
                        new_token(TOKEN_COMMA, NULL));
      } break;

      case ';': {
        push_back_token(outTokens, &tokensLen, tokensEndPos,
                        new_token(TOKEN_SEMICOLON, NULL));
      } break;

      case '=': {
        switch (peek_buf_stream(readSize, i, buf)) {
        case '=': {
          next_pos(&pos, buf[i++]);
          push_back_token(outTokens, &tokensLen, tokensEndPos,
                          new_token(TOKEN_EQUAL_EQUAL, NULL));
        } break;
        default: {
          push_back_token(outTokens, &tokensLen, tokensEndPos,
                          new_token(TOKEN_EQUAL, NULL));
        } break;
        }
      } break;

      case '!': {
        switch (peek_buf_stream(readSize, i, buf)) {
        case '=': {
          next_pos(&pos, buf[++i]);
          push_back_token(outTokens, &tokensLen, tokensEndPos,
                          new_token(TOKEN_BANG_EQUAL, NULL));
        } break;
        default: {
          push_back_token(outTokens, &tokensLen, tokensEndPos,
                          new_token(TOKEN_BANG, NULL));
        } break;
        }
      } break;

      case '<': {
        switch (peek_buf_stream(readSize, i, buf)) {
        case '<': {
          next_pos(&pos, buf[i]++);
          push_back_token(outTokens, &tokensLen, tokensEndPos,
                          new_token(TOKEN_LEFT_LEFT, NULL));
        } break;
        default: {
          push_back_token(outTokens, &tokensLen, tokensEndPos,
                          new_token(TOKEN_LESS_THAN, NULL));
        } break;
        }
      } break;

      case '>': {
        switch (peek_buf_stream(readSize, i, buf)) {
        case '>': {
          next_pos(&pos, buf[i]++);
          push_back_token(outTokens, &tokensLen, tokensEndPos,
                          new_token(TOKEN_RIGHT_RIGHT, NULL));
        } break;
        default: {
          push_back_token(outTokens, &tokensLen, tokensEndPos,
                          new_token(TOKEN_GREATER_THAN, NULL));
        } break;
        }
      } break;

      case '/': {
        switch (peek_buf_stream(readSize, i, buf)) {
        case '/': {
          next_pos(&pos, buf[i++]);
          while (peek_buf_stream(readSize, i, buf) != '\n') {
            next_pos(&pos, buf[i++]);
          }
        } break;
        default: {
          push_back_token(outTokens, &tokensLen, tokensEndPos,
                          new_token(TOKEN_SLASH, NULL));
        } break;
        }
      } break;

      case '&': {
        push_back_token(outTokens, &tokensLen, tokensEndPos,
                        new_token(TOKEN_AMPERSAND, NULL));
      } break;

      case '*': {
        push_back_token(outTokens, &tokensLen, tokensEndPos,
                        new_token(TOKEN_ASTERISK, NULL));
      } break;

      case '-': {
        push_back_token(outTokens, &tokensLen, tokensEndPos,
                        new_token(TOKEN_DASH, NULL));
      } break;

      case '+': {
        push_back_token(outTokens, &tokensLen, tokensEndPos,
                        new_token(TOKEN_PLUS, NULL));
      } break;

      case '(': {
        push_back_token(outTokens, &tokensLen, tokensEndPos,
                        new_token(TOKEN_OPEN_PAREN, NULL));
      } break;

      case ')': {
        push_back_token(outTokens, &tokensLen, tokensEndPos,
                        new_token(TOKEN_CLOSE_PAREN, NULL));
      } break;

      case '[': {
        push_back_token(outTokens, &tokensLen, tokensEndPos,
                        new_token(TOKEN_OPEN_BRACKET, NULL));
      } break;

      case ']': {
        push_back_token(outTokens, &tokensLen, tokensEndPos,
                        new_token(TOKEN_CLOSE_BRACKET, NULL));
      } break;

      case '{': {
        push_back_token(outTokens, &tokensLen, tokensEndPos,
                        new_token(TOKEN_OPEN_BRACE, NULL));
      } break;

      case '}': {
        push_back_token(outTokens, &tokensLen, tokensEndPos,
                        new_token(TOKEN_CLOSE_BRACE, NULL));
      } break;

      case '\'': {
        if (peek_buf_stream(readSize, i, buf) == '\'') {
          return unexpected_char(pos, buf[i]);
        } else {
          tempIdent = calloc_or_die(2, sizeof(char));
          tempIdent[0] = buf[i];
          push_back_token(outTokens, &tokensLen, tokensEndPos,
                          new_token(TOKEN_CHAR, tempIdent));
        }
      } break;

      case '"': {
        if (peek_buf_stream(readSize, i, buf) == '\'') {
          return unexpected_char(pos, buf[i]);
        } else {
          // TODO: read the whole string in
          push_back_token(outTokens, &tokensLen, tokensEndPos,
                          new_token(TOKEN_STRING, NULL));
        }
      } break;

      default: {
        /*dbg("%s\n", "other");*/
        /*dbg("%s\n", "identifier");*/
        /*dbg("%s\n", "float");*/
        /*dbg("%s\n", "int");*/
        /*dbg("%s\n", "line comment");*/
        /*dbg("%s\n", "block comment");*/
      }; break;
      }
    }
  }

  return RETURN_SUCCESS;
}

/* HELPERS */
// WARN: this returns '\0' for eof
// If the input program has any, it could will break lex logic
static inline char peek_buf_stream(size_t readSize, size_t pos, char *buf) {
  if (readSize <= pos + 1) {
    return 0;
  } else {
    return buf[pos];
  }
}

static inline void next_pos(Pos *pos, char c) {
  if (c == '\n') {
    pos->col = 0;
    pos->row++;
  } else {
    pos->col++;
  }
}

static inline Pos init_pos() {
  Pos p = {1, 1};
  return p;
}

static inline ReturnType unexpected_char(Pos pos, char c) {
  if (c != '\n') {
    fprintf(stderr, "Unexpected '%c' at %lu, %lu", c, pos.col, pos.row);
  } else {
    fprintf(stderr, "Unexpected '\\n' at %lu, %lu", pos.col, pos.row);
  }
  return LEXER_UNEXPECTED_CHAR;
}

static inline ReturnType unexpected_eof(Pos pos) {
  fprintf(stderr, "Unexpected EOF at %lu, %lu", pos.col, pos.row);
  return LEXER_UNEXPECTED_CHAR;
}

static inline Token *init_tokens(size_t nmemb) {
  return (Token *)calloc_or_die(nmemb, sizeof(Token));
}

static inline Token new_token(TokenType type, char *identifier) {
  Token t = {type, identifier};
  return t;
}

static inline void push_back_token(Token **ts, size_t *length, size_t *pos,
                                   Token t) {
  if (*length <= *pos) {
    *length = (*length) << 1;
    Token *temp = (Token *)realloc(*ts, *length);
    if (temp == NULL) {
      fprintf(stderr, "Failed to allocate %lu tokens", *length);
      exit(ALLOCATION_ERROR);
    }
    *ts = temp;
  } else {
    (*ts)[*pos] = t;
  }

  (*pos)++;
}

#endif // !CISH_LEXER_C
