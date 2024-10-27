#include "../../src/frontend/cish_lexer.h"
#include <iostream>
#include <vector>

void print_tokens(std::vector<Token> *tokens) {
  std::cout << "start print tokens" << std::endl;
  for (Token t : *tokens) {
    std::cout << t.type << ";" << t.position.col << ":" << t.position.row
              << "\n"
              << t.lexeme << std::endl;
  }
  std::cout << "end print tokens\n" << std::endl;
}

void test_preprocess() {
  std::vector<Token> tokens;
  lex("#define my balls haha \n", &tokens);
  print_tokens(&tokens);
  tokens.clear();

  lex("#define my balls hehe", &tokens);
  print_tokens(&tokens);
  tokens.clear();

  lex("#define my balls lmao\r\n", &tokens);
  print_tokens(&tokens);
  tokens.clear();
}

void test_braces() {
  std::vector<Token> tokens;
  lex("{", &tokens);
  print_tokens(&tokens);
  tokens.clear();

  lex("}", &tokens);
  print_tokens(&tokens);
  tokens.clear();
}

int main() {
  test_preprocess();
  test_braces();
}
