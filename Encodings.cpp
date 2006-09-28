#include "Encodings.h"
#include "Specie.h"
#include "TSDPoint.h"
#include <iostream>
#include <sstream>

extern int DEBUG_LEVEL;

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
	while(levels.size() > 0){
		std::vector<float> * f = levels.back();
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

std::vector<float> Encodings::getLevels(const Specie * s) const{
	if (s == NULL || s->getGeneUID() >= (int)levels.size()){
		std::vector<float> f;
		return f;	
	}
	return *levels.at(s->getGeneUID());
}


bool Encodings::useBins(int numBins){
	if(numBins > 9){
		cout << "TOO Many BINS!  We need to use 2 string slots in key for more bins and this is not implemented\n";
		return false;	
	}
	else if (s == NULL || e == NULL || numBins <= 0){
		cout << "ERROR: We have some NULLS " << s << " " << e << " " << numBins << "\n";
		return false;	
	}
	clearLevels();
	//cout << "there are a total of " << s->size() << "\n";
	for (int i = 0; i <= s->size(); i++){
		std::vector<float> v = e->getSortedValues(i);
		int current = 0;
		int left = v.size();
		int binSize = (int)left/numBins;
		int seen = 0;
		int levelsAssigned = 0;
		if (DEBUG_LEVEL > 0){
			cout << "\tAssing levels for specie " << i << " numBins " << numBins << ", Bin size: " << binSize << ", Amount left " << left << ", Amount seen " << seen << "\n";
		}
		while(levelsAssigned < numBins-1 && current < (int)v.size()){
			seen++; //seen is one more than current, as [0] is 1 'seen'
			if (DEBUG_LEVEL > 1){
				cout << "\t\tAmount Seen " << seen << ", bin Size:" << binSize << ", current value: " << v.at(current) << "\n";
			}
			//ramp up the floats that are the same in this round, watching out for the end
			while (current < ((int)v.size())-2 && v.at(current) == v.at(current+1)){
				if (DEBUG_LEVEL > 1){
					cout << "\t\t\t" << current << "<" << ((int)v.size())-2 << "&& " << v.at(current) << "==" << v.at(current+1) << "\n";
				}
				seen++;	
				current++;
			}
			if (seen >= binSize){
				if (DEBUG_LEVEL > 0){
					cout << "\t\t\tFound a bin at v[" << current << "], value: " << v.at(current) <<"\n";
				}
				//add in a new level vector if there isn't one
				if ((int)levels.size() == i){
					//cout << "Adding a vector at" << levels.size() << "\n";
					levels.push_back(new std::vector<float>());
				}
				std::vector<float> * f = levels.at(i);
				if (DEBUG_LEVEL > 0){
					cout << "\t\tAssigning level " << (int)f->size() << " for specie " << i << " at " << v.at(current) << "\n";
				}
				f->push_back(v.at(current));
				levelsAssigned++;
				seen = 0;
				//base the new bin size on how many results are left
				left = v.size() - current;
				binSize = (int)left / (numBins - levelsAssigned);
			}
			current++;
		}		
		//we ran out of data, too many bins
		if (current >= (int)v.size()){
			cout << "ERROR: Too many bins for the data available for specie " << i << "\n";
			return false;
		}
	}
	//Print the levels
	for (int i = 0; i < (int)levels.size(); i++){
		Specie * p = Specie::getInstance("??",i);
		std::cout << "Levels for " << *p << " are: ";
		std::vector<float> * v = levels.at(i);
		for (int j = 0; j < (int) v->size(); j++){
			std::cout << " " << v->at(j);
		}
		std::cout << "\n";
	}
	fillTSD();
	return true;
}

bool Encodings::useNumbers(int numBins){
	if(numBins > 9){
		cout << "TOO Many BINS!\n";
		return false;	
	}
	else if (s == NULL || e == NULL || numBins <= 0){
		cout << "ERROR: We have some NULLS " << s << " " << e << " " << numBins << "\n";
		return false;	
	}
	clearLevels();
	for (int i = 0; i <= s->size(); i++){
		std::vector<float> v = e->getSortedValues(i);
		if (v.size() < 2){
			cout << "UNABLE TO USE EXP " << i << " The size is too small " << v.size() << "\n";
		}
		else{
			float top = v.back();
			float bottom = v.at(0);
			float binSize = (top-bottom) / numBins;
			levels.push_back(new std::vector<float>());
			std::vector<float> * f = levels.at(i);
			for (int i = 1; i < numBins; i++){
				f->push_back(bottom+i*binSize);
			}
		}
	}
	fillTSD();
	return true;
	
}

void Encodings::fillTSD(){
	TSDPoint::clearAll();
	TSDPoint::InitializePoints();
	for (int i = 0; i < e->totalExperiments(); i++){
		for (int j = 0; j < e->totalRows(i) - t->getWindowSize(); j++){
			std::vector<float> * current = e->getRow(i, j);
			std::vector<float> * next = e->getRow(i, j+t->getWindowSize());
			std::vector<string> encoded;

			//Remember, that species doesn't include the first 'time' column
			int numColumns = s->size()+1;
			int * seen = new int[numColumns];
			int * rose = new int[numColumns];

			//get the current state of the system
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
			//calculate if things rose
			//cout << "At state " << state.str() << "\n";
			for (int k = 0; k < numColumns; k++){
				seen[k] = 1;
				rose[k] = 0;
				if (current->at(k)+t->getRisingAmount() <= next->at(k)){
					rose[k] = 1;
				}
			}
			TSDPoint * p = TSDPoint::getTSDPoint(state.str());
			p->updateValue(seen,rose,numColumns);
			delete [] seen;
			delete [] rose;
		}	
	}
}


float Encodings::getProb(const Specie * child, const std::vector<int> * l1, const std::vector<int> * l2) const{
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
	float prob = TSDPoint::calculateProbability(child->getGeneUID(),v, size, maxSize);
	delete [] v;
	delete [] size;
	return prob;
}


