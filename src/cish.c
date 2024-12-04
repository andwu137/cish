#include "cish_constants.c"
#include "cish_lexer.c"
#include "cish_parser.c"

int main() {
  const char *filename = "test/samples/outline.csh";
  lex(filename);

  return 0;
}
