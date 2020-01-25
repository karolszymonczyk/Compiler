FLAGS = -std=c++17 -Wall

all: kompilator

kompilator: parser.o lexer.o iofile.o compiler.o main.o
	g++ $(FLAGS) -o kompilator parser.o lexer.o iofile.o compiler.o main.o

main.o: main.cpp
	g++ $(FLAGS) -c main.cpp

iofile.o: iofile.cpp
	g++ $(FLAGS) -c iofile.cpp

compiler.o: compiler.cpp
	g++ $(FLAGS) -c compiler.cpp

lexer.o: lexer.c
	g++ $(FLAGS) -c lexer.c

parser.o: parser.tab.c
	g++ $(FLAGS) -c parser.tab.c -o parser.o

lexer.c: lexer.l
	flex -o lexer.c lexer.l

parser.tab.c: parser.y
	bison -d parser.y

clean: 
	rm -f *.o *~ *.c *.tab.c *.tab.h

cleanall: 
	rm -f kompilator

run:
	$(MAKE) all
	./kompilator code out
	$(MAKE) clean
