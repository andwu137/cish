#include <string>
#include <vector>

// types
struct Position {
  size_t col;
  size_t row;
};

enum TokenType {
  // pre compilation
  PRE_PROCESS,
  HEADER_NAME,
  // syntax
  OPEN_BRACE,
  CLOSE_BRACE,
  OPEN_PAREN,
  CLOSE_PAREN,
  SEMICOLON,
  COMMA,
  // keywords
  IF,
  WHILE,
  RETURN,
  TYPE,
  // user storage
  INT,
  REAL,
  OPERATOR,
  IDENTIFIER,
  COMMENT,
};

struct Token {
  TokenType type;
  std::string lexeme;
  Position position;
};

struct LexState {
  std::string_view input;
  size_t index;
  Position position;
};

// functions
LexState init_lex_state(std::string_view input);
Position init_position();

Token *new_token(LexState *state, TokenType type, std::string lexeme);

bool eof(LexState *state);
short eol(LexState *state);

char current_state(LexState *state);
bool next_position(LexState *state);

void until_eol(LexState *state, std::string *out_buf);
void lex(std::string_view input, std::vector<Token *> *tokens);
