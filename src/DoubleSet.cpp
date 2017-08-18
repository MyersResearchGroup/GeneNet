#include "DoubleSet.h"
#include "Set.h"
#include "Specie.h"
#include <cmath>
#include <cassert>
#include <sstream>

extern int DEBUG_LEVEL;

DoubleSet::DoubleSet()
{
}

DoubleSet::~DoubleSet()
{
	deleteMyDoubleSet();
}

void DoubleSet::clearAllSets(){
	deleteMyDoubleSet();
}

void DoubleSet::deleteMyDoubleSet(){
	while((int)myDoubleSet.size() > 0){
		Set * d = myDoubleSet.back();
		myDoubleSet.pop_back();
		delete d;	
	}
}
void DoubleSet::fillMyDoubleSet(const DoubleSet & d){
	for (int i = 0; i < d.size(); i++){
		myDoubleSet.push_back(new Set(*d.get(i)));
	}	
}

std::string DoubleSet::toIV() const{
  std::ostringstream cout;
  for (int i = 0; i < (int)myDoubleSet.size(); i++){
    cout << myDoubleSet[i]->toIV() << " " << fabs(myDoubleSet[i]->getScore()) << " ";
  }
  return cout.str();
}

DoubleSet::DoubleSet(const DoubleSet &d){
	fillMyDoubleSet(d);
}

const DoubleSet & DoubleSet::operator=(const DoubleSet & s){
	if (this != &s){
		deleteMyDoubleSet();
		fillMyDoubleSet(s);
	}	
	return *this;
}


int DoubleSet::size() const{
	return myDoubleSet.size();
}

Set DoubleSet::remove(int i){
	assert(i >= 0 && i < size());
	Set * s = myDoubleSet[i];
	myDoubleSet.erase(myDoubleSet.begin()+i);
	Set p(*s);
	delete s;
	return p;
}

Set DoubleSet::remove(const Set & s1){
	for (int i = 0; i < (int)myDoubleSet.size(); i++){
		if ((*myDoubleSet.at(i)) == s1){
			Set * s = myDoubleSet.at(i);
			myDoubleSet.erase(myDoubleSet.begin()+i);
			Set p(*s);
			delete s;
			return p;
		}
	}
	Set p;
	return p;
}

bool DoubleSet::addIfScoreBetterThanSubsets(const Set & s){
	bool better = true;
	for (int i = 0; i < (int)myDoubleSet.size(); i++){
		Set * s2 = myDoubleSet.at(i);
		if (s.contains(*s2) && fabs(s2->getScore()) > fabs(s.getScore())){
			if (DEBUG_LEVEL>0){
				std::cout << "\tNot Better because " << fabs(s2->getScore()) << " > " << fabs(s.getScore()) << "\n";
			}
			better = false;
			i = (int)myDoubleSet.size();
		}
		else{
			if (DEBUG_LEVEL>0){
				if(s.contains(*s2)){
					std::cout << "\tSpecie " << *s2 << " with score " << fabs(s2->getScore()) << " > " << fabs(s.getScore()) << "\n";
				}
			}	
		}
	}
	if (better){
		unionIt(s);
	}
	return better;
}


std::string DoubleSet::removeSubsets(){
	//std::vector<Set * > v;
        std::ostringstream out;
	for (int i = myDoubleSet.size()-1; i > 0; i--){
		for (int j = i-1; j >= 0; j--){
			if (myDoubleSet.at(i)->contains(*myDoubleSet.at(j))){
				//v.push_back(myDoubleSet.at(i));
				Set * s = *(myDoubleSet.begin()+j);
				if (DEBUG_LEVEL > 0){
					std::cout << "\t\tRemoving " << *s << " because it is a subset\n";
				}			
                                out << "\t" << s->toIV() << " " << fabs(s->getScore()) << " case 4 " << myDoubleSet.at(i)->toIV() << " " << fabs(myDoubleSet.at(i)->getScore()) << "\n";
                                /*
                                  if (m < 0){
                                  contenders << "\t" << baseSet.get(currentBases[i])->toIV('r') << " " << fabs(m) << " case 4 " ;
                                  }
                                  else{
                                  contenders << "\t" << baseSet.get(currentBases[i])->toIV('a') << " " << fabs(m) << " case 4 ";
                                  }
                                  contenders << currentWorking.toIV() << " " << fabs(score) << "\n";
                                */
				myDoubleSet.erase(myDoubleSet.begin()+j);
				delete s;
				i--;
			}
		}	
	}
	//return v;
        return out.str();
}

std::string DoubleSet::filterByScore(double f){
        std::ostringstream out;
	for (int i = 0; i < (int)myDoubleSet.size(); i++){
		if (fabs(myDoubleSet.at(i)->getScore()) <= f){
			Set * s = *(myDoubleSet.begin()+i);
			if (DEBUG_LEVEL > 0){
				std::cout << "\t\tRemoving " << *s << " because of score fabs(" << s->getScore() << ") < " << f << "\n";
			}
                        out << "\t" << s->toIV() << " " << fabs(s->getScore()) << " case 5 " << f << "\n";
			myDoubleSet.erase(myDoubleSet.begin()+i);
			delete s;
			i--;
		}	
	}
        if ((int)myDoubleSet.size() > 0){
          std::ostringstream backgroundIV;
          backgroundIV << "<";
          for (int i = 1; i < Specie::getNumSpecie(); i++){
            backgroundIV << "n";
          }
          backgroundIV << ">";
          out << "\t" << backgroundIV.str() << " " << f << " case 6 " << toIV() << "\n";
        }
        return out.str();
}

bool DoubleSet::contains(const Set & s){
	for (int i = 0; i < (int) myDoubleSet.size(); i++){
		if (*myDoubleSet.at(i) == s){
			return true;	
		}	
	}
	return false;
}


Set * DoubleSet::get(int i) const{
	assert(i >= 0 && i < size());
	return myDoubleSet[i];
}


void DoubleSet::unionIt(const Set & s2){
	//insert the 'set' into the correct position
	//zero elements
	//std::cout << "Inserting " << *s2 << " into a size of " << size() << "\n";
	if (myDoubleSet.size() == 0){
		myDoubleSet.push_back(new Set(s2));
		return;	
	}

	//std::cout << "Printing elements\n";
	//std::vector<Set*>::iterator tmp = myDoubleSet.begin();
	//while (tmp != myDoubleSet.end()){
	//	std::cout << **tmp << "\n";	
	//	tmp++;
	//}

	//check at the first, then the rest
	std::vector<Set*>::iterator iter = myDoubleSet.begin();
	if (s2 < *(*iter)){
		myDoubleSet.insert(iter,new Set(s2));
		return;
	}
	//std::cout << "Element " << *(*iter) << "\n";
	iter++;
	while(iter != myDoubleSet.end()){
		Set * prev = (*(iter-1));
		Set * cur = (*iter);
		//std::cout << "Element " << *(*iter) << "\n";
		if (s2 == *prev || s2 == *cur){
			//it is already in the set
			return;		
		}
		else if (s2 > *prev && s2 < *cur){
			myDoubleSet.insert(iter,new Set(s2));
			return;
		}
		iter++;
	}
	if (s2 > *myDoubleSet.at(size()-1)){
		myDoubleSet.push_back(new Set(s2));
		return;
	}
	//std::cout << "Somehow made it to here, so " << s2 << " !> " << *myDoubleSet.at(size()-1) << "\n";
	return;
}

Set DoubleSet::colapseToSet() const{
	Set S;
	for (int i = 0; i < (int)myDoubleSet.size(); i++){
		Set * p = myDoubleSet.at(i);
		for (int j = 0; j < p->size(); j++){
			S.insert(p->get(j),p->getIndividualScore(p->get(j)->getGeneUID()));
		}
	}
	return S;
}


DoubleSet unionIt(const DoubleSet& s1, Set& s2){
	DoubleSet newSet(s1);
	newSet.unionIt(s2);
	return newSet;
}

std::ostream& operator << (std::ostream& cout, const DoubleSet & source){
	if (source.size() < 2){
		cout << "<";
		for (int i = 0; i < source.size(); i++){
			cout << " " << *source.get(i) << " ";	
		}	
		cout << ">";
	}
	else{
		cout << "\n<\n";
		for (int i = 0; i < source.size(); i++){
			cout << *source.get(i) << "\n";	
		}	
		cout << ">\n";
	}
	return cout;
}


