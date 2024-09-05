#include <fstream>
#include <iostream>

#include "parser.hpp"
#include "interpreter.hpp"
#include "resolver.hpp"

int main(const int argc, const char **argv) {
  if (argc < 2) {
    std::cerr << "\033[1;31mNot enough arguemnts.\tUsage: lbpl [script]"
              << std::endl;
    return 1;
  }

  std::ifstream file(argv[1]);
  if (!file.good()) {
    std::cerr << "I/O error: couldn't load file `" << argv[1] << "`.";
    return 1;
  }

  Parser *parser = new Parser(file, argv[1]);
  std::vector<std::unique_ptr<Stmt>> statements = parser->parse();

  // AST_Printer test;
  // for (auto &&stmt : statements) {
  //   stmt->accept(&test);
  // }

  if (!parser->hadError) {
    Interpreter *interpreter = new Interpreter();
    Resolver *resolver = new Resolver(interpreter);
    resolver->resolve(statements);

    if (!resolver->hadError) {
      interpreter->interpret(statements);
    }

    delete resolver;
    delete interpreter;
  }

  delete parser;
}
