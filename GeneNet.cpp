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

extern bool InvertSortOrder;

int DEBUG_LEVEL = 0;

string * globDir = NULL;

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
	//used to let score function know about dir
	globDir = new string(dir);
		
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
	if (DEBUG_LEVEL > 1){
		writeLevels(dir, L,E,T);
	}
	scoreCache = new map<Specie*, map<Set, map<Set, vector<float> > > >();
	writeDot(dir, &C, E, T, L);
	delete globDir;
	delete scoreCache;
	cout << "Exiting and destroying Species, Experiments, NetCon, Thresholds, Encodings\n";
}

int main(int argc, char* argv[]){
	Thresholds T(1.15,0.75,0.5,1,1,3, 0.01, 0.025,2, false);
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

  //int ParentSetsRemoved = 0;

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
  
  cout << "All Network Connections are\n" << C << "\tbut score may be changed using an and sort for multiple parents\n";
  //cout << "Parent Sets Removed By Post Filter: " << ParentSetsRemoved << "\n";
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
  	if (DEBUG_LEVEL > 0){
  		cout << "\t\tFound score in score cache" << votesa << " " << votesr << " " << votesu << "\n";
  	}
	if (votesa+votesr+votesu < 0.001){
	  return 0;
	}
	return (votesa - votesr)/(votesa+votesr+votesu);
  }

  //Bug introduction to match perls - opposite setting at bottom of function
  if(P.size()>1){
  	if(P.getIndividualScore(P.get(0)->getGeneUID()) > 0 && P.getIndividualScore(P.get(1)->getGeneUID()) < 0){
	  InvertSortOrder = true;
  	}
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

  ofstream fout;
  if (DEBUG_LEVEL > 1){
	string sa = *globDir;
	sa.append("/gnuplot");
	//sa.append(s.getGeneName());
	//sa.append("-");
	for (int j = 0; j < P.size(); j++){
		sa.append("_");
		sa.append(P.get(j)->getGeneName());
	}
	sa.append("_-");
	for (int j = 0; j < G.size(); j++){
		sa.append("_");
		sa.append(G.get(j)->getGeneName());
	}
	sa.append(".dat");
	cout << "Opening " << s << " for write\n";
	fout.open(sa.c_str(),ios::out);
	fout << "#P Levels, G Levels, seen, rising, prob\n";
  }

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
					if(DEBUG_LEVEL>1){
						int gLevels = 0;
						int pLevels = 0;
						int power = 1;
						for (int j = 0; j < P.size(); j++){
							char c[2];
							c[0] = base->rowValues[P.get(j)->getGeneUID()];
							c[1] = 0;
							int a = atoi(c);
							pLevels += a * power;
							power = power * 10;
						}
						power = 1;
						for (int j = 0; j < G.size(); j++){
							char c[2];
							c[0] = base->rowValues[G.get(j)->getGeneUID()];
							c[1] = 0;
							int a = atoi(c);
							gLevels += a * power;
							power = power * 10;
						}
						fout << gLevels << " " << pLevels << " " << seen << " " << rising << " " << prob1 << "\n";
					}
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
				else{
					if(DEBUG_LEVEL>1){
						int gLevels = 0;
						int pLevels = 0;
						int power = 1;
						for (int j = 0; j < P.size(); j++){
							char c[2];
							c[0] = nextStr[P.get(j)->getGeneUID()];
							c[1] = 0;
							int a = atoi(c);
							pLevels += a * power;
							power = power * 10;
						}
						power = 1;
						for (int j = 0; j < G.size(); j++){
							char c[2];
							c[0] = nextStr[G.get(j)->getGeneUID()];
							c[1] = 0;
							int a = atoi(c);
							gLevels += a * power;
							power = power * 10;
						}
						fout << gLevels << " " << pLevels << " " << nextS << " " << nextR << " " << probN << "\n";
					}
					if (probRatio > T.getA() ){
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
  }

  if (DEBUG_LEVEL > 1){
 	fout.close();
  }

  //Bug introduction to match perls
  if(P.size()>1){
  	if(P.getIndividualScore(P.get(0)->getGeneUID()) > 0 && P.getIndividualScore(P.get(1)->getGeneUID()) < 0){
	  InvertSortOrder = false;
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
  		cout << "There are no parents for " << s << ", relaxing the thresholds to [" << newT.getR() << ", " << newT.getA() << "]\n";
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
	vector<Set*> b;
	for (int i = 0; i < a->size(); i++){
		b.push_back(a->get(i));
	}
	std::sort(b.begin(), b.end(), &setScoreSort);
	int odd = 0;
	if (a->size() % 2 == 1){ // the highest gets a by if there is an odd number
		odd++;
	}
	if (T.competeMultipleHighLow()){
		//order the sets by scores
		//Pick the lowest and highest scores to compete
		for (int i = 0; i < a->size()/2; i++){
			DoubleSet c;
			c.unionIt(*b[i+odd]);
			c.unionIt(*b[b.size()-i-1]);
			myDS.push_back(c);
		}
	}
	else{
		//pick score close to each other
		for (int i = odd; i < a->size(); i = i+2){
			DoubleSet c;
			c.unionIt(*b[i]);
			c.unionIt(*b[i+1]);
			myDS.push_back(c);
		}
	}
	return myDS;
}

/*
float Prob(const Specie s, const std::vector<int> * l1, const std::vector<int> * l2){
	return 1.0;
}
*/
void writeDot(const char dir[], NetCon * C, const Experiments& E, const Thresholds& T, const Encodings& L){
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
	cout << "Finished writing the species list\n";
	//write out the connections
	for (int i = 1; i < Specie::getNumSpecie(); i++){
		Specie * s = Specie::getInstance("tmp",i);
		DoubleSet * d = C->getParentsFor(*s);
		for (int j = 0; j < d->size(); j++){
			Set * p = d->get(j);
			float pScore = fabs(p->getScore());
			if (p->size() > 1){
				InvertSortOrder = true;
				cout << "Tring to get a different score than " << pScore << " with " << *s << " " << *p << "\n";
	  			pScore = ScoreBetter(*s,*p,*s->toSet(),E,T,L);
	  			cout << "After the score, with new score " << pScore << "\n";
			}
			InvertSortOrder = false;
			for (int k = 0; k < p->size(); k++){
				Specie * parent = p->get(k);
				ofile << "s" << parent->getGeneUID() << " -> s" << i << " ";
				string color;
				string arrowhead;
				bool isActivator = true;
				//the direction of the arc is based on the individual score
				if (p->getIndividualScore(p->get(k)->getGeneUID()) < 0){
					isActivator = false;
				}
				if (isActivator == false){
					color = "firebrick4";
					arrowhead = "tee";
				}
				else{
					color = "blue4";
					arrowhead = "vee";
				}
				ofile << "[color=\"" << color << "\",label=\"" << pScore;
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

void writeLevels(const char dir[], Encodings & L, Experiments & E, Thresholds & T){
	string s = dir;
	s.append("/levels.lvl");
	cout << "Opening " << s << " for write\n";
	ofstream lvl_file(s.c_str(),ios::out);


	//get the histogram sizes
	float histogram_size = 5;
	vector<vector<int> > seen;
	vector<vector<int> > rose;
	vector<int> max_seen;
	int num_entries = 0;
	const float h_offset = 1.05;
	for (int i = 0; i <= L.totalSpecies(); i++){
		vector<int> a;
		vector<int> b;
		seen.push_back(a);
		rose.push_back(b);
		max_seen.push_back(0);
	}
	//cout << "Put in " << L.totalSpecies()+1 << " into the seen and rose\n";
	for (int i = 0; i < E.totalExperiments(); i++){
		for (int j = 0; j < E.totalRows(i) - T.getWindowSize(); j++){
			std::vector<float> * current = E.getRow(i, j);
			std::vector<float> * next = E.getRow(i, j+T.getWindowSize());
			num_entries++;
			/*
			cout << "Testing vector [";
			for (int k = 0; k < (int)current->size(); k++){
				cout << " " << current->at(k);	
			}
			cout << " ]\n";
			*/

			//calculate if things rose
			for (int k = 0; k < (int)current->size(); k++){
				//add things into the seen and rose array;
				int num = (int)current->at(k);
				while((int)seen[k].size() <= num){
					seen[k].push_back(0);
					rose[k].push_back(0);
					//cout << "\t\tAdding a level to grow size to " << (int)seen[k].size() << "\n";
				}
				//cout << "\tSeen[" << k << "] now has " << (int)seen[k].size() << " things, and adding at " << (int)(current->at(k)/histogram_size) << " into it\n";
				seen[k][num]++;
				if (current->at(k)+T.getRisingAmount() <= next->at(k)){
					rose[k][num]++;
				}
			}
		}	
	}
	for (int i = 0; i < L.totalSpecies(); i++){
		Specie * p = Specie::getInstance("??",i);
		string t = dir;
		t.append("/histogram_");
		t.append(p->getGeneName());
		t.append(".dat");
		if (DEBUG_LEVEL > 0){
			cout << "\tOpening " << t << " for write\n";
		}
		ofstream histogram(t.c_str(),ios::out);
		histogram << "#amount seen rose\n";
		const int num_boxes = 20;
		const float offset = 2.5;
		float h_size = ((float)(seen[i].size())/(float)num_boxes);
		int addInNum = (int)(((int)seen[i].size()) - h_size/2);
		float cur_h_size = h_size;
		for (int j = 0; j < (int)seen[i].size(); ){
			int seen_i = 0;
			int rose_i = 0;
			for (; (j < (int)cur_h_size || j > addInNum) && j < (int)seen[i].size(); j++){
				seen_i += seen[i][j];
				rose_i += rose[i][j];
			}
			histogram << ((float)((int)((cur_h_size-h_size)/h_size*(100/num_boxes)))+offset)<< " " << seen_i << " " << rose_i << "\n";
			if(max_seen[i] < seen_i * h_offset){
				max_seen[i] = (int)(((float)seen_i) * h_offset);
			}
			cur_h_size += h_size;
		}
		histogram.close();
	}


	for (int i = 0; i <= L.totalSpecies(); i++){
		Specie * p = Specie::getInstance("??",i);
		lvl_file << p->getGeneName() << "";
		std::vector<float> v = L.getLevels(p);
		
		//write the level file
		for (int j = 0; j < (int) v.size(); j++){
			lvl_file << ", " << v.at(j);
		}
		lvl_file << "\n";

		//write to the other histogram files
		for (int j = 0; j <= L.totalSpecies(); j++){
			if (j != i){
				Specie * q = Specie::getInstance("??",j);
				string t = dir;
				t.append("/histogram_");
				t.append(p->getGeneName());
				t.append("_");
				t.append(q->getGeneName());
				t.append(".gnuplot");

				string u = "histogram_";
				u.append(p->getGeneName());
				string w = u;
				w.append("_");
				w.append(q->getGeneName());
				w.append(".ps");
				u.append(".dat");
	
				ofstream histogram(t.c_str(),ios::out);
				histogram << "set out '" << w << "'\n";
				histogram << "set terminal postscript eps color\n";
				histogram << "set size 0.4,0.6\n";
				histogram << "set title \"Expression table for " << p->getGeneName() << "\"\n";
				histogram << "set xlabel \"Expression level\"\n";
				histogram << "set ylabel \"Number of times seen\"\n";
				histogram << "#set autoscale\n";
				histogram << "set xrange [0:100]\n";
				histogram << "set yrange [0:" << max_seen[i] << "]\n";
				histogram << "set xtics 25\n";
				histogram << "set ytics 200\n";
				histogram << "set style fill solid border -1\n";
				histogram << "set boxwidth " << histogram_size-1 << "\n";
				histogram << "\n#Use the nearest 5th for the line, but also include the real histogram level\n";
				for (int k = 0; k < (int) v.size(); k++){
					int a = (int)((float)100*(float)v.at(k)/(float)seen[i].size());
				    histogram <<  "#set arrow from " << a << ",0 to " << a << "," << max_seen[i] << " nohead lt 1 front\n";
					float b = (int)a%5;
					a -= (int)b;
					if (b > 2.5){
						a += 5;
					}
				    histogram <<  "set arrow from " << a << ",0 to " << a << "," << max_seen[i] << " nohead lt 1 front\n";
				}
				histogram << "\nplot ";
				histogram << " \"" << u << "\" using 1:2 title '' with boxes lt 3\n";
				histogram.close();
			}
		}		
	}

	lvl_file.close();
}
