#include "Set.h"
#include "Specie.h"
#include <cassert>
#include <cmath>
#include <sstream>
#include <stdlib.h>

#define Set__DELTA 0.00001

extern int DEBUG_LEVEL;

Set::Set()
{
	myScore = 0;
	competitionScore = 0;
}

Set::~Set()
{
}

Set::Set(const Set & s){
	mySet = s.mySet;
	myScore = s.myScore;
	competitionScore = s.competitionScore;
	individualScores = s.individualScores;
}

double Set::getScore() const{
	return myScore;
}

double Set::getCompetitionScore() const{
	return competitionScore;	
}
void Set::setCompetitionScore(double x){
	competitionScore = x;	
}

double Set::getIndividualScore(int specieUID) const{
	for (int i = 0; i < (int) mySet.size(); i++){
		if (specieUID == mySet.at(i)->getGeneUID()){
			return individualScores[i];
		}
	}
	std::cout << "ERROR in getIndividualScore: unable to find specie id " << specieUID << " in " << *this << "\n";
	exit(0);
	return -1;
}

void Set::setScore(int specieUID, double x){
	if (specieUID == -1){
		myScore = x;
		//if there is only 1 specie, give it this score as well
		//however, don't give it a score if it is a 0 score, as this meant that using harsher numbers resulted in a lessened score.
		if (mySet.size() == 1 && fabs(x) > 0.001){
			individualScores[0] = (x);
		}
		return;
	}
	else{
		for (int i = 0; i < (int) mySet.size(); i++){
			if (specieUID == mySet.at(i)->getGeneUID()){
				individualScores[i] = x;
				return;
			}
		}
	}
	std::cout << "ERROR in setScore: unable to find specie id " << specieUID << " in " << *this << "\n";
	exit(0);
}

bool Set::sortsLowToHigh(int specieUID) const{
	//if it is the majority it sorts low to high
	//else it sorts high to low
	int activators = 0;
	int repressors = 0;
	int isActivator = -1;
	for (int j = 0; j < size(); j++){
		if (individualScores[j] >= 0){
			activators++;
		}
		else{
			repressors++;
		}
		if (specieUID == get(j)->getGeneUID()){
			if (individualScores[j] >= 0){
				isActivator = 1;
			}
			else{
				isActivator = 0;
			}
		}
	}
	bool returnValue = 0;
	if (isActivator == -1){
          if (DEBUG_LEVEL>0){
		std::cout << "Specie uid " << specieUID << " not found, but is should have been found, or this was a bad call for isActivator\n";
		assert(false);
		returnValue = false;
          }
	}
	else if (activators == 0 || repressors == 0){
		returnValue = true;	
	}	
	//tie goes to activators, so activators must be >=
	else if (isActivator == 1 && activators >= repressors){
		returnValue = true;
	}
	else if (isActivator == 0 && repressors > activators){
		returnValue = true;
	}
	else{
		returnValue = false;
	}
	return returnValue;
}

const Set & Set::operator=(const Set & s){
	if (this != &s){
		mySet = s.mySet;
		myScore = s.myScore;
		individualScores = s.individualScores;
	}	
	return *this;
}

int Set::size() const{
	return mySet.size();
}
Specie * Set::get(int i) const{
	assert(i >= 0 && i < size());
	return mySet[i];
}
void Set::insert(Specie * s, double individualScore){
	//keep the ordering
	bool added = false;
	std::vector<double>::iterator where = individualScores.begin();
	for (std::vector<Specie *>::iterator iter = mySet.begin(); iter != mySet.end() && added == false; iter++){
		if (s->getGeneUID() == (*iter)->getGeneUID()){
			added = true;
		}
		else if (s->getGeneUID() < (*iter)->getGeneUID()){
			mySet.insert(iter,s);
			individualScores.insert(where,individualScore);
			added = true;
		}
		where++;
	}
	if (!added){
		mySet.push_back(s);	
		individualScores.push_back(individualScore);
	}
}

std::string Set::toIV() const{
  std::ostringstream cout;
  cout << "<";
  if (mySet.size() > 0){
    for (int i = 1; i < Specie::getNumSpecie(); i++){
      if (containsSpecieID(i)){
        if (getIndividualScore(i) < 0){
          cout << "r";
        }
        else{
          cout << "a";
        }
      }
      else{
        cout << "n";
      }
    }
  }
  cout << ">";
  return cout.str();
}

std::string Set::toIndividualIV() const{
  std::ostringstream cout;
  if (mySet.size() > 0){
    for (int j = 0; j < size(); j++){
      int c = mySet[j]->getGeneUID();
      cout << "<";
      for (int i = 1; i < Specie::getNumSpecie(); i++){
        if (i == c){
          if (getIndividualScore(i) < 0){
            cout << "r";
          }
          else{
            cout << "a";
          }
        }
        else{
          cout << "n";
        }
      }
      cout << "> " << fabs(getIndividualScore(mySet[j]->getGeneUID())) << " ";
    }
  }
  return cout.str();
}

bool Set::containsSpecieID(const int s1) const{
	bool found = false;
	for (int j = 0; j < size()  && !found; j++){
		if (s1 == get(j)->getGeneUID()){
			return true;
		}
	}
	return false;
}

bool Set::contains(const Set& s1) const{
	//as the list is sorted, we can keep the current position in the array
	int current_me = 0;
	for (int i = 0; i < s1.size(); i++){
		bool found = false;
		for (int j = current_me; j < size()  && !found; j++){
			if (*s1.get(i) == *get(j)){
				found = true;
			}
			current_me++;
		}
		if (found == false){
			return false;	
		}
	}	
	return true;
}


Set unionIt(const Set& s1,const Set& s2){
	Set newSet;
	int i = 0;
	int j = 0;
	//As the sets are sorted, add things piecewise, keeping them sorted
	//The same specie could be in both sets, we want to 'remove' it
	while(i < s1.size() || j < s2.size()){
		if (i >= s1.size()){
			newSet.insert(s2.get(j), s2.getIndividualScore(s2.get(j)->getGeneUID()));
			j++;
		}
		else if (j >= s2.size()){
			newSet.insert(s1.get(i), s1.getIndividualScore(s1.get(i)->getGeneUID()));
			i++;
		}
		else{
			int s1_id = s1.get(i)->getGeneUID();
			int s2_id = s2.get(j)->getGeneUID();
			if (s1_id < s2_id){
				newSet.insert(s1.get(i), s1.getIndividualScore(s1.get(i)->getGeneUID()));
				i++;
			}
			else if (s2_id < s1_id){
				newSet.insert(s2.get(j), s2.getIndividualScore(s2.get(j)->getGeneUID()));
				j++;
			}
			else{
				newSet.insert(s1.get(i), s1.getIndividualScore(s1.get(i)->getGeneUID()));
				i++;
				j++;
			}
		}
	}
	return newSet;
}

bool operator == (const Set& s1, const Set& s2){
	if(s1.size() != s2.size()){
		return false;	
	}
	if (!(s1.getScore() + Set__DELTA > s2.getScore() && s2.getScore() + Set__DELTA > s1.getScore())){
		return false;	
	}
	for (int i = 0; i < s1.size(); i++){
		if (s1.mySet[i] != s2.mySet[i]){
			return false;
		}
	}
	return true;
}
bool operator != (const Set& s1, const Set& s2){
	return ! (s1 == s2);
}

bool operator <  (const Set& s1, const Set& s2){
	//order the sets based on size (so the contains function works easier, then on the values of their variables, then on score
	if (s1.size() < s2.size()){
		return true;
	}
	else if (s1.size() > s2.size()){
		return false;
	}
	for (int i = 0; i < s1.size(); i++){
		int one = s1.get(i)->getGeneUID();
		int two = s2.get(i)->getGeneUID();
		if (one < two){
			return true;
		}
		else if (one > two){
			return false;	
		}
	}
	if (fabs(s1.getScore()) + Set__DELTA < fabs(s2.getScore())){
		return true;	
	}
	else if (fabs(s1.getScore()) > fabs(s2.getScore()) + Set__DELTA){
		return false;
	}
	//they are the same
	return false;
}
bool operator >  (const Set& s1, const Set& s2){
	if (s1 < s2 || s2 == s1){
			return false;
	}
	return true;
}

std::ostream & operator << (std::ostream& cout, const Set & source){
	if (source.size() < 2){
		cout << "{ " << source.getScore() << ", ";
		for ( int i = 0; i < source.size(); i++){
			cout << " " << source.getIndividualScore(source.get(i)->getGeneUID()) << " " << *source.get(i) << ", ";
		}
		cout << "}";
	}
	else{
		cout << "{ " << source.getScore() << ", ";
		for ( int i = 0; i < source.size(); i++){
			cout << "\t" << source.getIndividualScore(source.get(i)->getGeneUID()) << " " << *source.get(i) << "\n";
		}
		cout << "}";
	}
	return cout;
}


Set operator -(const Set& s1,const Set& s2){
	Set newSet;
	int i = 0;
	int j = 0;
	//As the sets are sorted, add things piecewise, keeping them sorted
	//The same specie could be in both sets, we want to 'remove' it
	while(i < s1.size()){
		if (j >= s2.size()){
			newSet.insert(s1.get(i), s1.getIndividualScore(s1.get(i)->getGeneUID()));
			i++;
		}
		else{
			int s1_id = s1.get(i)->getGeneUID();
			int s2_id = s2.get(j)->getGeneUID();
			if (s1_id < s2_id){
				newSet.insert(s1.get(i), s1.getIndividualScore(s1.get(i)->getGeneUID()));
				i++;
			}
			else if (s2_id < s1_id){
				j++;
			}
			else{
				i++;
				j++;
			}
		}
	}
	return newSet;
}

