objects=main.o Lexer.o pl0.o Scanner.o SymbolTable.o interpret.o
target=pl0

CXXFLAGS+=-o3 -std=c++11 -g 
CCC=g++
pl0: $(objects)
	g++ $(objects) -o $(target)

main.o: pl0.h Lexer.h Scanner.h
Lexer.o: Lexer.h
pl0.o: pl0.h
Scanner.o: Scanner.h
SymbolTable.o: SymbolTable.h
interpret.o:

.PHONY: clean
clean:
	rm $(objects)


