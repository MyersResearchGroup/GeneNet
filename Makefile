
#we need to remove the runAllTests.cpp from the list, as this is only used for debuging
SRCS := $(subst runAllTests.cpp,, $(wildcard *.cpp))
OBJS := $(addsuffix .o, $(basename $(SRCS))) lex.o yacc.o


all: $(SRCS) $(OBJS)
	g++ -fpermissive -o GeneNet $(OBJS)

windows: $(SRCS) $(OBJS)
	g++ -fpermissive -o GeneNet $(OBJS)

%.o: %.cpp
	g++ -fpermissive -O3 -Wall -c -fmessage-length=0 -o$@ $<
lex.o: lex.l yacc.o
	flex lex.l
	g++ -fpermissive -c lex.yy.c -o lex.o
yacc.o: yacc.y
	bison -d yacc.y
	g++ -fpermissive yacc.tab.c -c -o yacc.o

.PHONY: clean

clean:
	rm -rf $(shell cat .cvsignore)
	rm -rf $(OBJS) $(GENENET_EXE) *~