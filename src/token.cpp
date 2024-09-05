#include "token.hpp"
#include <iomanip>

std::ostream &operator<<(std::ostream &os, const Token tk) {
  os << "Token { line: " << tk.line << ", column: " << tk.column
     << ", filename: " << tk.filename << ", type: " << type2str(tk.type);
  if (!std::holds_alternative<std::nullptr_t>(tk.lexeme)) {
    os << ", lexeme: ";
    std::visit(
        [&](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, int32_t> ||
                        std::is_same_v<T, float> || std::is_same_v<T, char>) {
            os << arg;
          } else if constexpr (std::is_same_v<T, const char *>) {
            os << std::quoted(arg);
          }
        },
        tk.lexeme);
  }

  os << " }";
  return os;
}
