#we need to remove the runAllTests.cpp from the list, as this is not used
SRCS := $(subst runAllTests.cpp,, $(wildcard *.cpp))
OBJS := $(addsuffix .o, $(basename $(SRCS))) lex.o yacc.o

EXAMPLES_DIR = ../examples/GeneNet/
GENENET_EXE = ../bin/GeneNet
GENENET_TSD := $(wildcard $(EXAMPLES_DIR)*/work/*/run-1.tsd)
GENENET_EXAMPLES := $(addsuffix GeneNet_method.dot, $(dir $(GENENET_TSD)))
GENENET_CHECKED := $(addsuffix GeneNet_checked.dot, $(dir $(GENENET_TSD)))
GENENET_POST_FILTER := $(addsuffix GeneNet_PF_0_4_method.dot, $(dir $(GENENET_TSD))) $(addsuffix GeneNet_PF_0_51_method.dot, $(dir $(GENENET_TSD)))
GENENET_POST_FILTER_CHECKED := $(subst _method,_checked, $(GENENET_POST_FILTER))

all: $(GENENET_EXE)

Bioinformatics: $(GENENET_EXAMPLES) $(GENENET_CHECKED) $(GENENET_POST_FILTER) $(GENENET_POST_FILTER_CHECKED)

#GeneNet creation Tool Chain
$(GENENET_EXE): $(SRCS) $(OBJS)
	g++  -oGeneNet $(OBJS)
	mv GeneNet ../bin/
%/GeneNet_method.dot: %/run-1.tsd
	/bin/nice -n19 /usr/bin/time -o $(dir $<)time.txt $(GENENET_EXE) $(dir $<) > $(dir $<)run.log
	mv $*/method.dot $@
%/GeneNet_checked.dot: %/GeneNet_method.dot
	../bin/check_dot.pl $< $(subst work/,, $(subst /GeneNet_checked.dot,.dot, $@)) $@
#post filter the GeneNet files
%/GeneNet_PF_0_4_method.dot: %/GeneNet_method.dot
	../bin/apply_post_filter.pl 0.4 $< $(subst /work,, $*.dot) $@
%/GeneNet_PF_0_51_method.dot: %/GeneNet_method.dot
	../bin/apply_post_filter.pl 0.51 $< $(subst /work,, $*.dot) $@
%/GeneNet_PF_0_4_checked.dot: %/GeneNet_PF_0_4_method.dot
	../bin/check_dot.pl $< $(subst work/,, $(subst /GeneNet_PF_0_4_checked.dot,.dot, $@)) $@
%/GeneNet_PF_0_51_checked.dot: %/GeneNet_PF_0_51_method.dot
	../bin/check_dot.pl $< $(subst work/,, $(subst /GeneNet_PF_0_51_checked.dot,.dot, $@)) $@



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
	rm -rf $(OBJS) $(GENENET_EXE) *~ $(GENENET_EXAMPLES)


