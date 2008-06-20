#include "Experiments.h"
#include "Specie.h"
#include <cassert>
#include <iostream>
#include <algorithm>

extern int DEBUG_LEVEL;

Experiments::Experiments()
{
	experiments = new vector<vector<vector<double>*>*>();
}
Experiments::Experiments(const Experiments & exp){
	fillExperiments(exp);
}

Experiments::~Experiments()
{
	deleteExperiments();
}

void Experiments::deleteExperiments(){
	while(experiments->size() > 0 ){
		while (experiments->at(experiments->size()-1)->size() > 0 ){
			vector<double> * p = experiments->at(experiments->size()-1)->back();
			experiments->at(experiments->size()-1)->pop_back();
			delete p;
		}
		vector<vector<double>*> * p = experiments->back();
		experiments->pop_back();
		delete p;
	}
	delete experiments;	
}

const Experiments & Experiments::operator=(const Experiments & e){
	if (this != &e){
		deleteExperiments();
		fillExperiments(e);
	}	
	return *this;
}

void Experiments::fillExperiments(const Experiments & exp){
	experiments = new vector<vector<vector<double>*>*>();
	for (int i = 0; i < (int)exp.experiments->size(); i++){
		vector<vector<double>*> * first = exp.experiments->at(i);
		vector<vector<double>*> * a = new vector<vector<double>*>();
		experiments->push_back(a);
		for (int j = 0; j < (int)first->size(); j++){
			vector<double> * second = first->at(j);
			vector<double> * b = new vector<double>;
			a->push_back(b);
			for (int k = 0; k < (int)second->size(); k++){
				b->push_back(second->at(k));
			}
		}
	}
}

bool Experiments::addTimePoint(int experiment, int row, int column, double data){
        //cout << "Trying to add point: e,r,c " << experiment << " " << row << " " << column << " " << data << endl;
	//disallow negative points
	//if (experiment < 0 || row < 0 || column < 0 || data < 0){
	//	return false;	
	//}
	//only allow one experiment to be added at a time, but can add to same exp
	if (experiment != (int)experiments->size() && experiment != (int)experiments->size()-1){
		return false;	
	}
	
	if (experiment == (int)experiments->size()){
		//check that all the data for each row is added
		if (experiment > 0){
			if (experiments->at(0)->at(0)->size() != experiments->back()->back()->size()){
				return false;
			}
		}
		experiments->push_back(new vector<vector<double>*>());
		//cout << "Adding an experiment: ";
	}
	vector<vector<double>*>* exp = experiments->at(experiment);
	
	//only allow one row to be added at a time, but can add to same row
	if (row != (int)exp->size() && row != (int)exp->size()-1){
		return false;	
	}
	if (row == (int)exp->size()){
		//cannot add a new row if the old row wasn't entirely filled in
		if (row > 0){
			if (experiments->at(0)->at(0)->size() != experiments->back()->back()->size()){
				return false;
			}
		}
		exp->push_back(new vector<double>());	
		//cout << "Adding a row: ";
	}
	
	vector<double> * r = exp->at(row);
	
	//only allow one int to be added at a time
	while (column >= (double)r->size()){
	  r->push_back(data);
	  //return false;	
	}
	r->at(column)=data;
	//cout <<"Adding Data: "<< data << endl;
	return true;
}
double Experiments::getTimePoint(int experiment, int row, int column){
	//disallow negative points
	if (experiment < 0 || row < 0 || column < 0){
		return -1;	
	}
	//make sure that we have the experiment, row and column
	if (experiment >= (int)experiments->size()){
		return -1;	
	}
	if (row >= (int)experiments->at(experiment)->size()){
		return -1;	
	}
	if (column >= (int)experiments->at(experiment)->at(row)->size()){
		return -1;	
	}
	return experiments->at(experiment)->at(row)->at(column);
	
}



std::vector<double> Experiments::getSortedValues(int column){
	std::vector<double> v;
	//std::cout << "There are " << (int)experiments->size() << " experiments\n";
	for (int i = 0; i < (int)experiments->size(); i++){
		vector<vector<double>*>* exp = experiments->at(i);
		for (int row = 0; row < (int)exp->size(); row++){
			vector<double> * r = exp->at(row);
			//make sure that s can be found in EVERY experiment
			if (column < 0 || column >= (int)r->size()){
				std::cout << "ERROR: for some reason this species is not found in every experiment\n";
				std::vector<double> empty;
				return empty;
			}
			else{
				v.push_back(r->at(column));
			}
		}
	}
	//std::cout << "There are " << (int)v.size() << "experiments\n";
	sort(v.begin(), v.end());
	return v;
}

std::vector<double> * Experiments::getRow(int experiment, int row){
	std::vector<double> * v = NULL;
	if (0 <= experiment && experiment < (int)experiments->size()){
		vector<vector<double>*>* exp = experiments->at(experiment);
		if (0 <= row && row < (int)exp->size()){
			return exp->at(row);
		}
	}
	return v;
}

int Experiments::totalExperiments(){
	return (int)experiments->size();
}
int Experiments::totalRows(int experiment){
	if (experiment < 0 || experiment >= (int)experiments->size()){
		return 0;	
	}
	else{
		return (int)experiments->at(experiment)->size();	
	}
}


//TODO: work with mutations
Experiments Experiments::removeMutations(Specie * s){
  if (DEBUG_LEVEL > 0.5){
    cout << "Remove mutations not implemented\n";
  }
	return *this;
}

