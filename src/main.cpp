#include "main.h"

int main(const int argc, const char **argv) {
  if (argc < 2) {
    std::cerr << "\033[1;31mNot enough arguemnts.\tUsage: lbpl [script]" << std::endl;
    return 1;
  }

  std::ifstream sourceFile(argv[1]);
  std::string filename(argv[1]);
  if (!sourceFile.is_open()) {
    std::cerr << "Error opening the file." << std::endl;
    return 1;
  }

  Parser *parser = new Parser(sourceFile, filename);
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
  sourceFile.close();
}
