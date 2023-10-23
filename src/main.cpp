#include "main.h"
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(const int argc, const char **argv) {
  if (argc < 2) {
    std::cerr << "\033[1;31mNot enough arguemnts.\tUsage: lbpl [script]"
              << std::endl;
    return 1;
  }

  int fd = open(argv[1], O_RDONLY, S_IRUSR | S_IWUSR);
  struct stat sb;

  if (fstat(fd, &sb) == -1) {
    perror("Couldn't get file size.\n");
  }

  char *file_in_memory =
      (char *)mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

  Parser *parser = new Parser(file_in_memory, argv[1]);
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
  munmap(file_in_memory, sb.st_size);
  close(fd);
}
