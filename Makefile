#we need to remove the runAllTests.cpp from the list, as this is not used
SRCS := $(subst runAllTests.cpp, , $(wildcard *.cpp))
OBJS := $(addsuffix .o, $(basename $(SRCS))) lex.o yacc.o

REB2SAC = ../bin/reb2sac

GENENET_EXE = ../bin/GeneNet

all: $(GENENET_EXE)

#GeneNet creation Tool Chain
$(GENENET_EXE): $(SRCS) $(OBJS)
	g++  -oGeneNet $(OBJS)
	mv GeneNet ../bin/
%.o: %.cpp
	g++ -O3 -Wall -c -fmessage-length=0 -o$@ $<
lex.o: lex.l yacc.o
	flex lex.l
	g++ -c lex.yy.c -o lex.o
yacc.o: yacc.y
	bison -d yacc.y
	g++ yacc.tab.c -c -o yacc.o

.PHONY: clean

clean:
	rm -rf $(shell cat .cvsignore)
	rm -rf $(OBJS) $(GENENET_EXE) *~


