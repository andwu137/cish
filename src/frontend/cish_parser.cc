#include "cish_lexer.h"
#include <string_view>

struct ParseState {
  std::string_view input_buf;
  size_t index;
  Position position;
};

ParseState init_parse_state(std::string_view input) {
  return {input, 0, {1, 1}};
}
