#include <fstream>
#include <iostream>

#include "AST-generation/parser.hpp"
#include "interpretation/interpreter.hpp"
#include "interpretation/resolver.hpp"

int main(const int argc, const char **argv) {
  if (argc < 2) {
    std::cerr << "\033[1;31mNot enough arguemnts.\tUsage: lbpl [script]"
              << std::endl;
    return -1;
  }

  std::ifstream file(argv[1]);
  if (!file.good()) {
    std::cerr << "I/O error: couldn't load file `" << argv[1] << "`.";
    return -1;
  }

  Parser parser(file, argv[1]);
  std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

  if (!parser.hadError) {
    Interpreter interpreter;
    Resolver resolver(interpreter);
    resolver.resolve(statements);

    if (!resolver.hadError) {
      interpreter.interpret(statements);
      return 0;
    }
  }

  return -1;
}
