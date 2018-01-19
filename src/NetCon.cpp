#include "NetCon.h"
#include "Set.h"
#include "DoubleSet.h"
#include "Specie.h"
#include <cmath>
#include <cassert>

NetCon::NetCon()
{
}

NetCon::NetCon(const NetCon & n){
	fillConnections(n);	
}

NetCon::~NetCon()
{
	deleteConnections();
}

void NetCon::fillConnections(const NetCon & n){
	deleteConnections();
	for (int i = 0; i < (int)n.myConnections.size(); i++){
		myConnections.push_back(new DoubleSet(*n.myConnections.at(i)));		
	}
}
void NetCon::deleteConnections(){
	while((int)myConnections.size() > 0){
		DoubleSet * d = myConnections.back();
		myConnections.pop_back();
		delete d;	
	}
}

const NetCon & NetCon::operator=(const NetCon & s){
	if (this != &s){
		deleteConnections();
		fillConnections(s);
	}	
	return *this;
}


void NetCon::unionIt(const Set & mySet, int TYPE, const Specie& s, double score){
	Set s2(mySet);
	s2.setScore(-1,score);
	if (s.getGeneUID() < 0 || TYPE <= 0 || TYPE >= MAX_TYPE_NUMBER){
          if (DEBUG_LEVEL>0){
            std::cout << "ERROR, UnionIt failed because of bad type\n";
          }
          return;
	}
	for (int j = myConnections.size(); j <= s.getGeneUID(); j++){
		myConnections.push_back(new DoubleSet());	
	}
	myConnections.at(s.getGeneUID())->unionIt(s2);
}

bool NetCon::addIfScoreBetterThanSubsets(const Specie & s, const Set & set){
	if (s.getGeneUID() < 0 || s.getGeneUID() >= (int)myConnections.size()){
          if (DEBUG_LEVEL>0){
            std::cout << "ERROR, AddIfScoreLargerThanSubsets failed because of bad specie\n";
          }
          return false;
	}
	return myConnections.at(s.getGeneUID())->addIfScoreBetterThanSubsets(set);
}

bool NetCon::containsAnyEdges(const Specie& s){
	if (s.getGeneUID() < 0 || s.getGeneUID() >= (int)myConnections.size()){
		return false;	
	}
	if(myConnections[s.getGeneUID()]->size() > 0){
		return true;	
	}
	return false;
}

Set NetCon::getSingleParentsFor(const Specie & s){
	Set newSet;
	if (s.getGeneUID() < 0 || s.getGeneUID() >= (int)myConnections.size()){
		return newSet;	
	}
	DoubleSet * ds = myConnections.at(s.getGeneUID());
	//std::cout << "There are " << (int)ds->size() << " things\n";
	for (int i = 0; i < ds->size(); i++){
		Set * p_set = ds->get(i);
		//std::cout << "There are " << (int)p_set->size() << " or " << *p_set << "\n";
		if (! (p_set->size() == 0)){ //There could be no connection between parents at this spot
			assert (p_set->size() == 1);  //Get single parents should not work with more parents
			Specie * p = p_set->get(0);
			newSet.insert(p,0.001);
		}
	}
	return newSet;
}
DoubleSet * NetCon::getParentsFor(const Specie & s){
	if (s.getGeneUID() < 0){
		return NULL;	
	}
	while( s.getGeneUID() + 1 > (int)myConnections.size()){
		myConnections.push_back(new DoubleSet());	
	}
	return myConnections.at(s.getGeneUID());
}

std::string NetCon::removeSubsets(const Specie & s){
	if (s.getGeneUID() < 0 || s.getGeneUID() >= (int)myConnections.size()){
          if (DEBUG_LEVEL>0){
            std::cout << "ERROR: Removing subsets for an invalid set\n";
          }
          return "";
	}
	return myConnections.at(s.getGeneUID())->removeSubsets();

}

std::string NetCon::filterByScore(const Specie & s,double f){
	if (s.getGeneUID() < 0 || s.getGeneUID() >= (int)myConnections.size()){
          if (DEBUG_LEVEL>0){
            std::cout << "ERROR: Removing subsets for an invalid set\n";
          }
          return "";
	}
	return myConnections.at(s.getGeneUID())->filterByScore(f);
}

int NetCon::totalParents(const Specie& s){
	if (s.getGeneUID() < 0 || s.getGeneUID() >= (int)myConnections.size()){
		return 0;
	}
	return myConnections.at(s.getGeneUID())->size();
}

std::string NetCon::removeLosers(const Specie & s, const DoubleSet& s1, double * scores, std::ostringstream &contenders){
  std::ostringstream cs;
  if (s.getGeneUID() < 0 || s.getGeneUID() >= (int)myConnections.size()){
    return cs.str();
  }
  //find the winner
  double max = -1;
  std::ostringstream winner;
  for (int i = 0; i < s1.size(); i++){
    if (fabs(scores[i]) > max){
      winner.str("");
      max = fabs(scores[i]);
      winner << s1.get(i)->toIV() << " " << max;
    }
  }
  if (DEBUG_LEVEL > COMPETITION_LOG){
    cs << "One Competition\n";
  }
  for (int i = 0; i < s1.size(); i++){
    if (fabs(fabs(scores[i])-max) > 0.00001 || max < 0.00001){
      if (DEBUG_LEVEL > COMPETITION_LOG){
        cs << "Parents " << (*s1.get(i)) << " failed with " << scores[i] << "\n";
      }
      contenders << "\t" << s1.get(i)->toIV() << " " << fabs(scores[i]) << " case 7 " << winner.str() << "\n";
      myConnections.at(s.getGeneUID())->remove(*s1.get(i));
    }
    else{
      if (DEBUG_LEVEL > COMPETITION_LOG){
        cs << "Parents " << (*s1.get(i)) << " beat with " << scores[i] << "\n";
      }			
    }
  }
  return cs.str();
}
std::ostream& operator << (std::ostream& cout, const NetCon & source){
	cout << "Net Con\n";
	for (int i = 1; i < (int)source.myConnections.size(); i++){
		Specie * p = Specie::getInstance("??",i);
		cout << "\tChild " << *p << " has " << *source.myConnections[i] << "\n";
	}
	cout << "End Net Con\n";
	return cout;
}

