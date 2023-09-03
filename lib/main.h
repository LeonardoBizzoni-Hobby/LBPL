#ifndef MAIN_H
#define MAIN_H

#include "common.h"
#include "interpreter.h"
#include "parser.h"
#include "resolver.h"
#include "statements.h"

#include "ast_printer.h"

#include <fstream>
#include <iomanip>
#include <iostream>

std::string readFile(const char *);

#endif
