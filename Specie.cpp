#include "Specie.h"
#include "Set.h"
#include "DoubleSet.h"
#include <iostream>
#include <stdio.h>
#include <cassert>
#include <sstream>

std::map<int,Specie*> * Specie::allSpecies = NULL;

void Specie::clearAllInstances(){
	if (allSpecies != NULL){
		for (std::map<int,Specie*>::iterator iter = allSpecies->begin(); iter != allSpecies->end(); iter++){
			Specie * s = (*iter).second;
			delete s;
		}
		allSpecies->clear();
		delete allSpecies;
		allSpecies = NULL;
	}
}

Specie * Specie::addInstance(std::string name, int uid){
	if (allSpecies == NULL){
	  allSpecies = new std::map<int, Specie*>();	
	}
	
	if (allSpecies->find(uid) == allSpecies->end()){
	  //std::cout << "Creating specie " << name << " uid " << uid << "\n";
	  Specie * t = new Specie(name,uid);
	  //std::cout << "Created specie " << t->getGeneName() << " uid " << t->getGeneUID() << "\n";
	  (*allSpecies)[uid] = t;
	  //std::cout << "after insert\n";
	}
	return (*allSpecies)[uid];
}

Specie * Specie::getInstance(std::string name, int uid){
	if (allSpecies == NULL){
	  return NULL;
	  //allSpecies = new std::map<int, Specie*>();	
	}
	
	if (allSpecies->find(uid) == allSpecies->end()){
	  return NULL;
		//std::cout << "Creating specie " << name << " uid " << uid << "\n";
		//Specie * t = new Specie(name,uid);
		//std::cout << "Created specie " << t->getGeneName() << " uid " << t->getGeneUID() << "\n";
		//(*allSpecies)[uid] = t;
		//std::cout << "after insert\n";
	}
	return (*allSpecies)[uid];
}

int Specie::getSpecies(std::string name){
  for (unsigned int i=0;i<allSpecies->size();i++) {
    if ((*allSpecies)[i]->name == name)
      return i;
  }
  return -1;
}

Specie::Specie(std::string n, int uid)
{
	name = n;
	geneUID = uid;
	mySet = new Set();
	mySet->insert(this,0.001);
	myDoubleSet = new DoubleSet();
	myDoubleSet->unionIt(*mySet);
	//std::cout << "Specie " << n << " uid: " << geneUID << " created\n";
}
Specie::Specie(const Specie &s)
{
	name = s.name;
	geneUID = s.geneUID;
	mySet = new Set();
	mySet->insert(this,0.001);
	myDoubleSet = new DoubleSet();
	myDoubleSet->unionIt(*mySet);
}

const Specie & Specie::operator=(const Specie & s){
	if (this != &s){
		name = s.name;
		geneUID = s.geneUID;	
	}
	return *this
;
}


Specie::~Specie()
{
	delete mySet;
	delete myDoubleSet;
}

DoubleSet& Specie::toDoubleSet() const{
	return *myDoubleSet;
}

Set * Specie::toSet() const{
	return mySet;
}
	
	
int Specie::getGeneUID() const
{
	return geneUID;
}
std::string Specie::getGeneName() const{
	return name;	
}

bool operator == (const Specie& s1, const Specie& s2){
	//std::cout << "In the == code with " << s1 << " and " << s2 << "\n";
	return 	(s1.getGeneUID() == s2.getGeneUID() && s1.getGeneName() == s2.getGeneName());
}

bool operator != (const Specie& s1, const Specie& s2){
	return 	!(s1 == s2);
}

std::ostream& operator << (std::ostream& cout, const Specie & source){
	cout << "( '" << source.getGeneName() << "' : '" << source.getGeneUID() << "')";
	assert(source.getGeneUID() < 100);
	return cout;	
}

int Specie::getNumSpecie(){
	return allSpecies->size();	
}

std::string Specie::toIV(char c) const{
  std::ostringstream cout;
  cout << "<";
  for (int i = 1; i < Specie::getNumSpecie(); i++){
    if (getGeneUID() == i){
        cout << c;
    }
    else{
      cout << "n";
    }
  }
  cout << ">";
  return cout.str();
}
