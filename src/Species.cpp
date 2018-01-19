#include "Species.h"
#include <cassert>
#include "Specie.h"

Species::Species()
{
}

Species::~Species()
{
	allSpecies.clear();
}

Specie* Species::get(int i) const{
	assert(i >= 0 && i < (int)allSpecies.size());
	return allSpecies.at(i);
}
int Species::size() const{
	return allSpecies.size();	
}

bool Species::addSpecie(Specie * a){
	//check for adding the same specie, but a different tsd file
	if (a->getGeneUID() >= 0 && a->getGeneUID() < size()){
		if(a == get(a->getGeneUID())){
			return true;	
		}
		return false;
	}
	//now check that we are adding in an extra one
	if (a->getGeneUID() != (int)allSpecies.size()){
		return false;	
	}
	allSpecies.push_back(a);
	return true;
}

bool Species::removeTimeSpecie(){
	//Specie * p = get(size()-1);
	Specie * p = get(0);
	if (p->getGeneName() != "time" && p->getGeneName() != "Time" && p->getGeneName() != "TIME"){
		std::cout << "ERROR:  The first column is not a 'time' column\nReformat the data\n";
		return false;	
	}
	//allSpecies.pop_back();
	allSpecies.erase(allSpecies.begin());
	return true;
}

std::ostream& operator << (std::ostream& cout, const Species & source){
	cout << "Species are:\n";
	for (int i = 0; i < source.size(); i++){
		cout << "\t" << *source.get(i) << "\n";
	}
	cout << "End Species\n";
	return cout;
}

