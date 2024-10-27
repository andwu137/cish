#include "cish_lexer.h"
#include "cish_syntax.h"
#include <cstdlib>
#include <string>
#include <vector>

Position init_position() { return {1, 1}; }
LexState init_lex_state(std::string_view input) {
  return {input, 0, init_position()};
}

Token *new_token(LexState *state, TokenType type, std::string lexeme) {
  Token *t = (Token *)std::calloc(1, sizeof(Token));
  t->type = type;
  t->lexeme = lexeme;
  t->position = state->position;
  return t;
}

bool eof(LexState *state) {
  return state->input.end() <= state->input.begin() + state->index ||
         current_state(state) == 0;
}

char current_state(LexState *state) { return state->input[state->index]; }

short eol(LexState *state) {
  if (current_state(state) == '\n') {
    return 1;
  }

  if (current_state(state) == '\r') {
    if (!eof(state) && current_state(state) == '\n') {
      return 2;
    }
    return 1;
  }

  return 0;
}

bool next_position(LexState *state) {
  if (eof(state)) {
    return false;
  }

  if (short chars = eol(state)) {
    state->position = {1, state->position.row + 1};
    state->index += chars;
    return true;
  }

  state->position = {state->position.col + 1, state->position.row};
  state->index += 1;
  return true;
}

void until_eol(LexState *state, std::string *out_buf) {
  do {
    if (eof(state) || eol(state)) {
      next_position(state);
      return;
    }

    out_buf->push_back(current_state(state));
  } while (next_position(state));
}

void lex(std::string_view input, std::vector<Token *> *tokens) {
  LexState state = init_lex_state(input);

  do {
    switch (current_state(&state)) {
    // pre compilation
    case '#': {
      tokens->push_back(new_token(&state, PRE_PROCESS, ""));
      next_position(&state);
      until_eol(&state, &(*tokens).back()->lexeme);
      break;
    }
    // syntax
    case '{':
      tokens->push_back(new_token(&state, OPEN_BRACE, ""));
      break;
    case '}':
      tokens->push_back(new_token(&state, CLOSE_BRACE, ""));
      break;
    case '(':
      tokens->push_back(new_token(&state, OPEN_PAREN, ""));
      break;
    case ')':
      tokens->push_back(new_token(&state, CLOSE_PAREN, ""));
      break;
    case ';':
      tokens->push_back(new_token(&state, SEMICOLON, ""));
      break;
    case ',':
      tokens->push_back(new_token(&state, COMMA, ""));
      break;
    // other
    default:
      // keywords & identifier
      // NOTE: Uangn: read identifier, and check if its reserved
      // whitespace is significant

      // operators
      // NOTE: Uangn: read operator, and check if its reserved
      // whitespace is significant

      // numbers
      return;
    }
  } while (next_position(&state));
}
