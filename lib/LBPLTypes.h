#ifndef LBPL_TYPES_H
#define LBPL_TYPES_H

#include <memory>
#include <string>
#include <variant>

class LBPLInstance;
class LBPLCallable;
class LBPLClass;

using LBPLType =
    std::variant<std::string, int, double, bool, char, std::nullptr_t,
                 std::shared_ptr<LBPLClass>, std::shared_ptr<LBPLInstance>,
                 std::shared_ptr<LBPLCallable>>;
#endif
