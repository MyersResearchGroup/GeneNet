#ifndef SPECIE_H_
#define SPECIE_H_
#include <string>
#include <map>

class Set;
class DoubleSet;

class Specie
{
public:
	static Specie * addInstance(std::string name, int uid);
        static Specie * getInstance(std::string name, int uid);
        static int getSpecies(std::string name);
	static void clearAllInstances();
	static int getNumSpecie();
	int getGeneUID() const;
	std::string getGeneName() const;
	DoubleSet& toDoubleSet() const;
	Set * toSet() const;
        std::string toIV(char c) const;
protected:
	Specie(std::string name, int uid);
	Specie(const Specie & s);
	virtual ~Specie();
	const Specie & operator=(const Specie & s);
private:
	int geneUID;
	Set * mySet;
	DoubleSet * myDoubleSet;
	std::string name;
	static std::map<int,Specie*> * allSpecies;
	
};

bool operator == (const Specie& s1, const Specie& s2);

bool operator != (const Specie& s1, const Specie& s2);
std::ostream& operator << (std::ostream& cout, const Specie & source);

#endif /*SPECIE_H_*/
