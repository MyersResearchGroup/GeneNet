#Makefile for tools

all: geneNet 

geneNet: FORCE
	make -C src
	mv src/GeneNet bin/GeneNet

clean: FORCE
	make -C src clean

FORCE:




