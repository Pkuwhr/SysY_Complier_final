all:	tigger2riscv

tigger2riscv:	bison flex main.cpp
	g++ -o $@ y.tab.c lex.yy.c main.cpp -g --std=c++14

bison:
	bison -d -y tigger2riscv.y
	bison -d -o tigger2riscv.tab.cpp tigger2riscv.y

flex:	bison
	flex tigger2riscv.l
	flex -o lex.yy.cpp tigger2riscv.l

clean:
	rm -rf y.tab.* lex.yy.* tigger2riscv tigger2riscv.tab.*
