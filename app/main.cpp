// Executables must have the following defined if the library contains
// doctest definitions. For builds with this disabled, e.g. code shipped to
// users, this can be left out.
#ifdef ENABLE_DOCTEST_IN_LIBRARY
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#endif

#include <iostream>
#include <stdlib.h>

#include "exampleConfig.h"
#include "lex.h"
#include "reader.h"
#include "type.h"
#include <tsl/htrie_set.h>

int main() {
  Lex lex = Lex("1.c");
  lex.parse();
  lex.print();
  return 0;
}
