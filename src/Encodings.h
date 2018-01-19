#ifndef ENCODINGS_H_
#define ENCODINGS_H_
#include <vector>
#include <fstream>
#include <iostream>
#include "Species.h"
#include "Experiments.h"
#include "Set.h"
#include "Thresholds.h"
#include "TSDPoint.h"

class Encodings
{
public:
	Encodings();
	void initialize(Species * species, Experiments * experiments, Thresholds * t);
	virtual ~Encodings();
	int totalSpecies() const;
	bool useBins(int numBins, bool useSpeciesLevels, bool succ, bool pred);
	bool useNumbers(int numBins, bool useSpeciesLevels, bool succ, bool pred);
	bool useFile(ifstream & lvl_file, bool checkOrdering, bool succ, bool pred);
	std::vector<double> getLevels(const Specie * s) const;
        int getMaxLevel(const Specie * s) const;
	double getProb(const Specie * child, const std::vector<int> * l1, const std::vector<int> * l2) const;
	void printLevels();
        Specie * getSpecie(int i) const;
private:
	void clearLevels();
	std::vector<std::vector<double>*> levels;
	Species * s;
	Experiments * e;
	Thresholds * t;
	void fillTSD(bool succ, bool pred);
};

#endif /*ENCODINGS_H_*/
