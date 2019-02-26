#makefile for parser
YACC = yacc
YACC_EX = y.tab.c
LEX = lex
LEX_EX = lex.yy.c
LEXFN = 0516021_lex.l
CC = gcc
LIBS = -ly -lfl
RM = rm -f
all: parser

parser: lex.yy.c y.tab.c
	$(CC) -o parser $(LEX_EX) $(YACC_EX) $(LIBS)
y.tab.c: parser.y
	$(YACC) -d -v parser.y
lex.yy.c: 0516021_lex.l
	$(LEX) $(LEXFN)
clean:
	$(RM) $(LEX_EX) $(YACC_EX) y.tab.h y.output parser
	

