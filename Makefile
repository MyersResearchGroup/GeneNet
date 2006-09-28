#we need to remove the runAllTests.cpp from the list, as this is not used
SRCS := $(subst runAllTests.cpp, , $(wildcard *.cpp))
OBJS := $(addsuffix .o, $(basename $(SRCS))) lex.o yacc.o

EXAMPLES_DIR = ../examples/GeneNet/
GENENET_EXE = ../bin/GeneNet
GENENET_TSD := $(wildcard $(EXAMPLES_DIR)*/work/*/run-1.tsd)
GENENET_EXAMPLES := $(addsuffix method.dot, $(dir $(GENENET_TSD)))

all: $(GENENET_EXE)

Bioinformatics: $(GENENET_EXAMPLES)

%/method.dot: %/run-1.tsd
	/bin/nice -n19 /usr/bin/time -o $(dir $<)time.txt $(GENENET_EXE) $(dir $<) > $(dir $<)run.log

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


