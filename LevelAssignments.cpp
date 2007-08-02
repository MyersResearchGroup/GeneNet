#include "LevelAssignments.h"
#include "Encodings.h"
#include "Set.h"
#include "DoubleSet.h"
#include "Specie.h"
#include <cassert>

LevelAssignments::LevelAssignments()
{
	//mySet = NULL;
}

void LevelAssignments::fillAssignments(const LevelAssignments & s){
	for (int i = 0; i < (int)s.myAssignments.size(); i++){
		std::vector<int> * v = new std::vector<int>();
		for (int j = 0; j < (int)s.myAssignments.at(i)->size(); j++){
			v->push_back(s.myAssignments.at(i)->at(j));
		}
		myAssignments.push_back(v);
	}	
}

void LevelAssignments::deleteAssignments(void){
	while (myAssignments.size() > 0){
		std::vector<int> * v = myAssignments.back();
		myAssignments.pop_back();
		delete v;	
	}	
}

LevelAssignments::~LevelAssignments()
{
	deleteAssignments();
	//delete mySet;
}
LevelAssignments::LevelAssignments(const LevelAssignments & s)
{
		deleteAssignments();
		fillAssignments(s);
}

const LevelAssignments & LevelAssignments::operator=(const LevelAssignments & s){
	if (this != &s){
		deleteAssignments();
		fillAssignments(s);
	}	
	return *this;
}

bool LevelAssignments::incrementLevel(int * currentAssignment, std::vector<double> * info, int size){
	//base case, no size left
	if (size <= 0){
		return false;	
	}
	//a '-1' means not to use this one
	else if (currentAssignment[size-1] == -1){
		return incrementLevel(currentAssignment, info, size-1);	
	}
	//we added 1 and carry it over to the next
	//NOTE 'info' contains the level assignment #, so for 2 levels there is one #, thus don't -1 to size
	else if (currentAssignment[size-1] == (int)info[size-1].size()){
		currentAssignment[size-1] = 0;
		return incrementLevel(currentAssignment, info, size-1);	
	}
	else {
		currentAssignment[size-1]++;
		return true;	
	}	
}

void LevelAssignments::creationHelper(const Set& P, const Encodings& L){
	//mySet = new Set(P);
	bool done = false;
	int size = L.totalSpecies()+1; // for time
	int * currentAssignment = new int[size];
	std::vector<double> * info = new std::vector<double>[size];
	currentAssignment[0] = -1;	//Assign time a -1
	for (int i = 1; i < size; i++){
		Specie * tmp = Specie::getInstance("tmp",i);
		info[i] = L.getLevels(tmp);

		//don't assign variables that are not in it
		currentAssignment[i] = 0;	
		if (! P.containsSpecieID(i)){
			currentAssignment[i] = -1;	
		}
	}
	while(!done){
		std::vector<int> * v = new std::vector<int>();
		for (int i = 0; i < size; i++){
			v->push_back(currentAssignment[i]);
		}
		//flip the repressors so that they start at their highest level
		for (int i = 0; i < size; i++){
			if (currentAssignment[i] != -1){ //do not flip things that are not being used
				//std::cout << "Testing " << i << " value " << currentAssignment[i] << " for activation\n";
				if (!P.sortsLowToHigh(i)){
					int topLevel = (int)(info[i].size());
					int currentValue = (*v)[i];
					(*v)[i] = topLevel-currentValue; //take the higest level and subtract the current level
				}
			}
		}
		myAssignments.push_back(v);

		//see if we are done	
		done = ! incrementLevel(currentAssignment, info, size);
	}
	delete [] info;
	delete [] currentAssignment;	
}

LevelAssignments::LevelAssignments(const Set& P, const Encodings& L){
	creationHelper(P,L);
}
LevelAssignments::LevelAssignments(const DoubleSet& G, const Encodings& L){
	creationHelper(G.colapseToSet(),L);
}
int LevelAssignments::size(){
	return myAssignments.size();
}

std::vector<int>* LevelAssignments::get(int i){
	if (i < 0 || i >= (int)myAssignments.size()){
		return NULL;
	}
	return myAssignments.at(i);
}

