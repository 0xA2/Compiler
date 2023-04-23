all: compiler

parser.tab.c parser.tab.h:	parser.y
	bison -t -v -d parser.y

lex.yy.c: lexer.x parser.tab.h
	flex lexer.x

compiler: main.c lex.yy.c parser.tab.c parser.tab.h ast.c symTable.c intermediateCode.c tmpTable.c toMips.c
	gcc -o compiler main.c parser.tab.c lex.yy.c ast.c symTable.c intermediateCode.c tmpTable.c toMips.c

clean:
	rm -f example parser.tab.c lex.yy.c parser.tab.h parser.output
