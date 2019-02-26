all: compiler

compiler: lex.yy.c y.tab.c 0516021.c 0516021_generate.c
	gcc -lm -o compiler lex.yy.c y.tab.c 0516021.c 0516021_generate.c -ly -lfl
y.tab.c: parser.y
	yacc -d -v parser.y
lex.yy.c: lex.l
	lex lex.l
clean:
	rm -f y.tab.c lex.yy.c y.tab.h y.output compiler
