#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <cassert>
#include <algorithm>
#include <map>

#include "Specie.h"
#include "Species.h"
#include "Experiments.h"
#include "NetCon.h"
#include "Thresholds.h"
#include "Encodings.h"
#include "Set.h"
#include "LevelAssignments.h"
#include "DoubleSet.h"
#include "GeneNet.h"
#include "TSDPoint.h"
#include "SimpleOpt.h"
#include "SimpleGlob.h"

using namespace std;

const int SPECIES_NAME_SIZE = 100;

int DEBUG_LEVEL = 0;

map<Specie*, map<Set, map<Set, vector<float> > > > * scoreCache = NULL;

#if defined(_MSC_VER)
# include <windows.h>
# include <tchar.h>
#else
# define TCHAR		char
# define _T(x)		x
# define _tprintf	printf
# define _tmain		main
#endif




static void ShowUsage()
{
    _tprintf(
        _T("Usage: GeneNet [OPTIONS] [Dir]\n")
        _T("-d [num] --debug [num]			Sets the debug or output level.  Default 0\n")
        _T("-a [num] --debug [num]			Sets the activation threshold.  Default 1.15\n")
        _T("-r [num] --debug [num]			Sets the repression threshold.  Default 0.75\n")
        _T("-v [num] --debug [num]			Sets how high a score must be to be considered a parent.  Default 0.5\n")
        _T("-wr [num] --windowRisingAmount [num]	Sets how much larger a number must be to be considered as a rise.  Default 1\n")
        _T("-ws [num] --windowSize [num]		Sets how far the TSD points are when compared.  Default 1\n")
        _T("-nb [num] --numBins [num]   		Sets how many bins are used in the evaluation.  Default 3\n")
        _T("-id [num] --influenceLevelDelta [num]	Sets how close parents must be in score to be considered for combination.  Default 0.01\n")
        _T("-rd [num] --relaxIPDelta [num]		Sets how fast the bound is relaxed for a and r if no parents are found in InitialParents, Default 0.025\n")
        );
}

CSimpleOpt::SOption g_rgFlags[] =
{
    { SO_O_EXACT,    _T("--exact"),     SO_NONE },
    { SO_O_NOSLASH,  _T("--noslash"),   SO_NONE },
    { SO_O_SHORTARG, _T("--shortarg"),  SO_NONE },
    { SO_O_CLUMP,    _T("--clump"),     SO_NONE },
    { SO_O_NOERR,    _T("--noerr"),     SO_NONE },
    SO_END_OF_OPTIONS
};

enum { OPT_HELP = 1000 };
CSimpleOpt::SOption g_rgOptions[] =
{
    { OPT_HELP,  _T("-?"),           		SO_NONE    },
    { OPT_HELP,  _T("-h"),           		SO_NONE    },
    { OPT_HELP,  _T("-help"),        		SO_NONE    },
    { OPT_HELP,  _T("--help"),       		SO_NONE    },
    {  1,        _T("-d"),           		SO_REQ_SEP },
    {  2,        _T("--debug"),      		SO_REQ_SEP },
    {  3,        _T("-a"),           		SO_REQ_SEP },
    {  4,        _T("-r"),           		SO_REQ_SEP },
    {  5,        _T("-v"),           		SO_REQ_SEP },
    {  6,        _T("-wr"),           		SO_REQ_SEP },
    {  7,        _T("--windowRisingAmount"),	SO_REQ_SEP },
    {  8,        _T("-ws"),           		SO_REQ_SEP },
    {  9,        _T("--windowSize"),		SO_REQ_SEP },
    { 10,        _T("-nb"),			SO_REQ_SEP },
    { 11,        _T("--numBins"),		SO_REQ_SEP },
    { 12,        _T("-id"),			SO_REQ_SEP },
    { 13,        _T("--influenceLevelDelta"),	SO_REQ_SEP },
    { 14,        _T("-rd"),			SO_REQ_SEP },
    { 15,        _T("--relaxIPDelta"),		SO_REQ_SEP },
    SO_END_OF_OPTIONS
};

string ShowFiles(int argc, TCHAR ** argv) {
    // glob files to catch expand wildcards
    CSimpleGlob glob(SG_GLOB_NODOT|SG_GLOB_NOCHECK);
    if (SG_SUCCESS != glob.Add(argc, argv)) {
        _tprintf(_T("Error while globbing files\n"));
        exit(0);
    }

    for (int n = 0; n < glob.FileCount(); ++n) {
        _tprintf(_T("file %2d: '%s'\n"), n, glob.File(n));
    }
    if (glob.FileCount() > 0){
    	return glob.File(0);	
    }
    else{
      ShowUsage();
      exit(0);
    }
}





void callGeneNet(const char * dir, Thresholds & T){
	Species S;
	Experiments E;
	NetCon C;
	Encodings L;
		
	cout << "Reading the TSD files\n";
	if (! fillFromTSD(dir, &S, &E, &C, &T, &L)){
		return;	
	}
	cout << "Finished Reading the TSD files\n";

	//Remove out the 'time' from the species list
	cout << "Removing the Time specie from the list\n";
	if (S.removeTimeSpecie() == false){
		cout << "ERROR!, unable to find time specie\n";
		return;	
	}
		
	GeneNet(S,E,C,T,L);
	cout << "Exiting and destroying Species, Experiments, NetCon, Thresholds, Encodings\n";
	writeDot(dir, &C);
}

int main(int argc, char* argv[]){
	Thresholds T(1.15,0.75,0.5,1,1,3, 0.01, 0.025,2);
	cout << "GeneNet Arugments:\n";
	for (int i = 0; i < argc; i++){
		cout << "\t" << argv[i] << "\n";	
	}
    // get the flags to use for SimpleOpt from the command line
    int nFlags = SO_O_USEALL;
    CSimpleOpt flags(argc, argv, g_rgFlags, SO_O_NOERR|SO_O_EXACT);
    while (flags.Next()) {
        nFlags |= flags.OptionId();
    }

    // now process the remainder of the command line with these flags
    CSimpleOpt args(flags.FileCount(), flags.Files(), g_rgOptions, nFlags);
    while (args.Next()) {
        if (args.LastError() != SO_SUCCESS) {
            TCHAR * pszError = _T("Unknown error");
            switch (args.LastError()) {
            case SO_SUCCESS:
            	pszError = _T("This should not happen, but keeps a warning away to leave it in\n");
            	break;
            case SO_OPT_INVALID:
                pszError = _T("Unrecognized option");
                break;
            case SO_OPT_MULTIPLE:
                pszError = _T("Option matched multiple strings");
                break;
            case SO_ARG_INVALID:
                pszError = _T("Option does not accept argument");
                break;
            case SO_ARG_INVALID_TYPE:
                pszError = _T("Invalid argument format");
                break;
            case SO_ARG_MISSING:
                pszError = _T("Required argument is missing");
                break;
            }
            _tprintf(
                _T("%s: '%s' (use --help to get command line help)\n"),
                pszError, args.OptionText());
            continue;
        }

        if (args.OptionId() == OPT_HELP) {
            ShowUsage();
            return 0;
        }

        if (args.OptionArg()) {
            _tprintf(_T("option: %2d, text: '%s', arg: '%s'\n"),
                args.OptionId(), args.OptionText(), args.OptionArg());
            switch (args.OptionId()) {
            case 1:
            case 2:
            	DEBUG_LEVEL = atoi(args.OptionArg());
            	cout << "\tSetting Debug Level to '" << DEBUG_LEVEL << "'\n";
            	break;
            case 3:
            	T.setA(atof(args.OptionArg()));
            	cout << "\tSetting a to '" << T.getA() << "'\n";
            	break;
            case 4:
            	T.setR(atof(args.OptionArg()));
            	cout << "\tSetting r to '" << T.getR() << "'\n";
            	break;
            case 5:
            	T.setV(atof(args.OptionArg()));
            	cout << "\tSetting v to '" << T.getV() << "'\n";
            	break;
            case 6:
            case 7:
            	T.setRisingAmount(atoi(args.OptionArg()));
            	cout << "\tSetting windowRisingAmount to '" << T.getRisingAmount() << "'\n";
            	break;
            case 8:
            case 9:
            	T.setWindowSize(atoi(args.OptionArg()));
            	cout << "\tSetting windowSize to '" << T.getWindowSize() << "'\n";
            	break;
            case 10:
            case 11:
            	T.setBins(atoi(args.OptionArg()));
            	cout << "\tSetting numBins to '" << T.getBins() << "'\n";
            	break;
            case 12:
            case 13:
            	T.setInfluenceLevelDelta(atof(args.OptionArg()));
            	cout << "\tSetting influenceLevelDelta to '" << T.getInfluenceLevelDelta() << "'\n";
            	break;
            case 14:
            case 15:
            	T.setRelaxInitialParentsDelta(atof(args.OptionArg()));
            	cout << "\tSetting relaxIPDelta to '" << T.getRelaxInitialParentsDelta() << "'\n";
            	break;
            }
        }
        else {
            _tprintf(_T("option: %2d, text: '%s'\n"),
                args.OptionId(), args.OptionText());
        }
    }

    /* process files from command line */
    string s = ShowFiles(args.FileCount(), args.Files());

    if (s.substr(s.length()-1,1) == "/"){
      s = s.substr(0,s.length()-1);
    }
    callGeneNet(s.c_str(), T);

    //clear things
    cout << "Clearing the species that we created\n";
    Specie::clearAllInstances();
	cout << "Done\n";
	
	
    return 0;
}



bool fillFromTSD(const char dir[], Species * S, Experiments * E, NetCon * C, Thresholds* T, Encodings*L){
	ifstream infile;
	ostringstream oss;
	int i = 1;
	oss << dir << "/run-" << i << ".tsd";
	cout << "Got '" << oss.str() << "' for call\n";
	infile.open(oss.str().c_str());
	while (infile){
		bool f = yaccParse(fopen(oss.str().c_str(),"r"), S, E,i-1);
		if (f == false){
			cout << "Bad time series data file at " << i << "\n";
			assert(false);	
		}
		i++;
		oss.str("");
		oss << dir << "/run-" << i << ".tsd";
		//cout << "Got '" << oss.str() << "' for call\n";
		infile.close();
		infile.open(oss.str().c_str());
	}
	cout << "Read in " << i-1 << " tsd files\n";
	infile.close();
	if (i == 1){
		cout << "\tERROR:, no TSD files found\n";
		return false;
	}
	return true;
}

void GeneNet(Species &S, Experiments &E, NetCon &C, Thresholds &T, Encodings &L){

  cout << "Encoding the experiments\n";
  EncodeExpts(S,E,T,L);
  cout << "Finished encoding\n";

  int ParentSetsRemoved = 0;

  for (int i = 0; i < S.size(); i++){
	scoreCache = new map<Specie*, map<Set, map<Set, vector<float> > > >();
    Specie * s = S.get(i);
    cout << "\nUsing specie " << *s << " as a child\n";

    if (!C.containsAnyEdges(*s)){
      Experiments Ep = E.removeMutations(s);
      SelectInitialParents(*s,S,Ep,C,T,L);
      CreateMultipleParents(*s,S,Ep,C,T,L);
      CompetePossibleParents(*s,S,Ep,C,T,L);
      //post filter based on score
      /*
      DoubleSet * p = C.getParentsFor(S);
      for (int i = 0; i < p->size(); i++){
      	Set * a = p->get(i);
     	if (a->getScore() < T.getPostFilter()){
			ParentSetsRemoved++;
     		p->remove(a);
      	}
      }
      */
    }
	delete scoreCache;
  }
  
  cout << "All Network Connections are\n" << C << "\n";
  cout << "Parent Sets Removed By Post Filter: " << ParentSetsRemoved << "\n";
  cout << "Exiting the GeneNet algorithm\n";
}


void EncodeExpts(Species& S, Experiments& E, Thresholds & T, Encodings& L){
	L.initialize(&S,&E,&T);
	cout << "Filling hashes now\n";
	if (!L.useBins(T.getBins())){
		cout << "ERROR! Bins didn't work\n";
		exit(0);
	}
	//L.useNumbers(3);
	cout << "Finished setting up the hashes\n";
	return;
}


float ScoreBetter(Specie& s, const Set& P, const Set& G, const Experiments& E, const Thresholds& T, const Encodings& L){
  float votesa; float votesr; float votesu;
  votesa = 0; votesr = 0; votesu = 0;


  if ((*scoreCache)[&s][P][G].size() > 0){
  	vector<float> f = (*scoreCache)[&s][P][G];
  	for (int i = 0; i < (int)f.size(); i++){
  		float probRatio = f[i];
  		if (probRatio > T.getA() ){
  			votesa++;
  		}
		else if (probRatio < T.getR()){
			votesr++;
		}
		else{
			votesu++;
		}
  	}
	if (votesa+votesr+votesu < 0.001){
	  return 0;
	}
	return (votesa - votesr)/(votesa+votesr+votesu);
  }
  vector<float> * fillProbVector = &(*scoreCache)[&s][P][G];
  
  Set tmpG(G);
  Set tmpP(P);

  TSDPoint::G = &tmpG;
  TSDPoint::P = &tmpP;
  //cout << "G is " << tmpG << "\n";
  //cout << "P is " << tmpP << "\n";
  sort(TSDPoint::initialValues->begin(), TSDPoint::initialValues->end(), &TSDPoint::sortTSDPoints);

  /*
  for (vector<TSDPoint*>::iterator iter = TSDPoint::initialValues->begin(); iter != TSDPoint::initialValues->end(); iter++){
  		cout << "\t" << (*iter)->rowValues << "\n";
  }
  */

  TSDPoint * base = NULL;
  TSDPoint * next = NULL;
  float rising = 0;
  float seen = 0;
  float nextR = 0;
  float nextS = 0;
  float prob1 = -1;
  float nValue = 0;
  int end = (int)TSDPoint::initialValues->size();
  for (int i = 0; i <= end; i++){
  	//we have to do the very end one for a score
  	if (i < end && base == NULL){
  		base = (*TSDPoint::initialValues)[i];
  		rising = base->risings[s.getGeneUID()];	
  		seen = base->seen[s.getGeneUID()];
  		prob1 = -1;
  		//Find out how many values are at this level for use later
		nValue = seen;
		for (int j = i+1; j < end && ! base->cannotCompareLevels((*TSDPoint::initialValues)[j]); j++){
			nValue += (*TSDPoint::initialValues)[j]->seen[s.getGeneUID()];
		}
		//reset the other values
		next = NULL;
		nextR = 0;
		nextS = 0;
  	}
	else if (i < end && base->sameLevels((*TSDPoint::initialValues)[i])){
  		rising += (*TSDPoint::initialValues)[i]->risings[s.getGeneUID()];	
  		seen   += (*TSDPoint::initialValues)[i]->seen[s.getGeneUID()];
	}
	else if (i < end && next == NULL && base->cannotCompareLevels((*TSDPoint::initialValues)[i])){
		//cout << "\t\t\t\t\tUnable to compare " << base->rowValues << " with " << (*TSDPoint::initialValues)[i]->rowValues << "\n";
		//make sure that we can even compare base to the next one and it hasn't jumped
		base = NULL;
		i--;
	}
	else if (i < end && next == NULL){
  		next = (*TSDPoint::initialValues)[i];
  		nextR = next->risings[s.getGeneUID()];	
  		nextS = next->seen[s.getGeneUID()];
	}
	else if (i < end && next->sameLevels((*TSDPoint::initialValues)[i])){
  		nextR += (*TSDPoint::initialValues)[i]->risings[s.getGeneUID()];	
  		nextS += (*TSDPoint::initialValues)[i]->seen[s.getGeneUID()];
	}
	else{
		//we have valid values, but we have to either start base at this i, or next at this i
		if (i < end){
			i--;
		}
		if (rising == 0 || seen == 0 || (seen <= nValue/10.0f && seen < 20)){
			//don't fill it, but let base start from this value
			if (DEBUG_LEVEL>0){
				cout << "\t\t\t\tBase prob " << base->rowValues << " = " << rising << " / " << seen << " = " << prob1 << " BUT!!\n";
				if (rising == 0 || seen == 0){
					cout << "\t\t\t\t\t\tNot using current as base because = " << rising << " / " << seen;
				}
				else{
					cout << "\t\t\t\t\t\tNot using current as base because of N Value " << rising << " / " << seen << " and nValue " << nValue;
				}
			    cout << " instead trying " << nextR << " / " << nextS << "\n";
			}
			base = next;
			rising = nextR;
			seen = nextS;
			next = NULL;
		}
		else{
			//We have finished with next and just need to calculate the probabilities
			string nextStr = "";
			if (next != NULL){
				nextStr = next->rowValues;
			}
			next = NULL;
			if (prob1 == -1){ // we only have to print this one time until it resets
				prob1 = rising/seen;
				if (DEBUG_LEVEL>0){
					cout << "\t\t\t\tBase prob " << base->rowValues << " = " << rising << " / " << seen << " = " << prob1 << " with nValue of " << nValue << "\n";
				}
			}
			if (nextR == 0 || nextS == 0){
				//cannot compute this difference
				if (DEBUG_LEVEL>0){
				  	cout << "\t\t\t\t\t\tNot A Valid Ratio Because the Next = " << nextR << " / " << nextS << "\n";
				}
			}
			else{
				float probN = nextR/nextS;
				if (DEBUG_LEVEL>0){
					cout << "\t\t\t\tNext prob " << nextStr << " = " << nextR << " / " << nextS << " = " << probN << "\n";
				}
 	
 				float probRatio = probN / prob1;
				if (DEBUG_LEVEL>0){
					cout << "\t\t\t\t\tRatio is " << prob1 << " / " << probN << " = " << probRatio << "\n";
				}
				if (nextS <= nValue/10.0f && nextS < 20){
					//the difference in amount is too large to use
					if (DEBUG_LEVEL>0){
						cout << "\t\t\t\t\t\tHOWEVER, not using it as (" << nextS << " <= " << seen << "/10.0" << " && " << nextS << " < 20)\n";
					}
				}
				else if (probRatio > T.getA() ){
					fillProbVector->push_back(probRatio);
					votesa++;
			    }
			    else if (probRatio < T.getR()){
					fillProbVector->push_back(probRatio);
					votesr++;
			    }
			    else{
					fillProbVector->push_back(probRatio);
					votesu++;
			    }
			}
		}
	 }
  }

  TSDPoint::G = NULL;
  TSDPoint::P = NULL;

  if (DEBUG_LEVEL>0){
	  cout << "\t\tvotes (a,r,u) (" << votesa << " " << votesr << " " << votesu << ") or ";
	  cout << "(" << votesa << " - " << votesr << ")/(" << votesa+votesr+votesu << ")\n";
  }
  if (votesa+votesr+votesu < 0.001){
    return 0;
  }
  return (votesa - votesr)/(votesa+votesr+votesu);
}

float Score(const Specie& s, const Set& P, const Set& G, const Experiments& E, const Thresholds& T, const Encodings& L){
  cout << "TODO: this needs to be fixed so that n_values work correctly\n";
  exit(0);
  float votesa = 0;  float votesr = 0;  float votesu = 0;
  LevelAssignments lp(G,L);
  if (DEBUG_LEVEL>0){
  	cout << "\tScoring using P as: " << P << "\n\t\tand G as " << G << "\n";
  }
  for (int i = 0; i < lp.size(); i++){

    LevelAssignments l(P,L);
    float prob1 = L.getProb(&s,l.get(0),lp.get(i));

    for (int j = 1; j < l.size(); j++){
      float prob2 = L.getProb(&s,l.get(j),lp.get(i));
      float probRatio = prob2 / prob1;

	  if (DEBUG_LEVEL>0){
	  	cout << "\t\t\t\t\tRatio is " << prob1 << " / " << prob2 << " = " << probRatio << "\n";
	  }
	  if (prob1 == -1 || prob2 == -1){
		if (DEBUG_LEVEL>0){
		  	cout << "\t\t\t\t\t\tNot A Valid Ratio\n";
		}
	  }
      else if (probRatio > T.getA() ){
		votesa++;
      }
      else if (probRatio < T.getR()){
		votesr++;
      }
      else{
		votesu++;
      }
    }
  }
  if (DEBUG_LEVEL>0){
  	cout << "\t\tvotes (a,r,u) (" << votesa << " " << votesr << " " << votesu << ") or ";
 	cout << "(" << votesa << " - " << votesr << ")/(" << votesa+votesr+votesu << ")\n";
  }
  return (votesa - votesr)/(votesa+votesr+votesu);
}

void SelectInitialParents (Specie& s, const Species& S, const Experiments& E, NetCon& C, const Thresholds& T, const Encodings& L){
  Thresholds newT(T);
  bool relaxedTheBounds = false;
  while (C.getParentsFor(s)->size() == 0){
  	for (int i = 0; i < S.size(); i++){
  		Specie * p = S.get(i);
  		if (*p != s){
  			cout << "\tTesting specie " << *p << " as a parent\n";
  			float alpha = ScoreBetter(s,*p->toSet(),*s.toSet(),E,newT,L);
  			
  			cout << "\tScore of " << alpha << " and threshold +-" << newT.getV() << "\n";
  			if (alpha >= newT.getV()){
  				cout << "\t\tMeans an activation parent\n";
  				C.unionIt(*p->toSet(),C.ACTIVATION,s,alpha);
  			}
  			else if (alpha <= -newT.getV()){
  				cout << "\t\tMeans a represion parent\n";
  				C.unionIt(*p->toSet(),C.REPRESSION,s,alpha);
  			}
  		}
  	}
  	if (C.getParentsFor(s)->size() == 0){
  		relaxedTheBounds = true;
		newT.relaxInitialParentsThresholds();
  		cout << "There are no parents for " << s << ", relaxing the thresholds to [" << newT.getA() << ", " << newT.getR() << "]\n";
  	}
  }
  if (relaxedTheBounds){
	  cout << "Rescoring parents with harsher numbers, as we relaxed the bounds\n";
	  DoubleSet * rescore = C.getParentsFor(s);
	  for (int i = 0; i < rescore->size(); i++){
	  	  Set * rescoreSet = rescore->get(i);
		  assert(rescoreSet->size() == 1);
	  	  Specie * p = rescoreSet->get(0);
		  float alpha = ScoreBetter(s,*p->toSet(),*s.toSet(),E,T,L);
		  cout << "\tScore of " << alpha << " and threshold +-" << T.getV() << "for " << *p << "\n";
		  rescoreSet->setScore(-1, alpha);
	  }
  }
  //now print out the parents
  cout << "Initial parents for " << s << " are: " <<  *C.getParentsFor(s) << "\n";  	
}

bool incrementBaseSet(int * c, int arraySize, int numberOfBases){
	//move to the left until something increments
	//cout << "Entering base set with size " << arraySize << " numOfBases " << numberOfBases << " current " << *c << "\n";
	int i = arraySize-1;
	bool found = false;
	while(i >= 0 && !found){
		//cout << "Testing if ( " << c[i] << " < " << numberOfBases  << " - (" << arraySize << " - " << i << " )\n";
		//Take the max number that a square can be, then subtract how far in you are, as each
		//square you go in decreases what you can be by 1
		//also, remember that c[i] is a position and not a max #
		if (c[i] < numberOfBases - (arraySize-i)){
			//cout << "\tIt is!\n";
			found = true;
			c[i]++;	
			i++;  // so that we start filling on the next one 
		}
		else{
			//cout << "\tIt is not, sorry\n";
			i--;
		}
	}
	if (found == false){
		//cout << "\tI now have to exit with false, as I seem to not be able to increment anything\n";
		return false;	
	}
	//start filling
	while(i < arraySize){
		c[i] = c[i-1]+1;
		i++;
	}
	return true;
}

void CreateMultipleParents(Specie& s, const Species& S, const Experiments& E, NetCon& C, const Thresholds& T, const Encodings& L){
  Set baseSet = C.getParentsFor(s)->colapseToSet();
  cout << "In Create Multiple Parents for " << s << " with " << *C.getParentsFor(s) << "\n";
  cout << "\tWhich colapses to " << baseSet << "\n";
  bool addedASetAtLevel = baseSet.size() > 1;
  int currentNumOfBasesUsed = 2;
  while(addedASetAtLevel && currentNumOfBasesUsed <= T.getMaxParentSetSize()){
  	cout << "\tCreating parents sets of " << currentNumOfBasesUsed << " bases\n";
  	addedASetAtLevel = false;
	int * currentBases = new int[currentNumOfBasesUsed];
	//fill the current with the first few
	for (int i = 0; i < currentNumOfBasesUsed-1; i++){
		currentBases[i] = i;
	}
	//set the last one to one lower, so that increment works the first time
	currentBases[currentNumOfBasesUsed-1] = currentNumOfBasesUsed-2;
	while(incrementBaseSet(currentBases,currentNumOfBasesUsed,baseSet.size())){
	  	Set currentWorking;
	  	bool allAtSameInfluenceLevel = true;
	  	float influenceLevel = fabs(baseSet.getIndividualScore(baseSet.get(currentBases[0])->getGeneUID()));
		for (int i = 0; i < currentNumOfBasesUsed; i++){
			float m = baseSet.getIndividualScore(baseSet.get(currentBases[i])->getGeneUID());
			currentWorking.insert(baseSet.get(currentBases[i]),m);
			
			if (!(fabs(m) + T.getInfluenceLevelDelta() > influenceLevel && influenceLevel + T.getInfluenceLevelDelta() > fabs(m))){
				allAtSameInfluenceLevel = false;
				i = currentNumOfBasesUsed;	
			}
		}
		if (allAtSameInfluenceLevel){
			if (DEBUG_LEVEL>0){
				cout << "Checking if set " << currentWorking << " is better than the subsets\n";
			}
	    	float score = ScoreBetter(s,currentWorking,*s.toSet(),E,T,L);
    		currentWorking.setScore(-1,score);
    		if (C.addIfScoreBetterThanSubsets(s,currentWorking)){
    			addedASetAtLevel = true;
				if (DEBUG_LEVEL>0){
					cout << "\tIt Was Better than the subsets!\n";
				}
	    	}
    		else{
				if (DEBUG_LEVEL>0){
	    			cout << "\tNot Better\n";	
				}
    		}
		}
		else{
			if (DEBUG_LEVEL>0){
				cout << "NOT Checking if set " << currentWorking << " is better than the subsets, as the influences are not similar enough. (within " << T.getInfluenceLevelDelta() << ")\n";
			}
		}
	}
	C.removeSubsets(s);
	delete [] currentBases;
   	currentNumOfBasesUsed++;
  }
  cout << "Multiple parents for " << s << " are: " << *C.getParentsFor(s) << "\n";
}


void CreateMultipleParents_Too_Costly(Specie& s, const Species& S, const Experiments& E, NetCon& C, const Thresholds& T, const Encodings& L){
  cout << "TODO: This function needs an update so that only parents with similar scores are combined\n";
  exit(0);
  
  DoubleSet * baseSet = C.getParentsFor(s);
  DoubleSet * workingSet = new DoubleSet(*baseSet);
  cout << "In Create Multiple Parents for " << s << " with " << *baseSet << "\n";
  while(workingSet->size() > 1){
  	DoubleSet * nextWorking = new DoubleSet();
  	for (int i = 0; i < baseSet->size(); i++){
	    Set * p1 = baseSet->get(i); //getParent
    	float alpha1 = ScoreBetter(s,*p1,*s.toSet(),E,T,L);
	    cout << "\tCompeting " << *p1 << " with score of " << alpha1 << " with the following:\n";
    	for (int j = 0; j < workingSet->size(); j++){
	      if (j != i){
			Set * p2 = workingSet->get(j); //getParent
			float alpha2 = ScoreBetter(s,*p2,*s.toSet(),E,T,L);
			float alphab = ScoreBetter(s,unionIt(*p1,*p2),*s.toSet(),E,T,L);
    		cout << "\t\tWITH " << *p2 << " with score of " << alpha2 << " (the above score again) " << alpha1 << "\n";
    		cout << "\t\tBoth have score of " << alphab << "\n";
			if (fabs(alphab) >= fabs(alpha1) && fabs(alphab) >= fabs(alpha2)){
				//TODO We need to fix this
			  Set tmp = unionIt(*p1,*p2);
			  nextWorking->unionIt(tmp);
			  C.unionIt(tmp,C.EITHER,s,alphab);
	    	  cout << "\t\t\tWhich is good enough to union them\n";
			}
		  }
        }
     }
     delete workingSet;
     workingSet = nextWorking;
  }
  delete workingSet;
  C.removeSubsets(s);
  cout << "Multiple parents for " << s << " are: " << *C.getParentsFor(s) << "\n";
}

void CompetePossibleParents(Specie& s, const Species& S, const Experiments& E, NetCon& C, const Thresholds& T_old, const Encodings& L){
  cout << "Competing parents for child " << s << "\n";
//  bool progress = C.totalParents(s) > 1;
  Thresholds T(T_old);
//  while (progress){
  while(C.totalParents(s) > 1){
  	int currentNumParents = C.totalParents(s);
  	vector<DoubleSet> matchups = assignMatchups(s,S,E,C,T,L);
  	for (int i = 0; i < (int)matchups.size(); i++){
    	DoubleSet Q = matchups[i];
    	cout << "\tCompeting " << Q << "\n";
		float * Scores = new float[Q.size()];
    	for (int i = 0; i < Q.size(); i++){
      		Set * q = Q.get(i);
      		Set tmp = Q.colapseToSet();
      		tmp = tmp - *q;
      		tmp = unionIt(tmp,*s.toSet());
      		Scores[i] = ScoreBetter(s,*q,tmp,E,T,L);
    	}
    	cout << "\tScores: ";
    	for (int i = 0; i < Q.size(); i++){
    		cout << Scores[i] << " ";
    	}
    	cout << "\n";
    	C.removeLosers(s,Q,Scores);
   		delete [] Scores;
  	}
//  	progress = currentNumParents != C.totalParents(s);
	if (currentNumParents == C.totalParents(s)){
		T.harshenInitialParentsThresholds();
		cout << "\tUsing harsher numbers, as " << currentNumParents << " == " << C.totalParents(s) << ", to " << T.getA() << " " << T.getR() << "\n";
	}
  }
  cout << "After Competion " << *C.getParentsFor(s) << " is the winner set for child " << s << "\n";
}

bool setScoreSort(const Set* a, const Set* b){
	return fabs(a->getScore()) < fabs(b->getScore());
}

vector<DoubleSet> assignMatchups(const Specie& s, const Species& S, const Experiments& E,const NetCon& C, const Thresholds& T, const Encodings& L){
	vector<DoubleSet> myDS;
	DoubleSet * a = C.myConnections.at(s.getGeneUID());
	//order the sets by scores
	vector<Set*> b;
	for (int i = 0; i < a->size(); i++){
		b.push_back(a->get(i));
	}
	std::sort(b.begin(), b.end(), &setScoreSort);
	int odd = 0;
	if (a->size() % 2 == 1){ // the highest gets a by if there is an odd number
		odd++;
	}
	for (int i = 0; i < a->size()/2; i++){
		DoubleSet c;
		c.unionIt(*b[i+odd]);
		c.unionIt(*b[b.size()-i-1]);
		myDS.push_back(c);
	}
	return myDS;
}

/*
float Prob(const Specie s, const std::vector<int> * l1, const std::vector<int> * l2){
	return 1.0;
}
*/
void writeDot(const char dir[], NetCon * C){
	string s = dir;
	s.append("/method.dot");
	cout << "Opening " << s << " for write\n";
	ofstream ofile(s.c_str(),ios::out);
	//write out the species
	ofile << "digraph G {\n";
	for (int i = 1; i < Specie::getNumSpecie(); i++){
		Specie * s = Specie::getInstance("tmp",i);
		ofile << "s" << i << " [shape=ellipse,color=black,label=\"" << s->getGeneName() << "\"];\n";
	}	
	//write out the connections
	for (int i = 1; i < Specie::getNumSpecie(); i++){
		Specie * s = Specie::getInstance("tmp",i);
		DoubleSet * d = C->getParentsFor(*s);
		for (int j = 0; j < d->size(); j++){
			Set * p = d->get(j);
			for (int k = 0; k < p->size(); k++){
				Specie * parent = p->get(k);
				ofile << "s" << parent->getGeneUID() << " -> s" << i << " ";
				string color;
				string arrowhead;
				//if (p->getIndividualScore(p->get(k)->getGeneUID()) < 0){
				if (p->getScore() < 0){
					color = "firebrick4";
					arrowhead = "tee";
				}
				else{
					color = "blue4";
					arrowhead = "vee";
				}
				ofile << "[color=\"" << color << "\",label=\"" << fabs(p->getScore());
				if (p->size() > 1){
					ofile << "_m_" << p->size();
				}
				ofile << "\",arrowhead=" << arrowhead << "]\n";
			}
		}
	}
	ofile << "}\n";
	ofile.close();
}
