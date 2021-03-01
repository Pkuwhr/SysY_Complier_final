#include "op.h"
#include "tree.h"
#include "eeyore2mid.tab.hpp"
#include "utility.h"

extern FILE* yyin;
extern FILE* yyout;
extern TreeNode *gramma_root;
extern int yyparse();
vector<string> tiggercode;

int main(int argc, char **argv) {

	if (argc > 1) {
		if (!(yyin = fopen(argv[3], "r"))) {
			perror(argv[3]);
			return 1;
		}
		if (!(yyout = fopen(argv[5], "w"))) {
			perror(argv[5]);
			return 1;
		}
  	}

	yyparse();
	RegManager::InitReg();

	gramma_root->GenCode();

	cout<<endl<<endl<<"============================================"<<endl<<endl;
	vector<string>::iterator it;
	for(it=tiggercode.begin();it!=tiggercode.end();it++){
		fprintf(yyout,"%s",(*it).c_str());
	}
}
