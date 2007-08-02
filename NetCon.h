#ifndef NETCON_H_
#define NETCON_H_
#include <vector>
#include <sstream>
#include <stdio.h>
#include <iostream>

class Specie;
class Set;
class DoubleSet;
class Species;
class Experiments;
class NetCon;
class Thresholds;
class Encodings;

extern const int COMPETITION_LOG;
extern int DEBUG_LEVEL;

class NetCon
{
public:
	const static int ACTIVATION = 1;
	const static int REPRESSION = 2;
	const static int WEEK_CONNECTION = 3;
	const static int NOCONNECTION = 4;
	const static int EITHER = 5;
	const static int MAX_TYPE_NUMBER = 5;

	NetCon();
	NetCon(const NetCon & n);
	virtual ~NetCon();
	void unionIt(const Set & mySet, int TYPE, const Specie& s, double score);
	bool containsAnyEdges(const Specie& s);
	Set getSingleParentsFor(const Specie & s);
	std::string removeSubsets(const Specie & s);
	std::string filterByScore(const Specie & s,double f);
	int totalParents(const Specie& s);
	std::string removeLosers(const Specie& s, const DoubleSet& s1, double * scores, std::ostringstream & contenders);

	bool addIfScoreBetterThanSubsets(const Specie & s, const Set & set);

	friend std::vector<DoubleSet> assignMatchups(const Specie& s, const Species& S, const Experiments& E,const NetCon& C, const Thresholds& T, const Encodings& L);
	friend std::ostream& operator << (std::ostream& cout, const NetCon & source);

	const NetCon & operator=(const NetCon & s);
	DoubleSet * getParentsFor(const Specie & s);
	
private:
	std::vector< DoubleSet* > myConnections;
	void deleteConnections();
	void fillConnections(const NetCon& n);
};

std::ostream& operator << (std::ostream& cout, const NetCon & source);


#endif /*NETCON_H_*/
