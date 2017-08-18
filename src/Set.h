#ifndef SET_H_
#define SET_H_
#include <vector>
#include <iostream>
#include <map>

class Specie;


class Set
{
public:
	static const double DELTA = 0.0001;
	Set();
	Set(const Set & s);
	virtual ~Set();
	int size() const;
	Specie * get(int i) const;
	void insert(Specie * s, double individualScore);
	
	bool contains(const Set& s1) const;
	bool containsSpecieID(int s1) const;
	
	bool sortsLowToHigh(int specieUID) const;
	
	const Set & operator=(const Set & s);
	
	double getScore() const;
	void setScore(int specieUID, double x);
	double getIndividualScore(int specieUID) const;
	
	double getCompetitionScore() const;
	void setCompetitionScore( double x);

        std::string toIV() const;
        std::string toIndividualIV() const;
	friend bool operator == (const Set& s1, const Set& s2);
	friend bool operator != (const Set& s1, const Set& s2);
	friend bool operator <  (const Set& s1, const Set& s2);
	friend bool operator >  (const Set& s1, const Set& s2);
private:
	std::vector<Specie*> mySet;
	std::vector<double> individualScores;

	double myScore;
	double competitionScore;
};

Set unionIt(const Set& s1,const Set& s2);
bool operator == (const Set& s1, const Set& s2);
bool operator != (const Set& s1, const Set& s2);
bool operator <  (const Set& s1, const Set& s2);
bool operator >  (const Set& s1, const Set& s2);
Set  operator - (const Set & s1, const Set & s2);

std::ostream& operator << (std::ostream& cout, const Set & source);

#endif /*SET_H_*/
