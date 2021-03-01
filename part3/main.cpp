
#include "tigger2riscv.tab.hpp"
#include <cstdio>

extern int yyparse();
extern FILE* yyin;
extern FILE* yyout;
/*
int main(int argc, char** argv)
{
	yyparse();
	return 0;
}
*/

int main(int argc, char **argv) {
  if (argc > 1) {
    if (!(yyin = fopen(argv[2], "r"))) {
      perror(argv[3]);
      return 1;
    }
    if (!(yyout = fopen(argv[4], "w"))) {
      perror(argv[5]);
      return 1;
    }
  }

  yyparse();

  return 0;
}