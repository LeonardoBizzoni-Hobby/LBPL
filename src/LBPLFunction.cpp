#include "LBPLFunction.hpp"
#include "interpreter.hpp"

void LBPLFunc::bind(std::shared_ptr<LBPLInstance> &&instance) {
  closureEnv = std::make_shared<Environment>();
  closureEnv->define("this", instance);
}

void LBPLFunc::bind(std::shared_ptr<LBPLInstance> &instance) {
  closureEnv = std::make_shared<Environment>();
  closureEnv->define("this", instance);
}

int LBPLFunc::arity() { return stmt->args.size(); }

Value LBPLFunc::call(Interpreter *interpreter, std::vector<Value> &args) {
  auto env = std::make_shared<Environment>(closureEnv);

  for (int i = 0; i < stmt->args.size(); i++) {
    env->define(stmt->args[i]->lexeme, args[i]);
  }

  try {
    interpreter->executeBlock(stmt->body, env);
  } catch (ReturnException &ret) {
    return isInitializer ? closureEnv->getAt(0, "this") : ret.value;
  }

  return isInitializer ? closureEnv->getAt(0, "this") : nullptr;
}
