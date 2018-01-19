#ifndef DOUBLESET_H_
#define DOUBLESET_H_
#include <vector>
#include <iostream>

class Set;

class DoubleSet
{
public:
	DoubleSet();
	DoubleSet(const DoubleSet &d);
	virtual ~DoubleSet();
	int size() const;
	Set remove(int i);
	Set remove(const Set & s1);
	std::string removeSubsets();
	std::string filterByScore(double f);
	Set * get(int i) const;
	void unionIt(const Set & s2);
	const DoubleSet & operator=(const DoubleSet & s);
	bool contains(const Set & s);
	bool addIfScoreBetterThanSubsets(const Set & s);
	void clearAllSets();
	Set colapseToSet() const;
        std::string toIV() const;

	
	friend DoubleSet unionIt(const DoubleSet& s1, Set & s2);
private:
	void deleteMyDoubleSet();
	void fillMyDoubleSet(const DoubleSet & d);
	std::vector<Set*> myDoubleSet;
};

DoubleSet unionIt(const DoubleSet& s1, Set& s2);

std::ostream& operator << (std::ostream& cout, const DoubleSet & source);

#endif /*DOUBLESET_H_*/
