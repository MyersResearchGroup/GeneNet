#ifndef ENCODINGS_H_
#define ENCODINGS_H_
#include <vector>
#include <fstream>
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
	bool useBins(int numBins);
	bool useNumbers(int numBins);
	std::vector<float> getLevels(const Specie * s) const;
	float getProb(const Specie * child, const std::vector<int> * l1, const std::vector<int> * l2) const;
private:
	void clearLevels();
	std::vector<std::vector<float>*> levels;
	Species * s;
	Experiments * e;
	Thresholds * t;
	void fillTSD();
};

#endif /*ENCODINGS_H_*/
