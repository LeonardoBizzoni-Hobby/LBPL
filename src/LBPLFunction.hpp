#ifndef LBPL_FUNCTION_H
#define LBPL_FUNCTION_H

#include "LBPLCallable.hpp"
#include "environment.hpp"
#include "statements.hpp"

#include <memory>

class LBPLFunc : public LBPLCallable {
private:
  FnStmt *stmt;
  std::shared_ptr<Environment> closureEnv;
  bool isInitializer;

public:
  LBPLFunc(FnStmt *stmt, std::shared_ptr<Environment> &closureEnv,
           bool isInitializer)
      : stmt(stmt), closureEnv(closureEnv), isInitializer(isInitializer) {}
  LBPLFunc(FnStmt *stmt, std::shared_ptr<Environment> &&closureEnv,
           bool isInitializer)
      : stmt(stmt), closureEnv(closureEnv), isInitializer(isInitializer) {}

  void bind(std::shared_ptr<LBPLInstance> &&instance);
  void bind(std::shared_ptr<LBPLInstance> &instance);

  int arity() override;
  Value call(Interpreter *, std::vector<Value> &) override;
};

#endif
