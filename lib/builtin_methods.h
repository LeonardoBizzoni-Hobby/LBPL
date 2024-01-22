#ifndef BUILTIN_METHODS_H
#define BUILTIN_METHODS_H

#include "LBPLCallable.h"
#include "statements.h"

#include <chrono>
#include <iostream>
#include <variant>

class LBPLPrintln : public LBPLCallable {
public:
  LBPLPrintln() {}

  int arity() override { return 1; };

  Value call(Interpreter *, std::vector<Value> &args) override {
    if (std::holds_alternative<std::string>(args[0])) {
      std::cout << std::get<std::string>(args[0]) << std::endl;
    } else if (std::holds_alternative<int>(args[0])) {
      std::cout << std::get<int>(args[0]) << std::endl;
    } else if (std::holds_alternative<double>(args[0])) {
      std::cout << std::get<double>(args[0]) << std::endl;
    } else if (std::holds_alternative<char>(args[0])) {
      std::cout << std::get<char>(args[0]) << std::endl;
    } else if (std::holds_alternative<bool>(args[0])) {
      std::cout << (std::get<bool>(args[0]) ? "true" : "false") << std::endl;
    }

    return nullptr;
  }
};

class LBPLClock : public LBPLCallable {
public:
  LBPLClock() {}

  int arity() override { return 0; };

  Value call(Interpreter *, std::vector<Value> &args) override {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
               .count() /
           1000.0;
  }
};

#endif
