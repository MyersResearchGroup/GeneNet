#include "TSDPoint.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>
#include "Set.h"
#include "Specie.h"
#include "Encodings.h"
#include <stdlib.h>

extern int DEBUG_LEVEL;

bool InvertSortOrder = false;
std::vector<int> TSDPoint::maxEncodings;

std::map<std::string,TSDPoint*> * TSDPoint::thePoints = NULL;

std::vector<TSDPoint*> * TSDPoint::initialValues = NULL;

Set * TSDPoint::G = NULL;
Set * TSDPoint::P = NULL;
int TSDPoint::numS = 0;

void TSDPoint::InitializePoints(){
	if (thePoints == NULL){
		thePoints = new std::map<std::string,TSDPoint*>();
		initialValues = new std::vector<TSDPoint*>();
	}
}

bool TSDPoint::incrementLevel(int * currentAssignment, int * holdSteady, int * totalSize, int size){
	//base case, no size left
	if (size <= 0){
		return false;	
	}
	//a '-1' means not to use this one
	else if (holdSteady[size-1] != -1){
		return incrementLevel(currentAssignment, holdSteady, totalSize, size-1);	
	}
	//we added 1 and carry it over to the next
	else if (currentAssignment[size-1] == totalSize[size-1]){
		currentAssignment[size-1] = 0;
		return incrementLevel(currentAssignment, holdSteady, totalSize, size-1);	
	}
	else {
		currentAssignment[size-1]++;
		return true;	
	}	
}

double TSDPoint::calculateProbability(int child, int * holdSteady, int * totalSize, int size){
	std::ostringstream state;

	assert(holdSteady[child] != -1);

	//tack on a - for the time state NO MORE
	//state << "-";
	for (int i = 0; i < size; i++){
		if (holdSteady[i] != -1){
			state << holdSteady[i];
		}
		else {
			state << "-";	
		}
	}
	//std::cout << "looking for state " << state.str() << "\n";

	TSDPoint * p = getTSDPoint(state.str());
	//We no longer need to look through all the states, if we have them stored already.
	if (p->hasBeenInitialized == false){
		p->hasBeenInitialized = true;
		/*
		std::cout << "Finding Probability with \n";
		std::cout << "Hold Steady: ";
		for (int i = 0; i < size; i++){
			std::cout << holdSteady[i];
		}
		std::cout << "\n";
		std::cout << "Total Size: ";
		for (int i = 0; i < size; i++){
			std::cout << totalSize[i];
		}
		std::cout << "\n";
		*/
		//no longer go through all states, but just the actual data
		/*
		bool moreLevels = true;
		int * vstate = new int[size];
		//fill with initial values
		for (int i = 0; i < size; i++){
			vstate[i] = 0;
			if (holdSteady[i] != -1){
				vstate[i] = holdSteady[i];
			}	
		}
		//cycle through the states adding up the probabilities
		while (moreLevels){
			state.str("");
			for (int i = 0; i < size; i++){
				state << vstate[i];
			}
			TSDPoint * point = getTSDPoint(state.str());
			if (point->hasBeenInitialized == true){
				std::cout << "Found a level at " << state.str() << "\n";
				for (int i = 0; i < size; i++){
					p->seen[i]    += point->seen[i];
					p->risings[i] += point->risings[i];
				}
			}
			moreLevels = incrementLevel(vstate, holdSteady, totalSize, size);
		}
		delete [] vstate;
		*/
		//std::cout << "Updating tsdvalues for " << state.str() << "\n";
		for (std::vector<TSDPoint *>::iterator iter = initialValues->begin(); iter != initialValues->end(); iter++){
			//check that the values match
			//std::cout << "\tChecking it against " << (*iter)->rowValues << " ";
			bool matched = true;
			for (int i = 0; i < size; i++){
				if (holdSteady[i] != -1){
					char c[2];
					c[0] = (*iter)->rowValues[i];
					c[1] = 0; //to terminate the atoi function
					if (std::atoi(c) != holdSteady[i]){
						//std::cout << "at " << i << " '" << std::atoi(c) << "' or '" << c << "' != '" << holdSteady[i] << "'";
						matched = false;
						i = size;
					}
				}
			}
			if (matched){
				//std::cout << "\tMatched!";
				//std::cout << "\tMatched at " << (*iter)->rowValues << " with ";
				for (int i = 0; i < size; i++){
					p->seen[i]    += (*iter)->seen[i];
					p->risings[i] += (*iter)->risings[i];
					//std::cout << "+=" << (*iter)->risings[i] << "/" << (*iter)->seen[i] << " to " <<  p->risings[i] << "/" << p->seen[i] << "\t";
 				}
 				//std::cout << "\n";
			}
			//std::cout << "\n";
		}
	}


	//get the probability
	double s = p->seen[child];
	double r = p->risings[child];
	if (DEBUG_LEVEL>0){
		std::cout << "\t\t\tProb for '" << p->rowValues << "' child " << child << " is " << r << " / " << s << " = ";
	}
	if (s == 0){
		if (DEBUG_LEVEL>0){
			std::cout << "-1\n";
		}
		return -1;	
	}
	else if (r == 0){
		if (DEBUG_LEVEL>0){
			std::cout << "0\n";
		}
		return 0;	
	}
	double t = r/s;
	if (DEBUG_LEVEL>0){
		std::cout << t << "\n";
	}
	return t;	
}


void TSDPoint::clearAll(){
	if (thePoints != NULL){
		std::map<std::string, TSDPoint *>::iterator iter;
		//We have to check if there is something in the map, based on a bad initilaztion
		//of the stl......
		for (iter = thePoints->begin(); iter != thePoints->end() && (int)thePoints->size() != 0; iter++){
			TSDPoint * p = iter->second;
			delete p;
		}
		thePoints->clear();
		delete thePoints;
		delete initialValues;
		thePoints = NULL;
		initialValues = NULL;
	}
}

TSDPoint * TSDPoint::getTSDPoint(std::string rowValue){	
	numS = (int)rowValue.size();
	if (thePoints->find(rowValue) == thePoints->end()){
		TSDPoint * v = new TSDPoint(rowValue);
		(*thePoints)[rowValue] = v;
	}
	TSDPoint * p = (*thePoints)[rowValue];
	return p;

}

bool TSDPoint::increaseMinuses(int * v, int stringSize, int size){
	if (size <= 0){
		return false;	
	}
	else if (size == 1){
		if (v[size-1] == stringSize-1){
			return false;	
		}
		else{
			v[size-1]++;
			return true;
		}
	}
	//we add one more than the previous
	else if (v[size-1] == stringSize-1){
		if (increaseMinuses(v,stringSize-1, size-1)){
			v[size-1] = v[size-2]+1;
			return true;
		}
		else{
			return false;	
		}
	}
	else {
		v[size-1]++;
		return true;	
	}	
}

//Assume that this only gets called by Encodings when given a real TSD row.
void TSDPoint::updateValue(int * childrenSeen, int * childrenRose, int size){
	//update yourself
	updateValueStop(childrenSeen, childrenRose, size);
	if (std::find(initialValues->begin(), initialValues->end(),this) == initialValues->end()){
		initialValues->push_back(this);
		/*
		bool inserted = false;
		for (std::vector<TSDPoint *>::iterator iter = initialValues->begin(); iter != initialValues->end() && inserted == false; iter++){
			if (rowValues < (*iter)->rowValues){
				inserted = true;
				initialValues->insert(iter,this);
			}
		}
		if (inserted == false){
			initialValues->push_back(this);
		}
		*/
	}

	//do all the subsets
	//std::cout << "\tIn TSDPoint " << rowValues << " with child " << child << " and " << this->seen[child] << " " << this->risings[child] << "\n";
	//assert(this->rowValues.find('-') == std::string::npos);  //This assertion no longer works with time at the front


	//Don't worry about single parent cases anymore.  They will be found by one pass through the data later, and no longer based on every specie levels
	/*
	//Fill in the single cases, as we don't want to have to find these cases later
	std::ostringstream inv;
	for (int i = 0; i < (int)rowValues.length(); i++){
		inv << '-';
	}
	int * v = new int[1]; //Keeps track of what column we are on
	//put the last one back one so that in increase function works the first time
	v[0] = -1;
	while(increaseMinuses(v, rowValues.length(), 1)){
		//put the - at the correct position, use it both for - and inverse
		std::string in(inv.str());
		in[v[0]] = rowValues[v[0]]; //fill in the value for the child
		
		//std::cout << "\t\tFound out that '" << s[child] << "' != '-'\n";
		TSDPoint * p = getTSDPoint(in);
		p->updateValueStop(childrenSeen, childrenRose, size);
	}
	delete [] v;
	*/

	/*For the idea I had of storing every possible combination
	//std::cout << "\tRecursivly creating on " << this->rowValues << "\n";
	//build up the inverse string;
	//we only have to go up to half of the string if we use inverses
	//An odd string needs to go to one more than half.  ie 13 needs 7 and 14 needs 7
	//however for evens the middle /largest/ even the inverse as it produces dups, so don't do it
	//it seems that this middle for odds doesn't duplicate like the evens....
	//std::cout << "For a string of " << rowValues.length() << " we have " << (int)((int)rowValues.length()+1)/2 << " items\n";
	for (int i = 1; i <= (int)((int)rowValues.length()+1)/2; i++){
		int * v = new int[i];
		for (int j = 0; j < i; j++){
			v[j] = j;	
		}
		//put the last one back one so that in increase function works the first time
		v[i-1] = i-2;
		while(increaseMinuses(v, rowValues.length(), i)){
			//put the - at the correct position, use it both for - and inverse
			std::string s(this->rowValues);
			std::string in(inv.str());
			for (int j = 0; j < i; j++){
				s[v[j]] = '-';
				in[v[j]] = rowValues[v[j]];
			}
			
			std::cout << "\tWorking with '-' at ";
			for (int j = 0; j < i; j++){
				std::cout << v[j] << " ";
			}
			std::cout << "which is " << s;
			if (!(i == (int)((int)rowValues.length()+1)/2 && rowValues.length() % 2 == 0)){
				std::cout << " and inverse too " << in;
			}
			std::cout  << "\n";
			//std::cout << "\t\tFound out that '" << s[child] << "' != '-'\n";
			TSDPoint * p = getTSDPoint(s);
			p->updateValueStop(childrenSeen, childrenRose, size);

			if (!(i == (int)((int)rowValues.length()+1)/2 && rowValues.length() % 2 == 0)){
				//std::cout << "\t\tcalling inverse\n";
				p = getTSDPoint(in);
				p->updateValueStop(childrenSeen, childrenRose, size);
			}
		}
		delete [] v;
	}
	*/
}

void TSDPoint::updateValueStop(int * childrenSeen, int * childrenRose, int size){
	assert(size == (int)this->seen.size());
	hasBeenInitialized = true;
	//we could not put into info for '-'s but, just do it for now
	for (int i = 0; i < size; i++){
		this->seen[i] += childrenSeen[i];
		this->risings[i] += childrenRose[i];
	}
	/*
	std::cout << "Updating TSDPoint " << this->rowValues << " s: ";
	for (int i = 0; i < size; i++){
		std::cout << seen[i] << " ";
	}
	std::cout << " r: ";
	for (int i = 0; i < size; i++){
		std::cout << risings[i] << " ";	
	}
	std::cout << "\n";
	*/
}


TSDPoint::TSDPoint(std::string rowValues)
{
	this->hasBeenInitialized = false;
	this->rowValues = rowValues;
	for (int i = 0; i < (int)rowValues.length(); i++){
		this->seen.push_back(0);
		this->risings.push_back(0);	
	}
}

TSDPoint::~TSDPoint()
{
}

bool TSDPoint::sameLevels(TSDPoint * i){
	//if the Parents in G and P are the same, (they both sort to the same position) they can be merged
	if (sortTSDPointsHelper(this,i,G) == 0 && sortTSDPointsHelper(i,this,P) == 0){
		return true;	
	}
	return false;
}
bool TSDPoint::cannotCompareLevels(TSDPoint* i){
	//if the G set parents sort to the same position, these points can be compared
	if (sortTSDPointsHelper(this,i,G) == 0){
		return false;
	}
	return true;
}

int TSDPoint::sortTSDPointsHelper(const TSDPoint * a, const TSDPoint * b, const Set * c){
	std::vector<int> lowHigh;
	std::vector<int> highLow;
	for (int i = 0; i < c->size(); i++){
		int w = c->get(i)->getGeneUID();
		bool lowHighSort = c->sortsLowToHigh(w);
		if (InvertSortOrder){
			lowHighSort = !lowHighSort;	
		}
		if(lowHighSort){
			lowHigh.push_back(w);	
		}
		else{
			highLow.push_back(w);	
		}

	}
	for (int i = 0; i < (int)lowHigh.size(); i++){
		int w = lowHigh[i];
		if (a->rowValues[w] < b->rowValues[w]){
			//std::cout << "\tA is smaller because " << a->rowValues[w] << " < " << b->rowValues[w] << "\n";
			return 1;	
		}
		else if (a->rowValues[w] > b->rowValues[w]){
			//std::cout << "\tA is larger because " << a->rowValues[w] << " > " << b->rowValues[w] << "\n";
			return -1;
		}
	}
	for (int i = 0; i < (int)highLow.size(); i++){
		int w = highLow[i];
		if (a->rowValues[w] < b->rowValues[w]){
			return -1;	
		}
		else if (a->rowValues[w] > b->rowValues[w]){
			return 1;
		}
	}
	return 0;
}

bool TSDPoint::sortTSDPoints(const TSDPoint * a, const TSDPoint * b){
	//first we sort by the member of the G set, then the P set.  All others don't matter
	//do not invert the globals to match perls order
	bool tmp = InvertSortOrder;
	InvertSortOrder = false;
	int x = sortTSDPointsHelper(a,b,G);
	InvertSortOrder = tmp;
	if (x == 1){
		return true;	
	}
	else if (x == -1){
		return false;	
	}
        /*
        int latticea = LatticeLevel(a,*P,GLOBAL_L);
        int latticeb = LatticeLevel(a,*P,GLOBAL_L);
        if (latticea < latticeb){
          return 1;
        }
        else if (latticeb < latticea){
          return 0;
        }
        */
	x = sortTSDPointsHelper(a,b,P);
	if (x == 1){
		return true;	
	}
	return false;
}


void TSDPoint::setMaxEncodings(const Encodings & L){
  for (int i = 0; i < L.totalSpecies(); i++){
    maxEncodings.push_back(L.getMaxLevel(L.getSpecie(i)));
  }
  return;
}


int TSDPoint::LatticeLevel(const Set& P){

  if ((int)maxEncodings.size() == 0){
    cout << "ERROR: Calling LatticeLevel with a null\n";
    exit(0);
  }

  //if (DEBUG_LEVEL > 1){
  //  cout << "\t\t\t\tCalculating lattice level for " << bin->rowValues << " and " << P << "\n";
  //}

  int ll = 0;
  for(int j = 0; j < P.size(); j++){
    Specie * st = P.get(j);
    char valc [2];
    valc[0]= rowValues[st->getGeneUID()];
    valc[1] = 0;
    int val = atoi(valc);
    int max = maxEncodings[st->getGeneUID()];
    if (P.sortsLowToHigh(st->getGeneUID())){
      ll += val;
    }
    else{
      ll += max - val;
    }
  }

  //if (DEBUG_LEVEL > 1){
  //  cout << "\t\t\t\t\tThe level is " << ll << "\n";
  //}
  return ll;
}
