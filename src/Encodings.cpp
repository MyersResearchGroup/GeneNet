#include "Encodings.h"
#include "Specie.h"
#include "TSDPoint.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>

extern int DEBUG_LEVEL;

using namespace std;

Encodings::Encodings()
{
	s = NULL;
	e = NULL;
	t = NULL;
}
void Encodings::initialize(Species * species, Experiments * experiments, Thresholds * thresholds){
	s = species;
	e = experiments;
	t = thresholds;
	TSDPoint::clearAll();
	return;
}

void Encodings::clearLevels(){
  if (DEBUG_LEVEL > 0.5){
    cout << "Clearing Levels\n";
  }
	while(levels.size() > 0){
		std::vector<double> * f = levels.back();
		levels.pop_back();
		delete f;
	}
	TSDPoint::clearAll();
}

Encodings::~Encodings()
{
	clearLevels();
}

int Encodings::totalSpecies() const{
	return s->size();
}

std::vector<double> Encodings::getLevels(const Specie * s) const{
	if (s == NULL || s->getGeneUID() >= (int)levels.size()){
		std::vector<double> f;
		return f;	
	}
	return *levels.at(s->getGeneUID());
}

Specie * Encodings::getSpecie(int i) const{
  if (i < 0 || i > totalSpecies()){
    return NULL;	
  }
  return s->get(i);
}

int Encodings::getMaxLevel(const Specie * s) const{
  if (s == NULL || s->getGeneUID() >= (int)levels.size()){
    return -1;
  }
  return (int)levels.at(s->getGeneUID())->size();
}


bool Encodings::useBins(int oldNumBins, bool useSpeciesLevels, bool succ, bool pred){
        int numBins = oldNumBins;
	if(numBins > 9){
		cout << "TOO Many BINS!  We need to use 2 string slots in key for more bins and this is not implemented\n";
		return false;	
	}
	else if (s == NULL || e == NULL || numBins <= 0){
		cout << "ERROR: We have some NULLS " << s << " " << e << " " << numBins << "\n";
		return false;	
	}
        int * oldLevels = new int[s->size()+1];
        if (useSpeciesLevels){
          for (int i = 0; i <= s->size() && i < (int) levels.size(); i++){
            oldLevels[i] = ((int) (levels.at(i))->size())+1;
          }
        }
	clearLevels();
	for (int i = 0; i <= s->size(); i++){
          if (DEBUG_LEVEL > 1){
            cout << "Assigning values for species " << i << "\n";
          }
          if (useSpeciesLevels){
            numBins = oldLevels[i];
          }
		std::vector<double> v = e->getSortedValues(i);
		int current = 0;
		double left = v.size();
		double binSize = left/numBins;
		int seen = 0;
		int levelsAssigned = 0;
		if (DEBUG_LEVEL > 0){
			cout << "\tAssigning levels for specie " << i << " numBins " << numBins << ", Bin size: " << binSize << ", Amount left " << left << ", Amount seen " << seen << "\n";
		}
		while(levelsAssigned < numBins-1 && current < (int)v.size()){
			seen++; //seen is one more than current, as [0] is 1 'seen'
			//ramp up the doubles that are the same in this round, watching out for the end
			while (current < ((int)v.size())-2 && v.at(current) == v.at(current+1)){
				if (DEBUG_LEVEL > 3){
					cout << "\t\t\t" << current << " < " << ((int)v.size())-2 << " && " << v.at(current) << "==" << v.at(current+1) << "\n";
				}
				seen++;	
				current++;
			}
			if (DEBUG_LEVEL > 1){
				cout << "\t\tAmount Seen " << seen << ", bin Size:" << binSize << ", current value: " << v.at(current) << "\n";
			}
			if (seen > binSize){
				if (DEBUG_LEVEL > 0){
					cout << "\t\t\tFound a bin at v[" << current << "], value: " << v.at(current) <<"\n";
				}
				//add in a new level vector if there isn't one
				while ((int)levels.size() <= i){
					//cout << "Adding a vector at" << levels.size() << "\n";
					levels.push_back(new std::vector<double>());
				}
				std::vector<double> * f = levels.at(i);
				if (DEBUG_LEVEL > 0){
					cout << "\t\tAssigning level " << (int)f->size() << " for specie " << i << " at " << v.at(current) << "\n";
				}
				f->push_back(v.at(current));
				levelsAssigned++;
				seen = 0;
				//base the new bin size on how many results are left
				left = v.size() - (current+1);
				binSize = left / (double)(numBins - levelsAssigned);
				if (DEBUG_LEVEL > 0){
					cout << "\t\tNew bin Size:" << binSize << " = " << left << " / (" << numBins << " - " << levelsAssigned << ")\n";
				}
			}
			current++;
		}		
		//we ran out of data, too many bins
		if (current >= (int)v.size()){
		  if (DEBUG_LEVEL > 0) {
			cout << "ERROR: Too many bins for the data available for specie " << i << "\n";
                        cout << "\tIt will get some null values but we will try to push on\n";
		  }
			//return false;
		  int j = 9990;
                        while (levelsAssigned < numBins-1){
                          //add in a new level vector if there isn't one
                          if ((int)levels.size() == i){
                            //cout << "Adding a vector at" << levels.size() << "\n";
                            levels.push_back(new std::vector<double>());
                          }
                          std::vector<double> * f = levels.at(i);
                          if (DEBUG_LEVEL > 0){
                            cout << "\t\tAssigning level " << (int)f->size() << " for specie " << i << " at " << j << "\n";
                          }
                          f->push_back(j);
                          j++;
                          levelsAssigned++;
                        }
		}
	}
        if (DEBUG_LEVEL > 0.5){
          printLevels();
        }
	fillTSD(succ, pred);
        delete [] oldLevels;
	return true;
}

void Encodings::printLevels(){
	//Print the levels
	for (int i = 0; i < (int)levels.size(); i++){
		Specie * p = Specie::getInstance("??",i);
		std::cout << "Levels for " << *p << " are: ";
		std::vector<double> * v = levels.at(i);
		for (int j = 0; j < (int) v->size(); j++){
			std::cout << " " << v->at(j);
		}
		std::cout << "\n";
	}
}
bool Encodings::useFile(ifstream & lvl_file, bool checkOrdering, bool succ, bool pred){
	clearLevels();
	for (int i = 0; i <= totalSpecies(); i++){
	  levels.push_back(new std::vector<double>());
	}
	for (int i = 0; i <= totalSpecies(); i++){
	        //levels.push_back(new std::vector<double>());
		std::vector<double> * f = NULL; // = levels.at(i);
		string name;
		int num_levels;
		double tmp;
		char c;
		lvl_file >> name;
		name = name.substr(0,name.size()-1); // remove the , at the end
		for (int j = 0; j <= totalSpecies(); j++){
		  if (name == Specie::getInstance("??",j)->getGeneName()){
		    f = levels.at(j);
		    break;
		  }
		}
		
// 		if (name != Specie::getInstance("??",i)->getGeneName()){
// 			cout << "ERROR: reading file names do not match in lvl file\n";
// 			cout << "'" << name << "' != '" << Specie::getInstance("??",i)->getGeneName() << "'\n";
// 			exit(0);
// 		}
// 		else{
// 			//cout << "Names match " << name << "\n";	
// 		}
		lvl_file >> num_levels;
		if(num_levels > 9){
			cout << "TOO Many BINS!\n";
			return false;	
		}
		else if (num_levels > 1){
			for (int i = 1; i < num_levels; i++){
				lvl_file >> c;
				lvl_file >> tmp;
				//cout << "\tread " << tmp << "\n";
				if (i > 1){
					if (f->at(((int)f->size())-1) < tmp || !checkOrdering){
						f->push_back(tmp);
					}
					else{
						cout << "ERROR: levels are not ordered for " << name << " " << f->at(((int)f->size())-1) << " !< " << tmp << "\n";
						return false;
					}
				}
				else{
					f->push_back(tmp);
				}
			}
			if ((int)f->size() + 1 != num_levels){ //1 less level barier than levels
				cout << "ERROR: read in an incorect number of levels " << (int)f->size() << " not " << num_levels << "\n";
				return false;
			}
		}
	}
        if (DEBUG_LEVEL > 0.5){
          printLevels();
        }
	fillTSD(succ, pred);
	return true;
}

bool Encodings::useNumbers(int oldNumBins, bool useSpeciesLevels, bool succ, bool pred){
        int numBins = oldNumBins;
	if(numBins > 9){
		cout << "TOO Many BINS!\n";
		return false;	
	}
	else if (s == NULL || e == NULL || numBins <= 0){
		cout << "ERROR: We have some NULLS " << s << " " << e << " " << numBins << "\n";
		return false;	
	}
        int * oldLevels = new int[s->size()+1];
        if (useSpeciesLevels){
          for (int i = 0; i <= s->size() && i < (int) levels.size(); i++){
            oldLevels[i] = ((int) (levels.at(i))->size())+1;
          }
        }
	clearLevels();
	for (int i = 0; i <= s->size(); i++){
          if (useSpeciesLevels){
            numBins = oldLevels[i];
          }
		std::vector<double> v = e->getSortedValues(i);
		if (v.size() < 2){
			cout << "UNABLE TO USE EXP " << i << " The size is too small " << v.size() << "\n";
		}
		else{
			double top = v.back();
			double bottom = v.at(0);
			double binSize = (top-bottom) / numBins;
                        while ((int)levels.size() <= i){
                          levels.push_back(new std::vector<double>());
                        }
			std::vector<double> * f = levels.at(i);
			for (int i = 1; i < numBins; i++){
				f->push_back(bottom+i*binSize);
			}
		}
	}
	fillTSD(succ, pred);
        delete [] oldLevels;
	return true;
	
}


void Encodings::fillTSD(bool succ, bool pred){
  if (succ == false && pred == false){
    cout << "ERROR: unable to calculate probabilities as both SUCC and PRED sets are empty\n";
    exit(0);
  }
  TSDPoint::clearAll();
  TSDPoint::InitializePoints();
  for (int i = 0; i < e->totalExperiments(); i++){
    for (int j = 0; j < e->totalRows(i) - t->getWindowSize(); j++){
      std::vector<double> * current = e->getRow(i, j);
      std::vector<double> * next = e->getRow(i, j+t->getWindowSize());
      //std::vector<string> encoded;
      
      //Remember, that species doesn't include the first 'time' column
      int numColumns = s->size()+1;
      int * seen = new int[numColumns];
      int * rose = new int[numColumns];
      
      //calculate if things rose
      //cout << "At state " << state.str() << "\n";
      for (int k = 0; k < numColumns; k++){
        seen[k] = 1;
        rose[k] = 0;
        if (current->at(k) /*+t->getRisingAmount()*/ < next->at(k)){
          rose[k] = 1;
        }
      }
      if (succ){ //get the current state of the system
        ostringstream state;
        state << "-";
        for (int k = 1; k < numColumns; k++){
          bool found = false;
          for (int l = 0; l < (int)levels.at(k)->size() && found == false; l++){
            //match the value to the level
            if (current->at(k) <= levels.at(k)->at(l)){
              found = true;
              state << l;
            }
          }
          if (!found){
            state << (int)levels.at(k)->size();
          }
        }
        TSDPoint * p = TSDPoint::getTSDPoint(state.str());
        p->updateValue(seen,rose,numColumns);
      }
      if (pred){ //get the next state of the system
        ostringstream state;
        state << "-";
        for (int k = 1; k < numColumns; k++){
          bool found = false;
          for (int l = 0; l < (int)levels.at(k)->size() && found == false; l++){
            //match the value to the level
            if (next->at(k) <= levels.at(k)->at(l)){
              found = true;
              state << l;
            }
          }
          if (!found){
            state << (int)levels.at(k)->size();
          }
        }
        TSDPoint * p = TSDPoint::getTSDPoint(state.str());
        p->updateValue(seen,rose,numColumns);
      }
      
      delete [] seen;
      delete [] rose;
    }	
  }
}


double Encodings::getProb(const Specie * child, const std::vector<int> * l1, const std::vector<int> * l2) const{
	//look into the tsd files at the child's probabilities
	//merge 'unused' parents into the required set

	//make sure that things match
	if (levels.size() != l1->size() && levels.size() != l2->size()){
		return -1;	
	}
	int maxSize = levels.size();
	int * v = new int[maxSize];
	int * size = new int[maxSize];
	for (int i = 0; i < maxSize; i++){
		size[i] = ((int)levels.at(i)->size())+1;
		if (l1->at(i) != -1){
			v[i] = l1->at(i);
		}
		else if (l2->at(i) != -1){
			v[i] = l2->at(i);
		}
		else{
			v[i] = -1;
		}
	}
	double prob = TSDPoint::calculateProbability(child->getGeneUID(),v, size, maxSize);
	delete [] v;
	delete [] size;
	return prob;
}


