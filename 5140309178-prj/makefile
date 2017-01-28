CC=g++
LEX=flex
YACC=yacc


scc:lex.yy.o y.tab.o 
	$(CC) y.tab.c -g -ly -ll -o scc 2> error.err
lex.yy.o:lex.yy.c y.tab.h 
	$(CC) -c lex.yy.c 2> error.err
y.tab.o:y.tab.c include/header.h include/treeNode.h quadruple.h semanticAnalysis.h include/symbolTable.h intermediateCode.h optimization.h codeGeneration.h
	$(CC) -c y.tab.c 2> error.err
lex.yy.c:y.tab.h smallc.l include/header.h quadruple.h semanticAnalysis.h intermediateCode.h optimization.h codeGeneration.h 
	$(LEX) smallc.l 2> error.log
y.tab.c y.tab.h:smallc.y include/header.h include/treeNode.h include/symbolTable.h quadruple.h semanticAnalysis.h intermediateCode.h optimization.h codeGeneration.h 
	$(YACC) smallc.y -v -d 2> error.log
clean:
	rm -rf *.o parser *.tab.* lex.yy.c *.output error* scc MIPSCode.s InterCode

  
