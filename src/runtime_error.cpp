#include "runtime_error.h"

std::string RuntimeError::what() {
  std::stringstream ss(msg);
  std::string lineInfo = "\033[1;31m[line " + std::to_string(line) + ":" +
                         std::to_string(column) + " in " + filename +
                         "]\033[0m: ";
  std::string line, res = lineInfo;
  std::string indentation(lineInfo.length() - 11, ' ');
  bool firstLine = true;

  while (std::getline(ss, line)) {
    if (firstLine) {
      res += line + "\n";
      firstLine = false;
    } else {
      res += indentation + line + "\n";
    }
  }

  return res;
}
