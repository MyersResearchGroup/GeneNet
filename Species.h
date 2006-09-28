#ifndef SPECIES_H_
#define SPECIES_H_
#include <vector>
#include <iostream>

class Specie;

class Species
{
public:
	Specie* get(int i) const;
	int size() const;
	Species();
	virtual ~Species();
	bool addSpecie(Specie * a);
	bool removeTimeSpecie();
private:
	std::vector<Specie*> allSpecies;	
};

std::ostream& operator << (std::ostream& cout, const Species & source);


#endif /*SPECIES_H_*/
