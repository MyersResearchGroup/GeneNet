#ifndef EXPERIMENTS_H_
#define EXPERIMENTS_H_
#include <vector>

class Specie;

using namespace std;

class Experiments
{
public:
	Experiments();
	Experiments(const Experiments & exp);
	Experiments removeMutations(Specie *s);
	bool addTimePoint(int experiment, int row, int column, double data);
	double getTimePoint(int experiment, int row, int column);
	std::vector<double> getSortedValues(int column);
	std::vector<double> * getRow(int experiment, int row);
	int totalExperiments();
	int totalRows(int experiment);
	virtual ~Experiments();

	const Experiments & operator=(const Experiments & e);

private:
	vector<vector<vector<double > *> *> * experiments;
	void deleteExperiments();
	void fillExperiments(const Experiments & exp);
};

#endif /*EXPERIMENTS_H_*/
