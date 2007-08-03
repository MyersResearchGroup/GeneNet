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

const int COMPETITION_LOG = 2;

bool HAS_TO_HAVE_MAJORITY = false;
bool CPP_USE_HARSHER_BOUNDS = false;
bool KEEP_SORT_ORDER_INVERTED = true;
bool CMP_NO_MAJORITY = false;
double TOSS_VOTES_NUMBER = 1.0001;
bool TOSS_CHANGED_SINGLE_INFLUENCE = true;
bool WRITE_LEVELS = false;
bool READ_LEVELS = false;
bool EQUAL_SPACING_PER_BIN = false;
bool SUCC = true;
bool PRED = false;
bool BASIC_FINDBASEPROB = false;

extern bool InvertSortOrder;

int DEBUG_LEVEL = 0;
double doubleCompareValue = 0.000001;

string * globDir = NULL;

map<Specie*, map<Set, map<Set, vector<double> > > > * scoreCache = NULL;

ofstream competitionLog;
ofstream contenders;



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
           _T("-tf [num] Sets the activation threshold.  Default 1.15\n")
           _T("-ta [num] Sets the repression threshold.  Default 0.75\n")
           _T("-ti [num] Sets how high a score must be to be considered a parent.  Default 0.5\n")
           _T("-tm [num] Sets how close IVs must be in score to be considered for combination.  Default 0.01\n")
           _T("-tn [num] Sets minimum number of parents to allow through in SelectInitialParents. Default 1\n")
           _T("-tj [num] Sets the max parents of merged influence vectors, Default 2\n")
           _T("-tr [num] Sets how fast the bound is relaxed for tf and ta, Default 0.025\n")

           _T("-d [num]  Sets the debug or output level.  Default 0\n")
           _T("-wr [num] Sets how much larger a number must be to be considered as a rise.  Default 1\n")
           _T("-ws [num] Sets how far the TSD points are when compared.  Default 1\n")
           _T("-nb [num] Sets how many bins are used in the evaluation.  Default 4\n")
           _T("--lvl 	  Writes out the suggested levels for every specie\n")
           _T("--readLevels Reads the levels from level.lvl file for every specie\n")
           _T("--cpp_harshenBoundsOnTie	Determins if harsher bounds are used when parents tie in CPP.\n")
           _T("--cpp_cmp_output_donotInvertSortOrder	Sets the inverted sort order in the 3 places back to normal")
           _T("--cpp_seedParents	Determins if parents should be ranked by score, not tsd order in CPP.\n")
           _T("--cmp_score_mustNotWinMajority	Determins if score should be used when following conditions are not met a > r+n || r > a + n\n")
           _T("--score_donotTossSingleRatioParents		Determins if single ratio parents should be kept\n")
           _T("--output_donotTossChangedInfluenceSingleParents	Determins if parents that change influence should not be tossed\n")
           _T("-binNumbers	Equal spacing per bin\n")
           _T("-noSUCC to not use successors in calculating probabilities\n")
           _T("-PRED use preicessors in calculating probabilities\n")
           _T("-basicFBP to use the basic FindBaseProb function\n")

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
    { OPT_HELP,  _T("-?"),           			SO_NONE    },
    { OPT_HELP,  _T("-h"),           			SO_NONE    },
    { OPT_HELP,  _T("-help"),        			SO_NONE    },
    { OPT_HELP,  _T("--help"),       			SO_NONE    },
    {  1,        _T("-d"),           			SO_REQ_SEP },
    {  2,        _T("--debug"),      			SO_REQ_SEP },
    {  3,        _T("-tf"),           			SO_REQ_SEP },
    {  4,        _T("-ta"),           			SO_REQ_SEP },
    {  5,        _T("-ti"),           			SO_REQ_SEP },
    {  6,        _T("-wr"),           			SO_REQ_SEP },
    {  7,        _T("--windowRisingAmount"),	SO_REQ_SEP },
    {  8,        _T("-ws"),           			SO_REQ_SEP },
    {  9,        _T("--windowSize"),			SO_REQ_SEP },
    { 10,        _T("-nb"),						SO_REQ_SEP },
    { 11,        _T("--numBins"),				SO_REQ_SEP },
    { 12,        _T("-tm"),						SO_REQ_SEP },
    { 13,        _T("--influenceLevelDelta"),	SO_REQ_SEP },
    { 14,        _T("-tr"),						SO_REQ_SEP },
    { 15,        _T("--relaxIPDelta"),			SO_REQ_SEP },
    { 16,        _T("-tn"),						SO_REQ_SEP },
    { 17,        _T("--cpp_harshenBoundsOnTie"),	SO_NONE },
    { 18,        _T("--cpp_cmp_output_donotInvertSortOrder"),		SO_NONE },
    { 19,        _T("--cpp_seedParents"),		SO_NONE },
    { 20,        _T("--cmp_score_mustNotWinMajority"),	SO_NONE },
    { 21,        _T("--score_donotTossSingleRatioParents"),	SO_NONE },
    { 22,        _T("--output_donotTossChangedInfluenceSingleParents"),	SO_NONE },
    { 23,        _T("--lvl"),	SO_NONE },
    { 24,        _T("--readLevels"),	SO_NONE },
    { 26,        _T("-tj"),							SO_REQ_SEP },
    { 27,        _T("-binNumbers"),	SO_NONE },
    { 28,        _T("-noSUCC"),	        SO_NONE },
    { 29,        _T("-PRED"),	        SO_NONE },
    { 30,        _T("-basicFBP"),	        SO_NONE },

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
  if (DEBUG_LEVEL > COMPETITION_LOG){
    string s = dir;
    s.append("/GeneNet_competitionLog.txt");
    cout << "Opening " << s << " for write\n";
    competitionLog.open(s.c_str(),ios::out);
  }
  {
    string s = dir;
    s.append("/contenders.txt");
    cout << "Opening " << s << " for write\n";
    contenders.open(s.c_str(),ios::out);
  }
  GeneNet(S,E,C,T,L,dir);

  scoreCache = new map<Specie*, map<Set, map<Set, vector<double> > > >();
  writeDot(dir, &C, E, T, L);
  delete globDir;
  delete scoreCache;
  cout << "Exiting and destroying Species, Experiments, NetCon, Thresholds, Encodings\n";
}

int main(int argc, char* argv[]){
  double TF = 1.15;
  double TA = 0.75;
  double TI = 0.5;
  int RisingAmount = 1;
  int WindowSize = 1;
  int NumBins = 4;
  double InfluenceLevelDelta = 0.01; //to allow parents to combine
  //double InfluenceLevelDelta = 1.0; //to allow parents to combine
  double RelaxInitialParentsDelta = 0.025;
  int MaxParentSetSize = 2;
  //int MaxParentSetSize = 3;
  bool CompeteMultipleHighLowBool = false;
  Thresholds T(TF,TA,TI,RisingAmount,WindowSize,NumBins,InfluenceLevelDelta,RelaxInitialParentsDelta,MaxParentSetSize, CompeteMultipleHighLowBool);
  cout << "GeneNet Arguments:\n";
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
      exit(0);
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
        T.setTF(atof(args.OptionArg()));
        cout << "\tSetting TF to '" << T.getTF() << "'\n";
        break;
      case 4:
        T.setTA(atof(args.OptionArg()));
        cout << "\tSetting TA to '" << T.getTA() << "'\n";
        break;
      case 5:
        T.setTI(atof(args.OptionArg()));
        cout << "\tSetting TI to '" << T.getTI() << "'\n";
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
        T.setTM(atof(args.OptionArg()));
        cout << "\tSetting TM to '" << T.getTM() << "'\n";
        break;
      case 14:
      case 15:
        T.setRelaxInitialParentsDelta(atof(args.OptionArg()));
        cout << "\tSetting relaxIPDelta to '" << T.getRelaxInitialParentsDelta() << "'\n";
        break;
      case 16:
        T.setsip_letNThrough(atoi(args.OptionArg()));
        cout << "\tSetting TN (allows n IVs though first round) to '" << T.getsip_letNThrough() << "'\n";
        break;
      case 26:
        T.setMaxParentSetSize(atoi(args.OptionArg()));
        cout << "\tSetting TJ to '" << T.getMaxParentSetSize() << "'\n";
        break;
      default:
        cout << "ERROR: unhandled argument\n";
        exit(1);
        break;
      }
    }
    else {
      _tprintf(_T("option: %2d, text: '%s'\n"),
               args.OptionId(), args.OptionText());
      switch (args.OptionId()) {
      case 17:
        CPP_USE_HARSHER_BOUNDS = true;
        cout << "\tSetting CPP to use harsher bounds\n";
        break;
      case 18:
        KEEP_SORT_ORDER_INVERTED = false;
        cout << "\tSetting sort order to not be inverted for AR\n";
        break;
      case 19:
        T.setCompeteMultipleHighLow(true);
        cout << "\tSetting CMP competitions to be seeded\n";
        break;
      case 20:
        CMP_NO_MAJORITY = true;
        cout << "\tSetting CMP and score to use the actual score, and not disallow a > r+n or r > a+n\n";
        break;
      case 21:
        TOSS_VOTES_NUMBER = 0.0001;
        cout << "\tSetting score to assign a score of 0 to only scores < 0.0001 and not score that have only a single vote\n";
        break;
      case 22:
        TOSS_CHANGED_SINGLE_INFLUENCE = false;
        cout << "\tNo longer tossing single parents that changed influence durring competition\n";
        break;
      case 23:
        WRITE_LEVELS = true;
        cout << "\tWriting out the levels\n";
        break;
      case 24:
        READ_LEVELS = true;
        cout << "\tReading the levels\n";
        break;
      case 27:
        EQUAL_SPACING_PER_BIN = true;
        cout << "\tUsing Equal spacing per bin'\n";
        break;
      case 28:
        SUCC = false;
        cout << "\tNot using successors\n";
        break;
      case 29:
        PRED = true;
        cout << "\tUsing predecessors\n";
        break;
      case 30:
        BASIC_FINDBASEPROB = true;
        cout << "\tUsing the basic FindBaseProb\n";
        break;
      default:
        cout << "ERROR: unhandled argument\n";
        exit(1);
        break;
      }
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

void GeneNet(Species &S, Experiments &E, NetCon &C, Thresholds &T, Encodings &L, const char * dir){

  cout << "Encoding the experiments\n";
  EncodeExpts(S,E,T,L, false);
  cout << "Finished encoding\n";

  if (READ_LEVELS){
    readLevels(dir, L,E,T);
  }
  if (WRITE_LEVELS){
    EncodeExpts(S,E,T,L, READ_LEVELS);
    writeLevels(dir, L,E,T);
    exit(0);
  }


  //int ParentSetsRemoved = 0;
  TSDPoint::setMaxEncodings(L);

  for (int i = 0; i < S.size(); i++){
    scoreCache = new map<Specie*, map<Set, map<Set, vector<double> > > >();
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


void EncodeExpts(Species& S, Experiments& E, Thresholds & T, Encodings& L, bool read_levels){
  L.initialize(&S,&E,&T);
  cout << "Filling hashes now\n";

  if(EQUAL_SPACING_PER_BIN){
    if (!L.useNumbers(T.getBins(), read_levels,SUCC,PRED)){
      cout << "ERROR! Equal Spacing per bin didn't work\n";
      exit(0);
    }
  }
  else if (!L.useBins(T.getBins(), read_levels,SUCC,PRED)){
    cout << "ERROR! Equal Data Per Bin didn't work\n";
    exit(0);
  }
  cout << "Finished initializing\n";
  return;
}


double ScoreBetter(Specie& s, const Set& P, const Set& G, const Experiments& E, const Thresholds& T, const Encodings& L){
  double votesa; double votesr; double votesu;
  votesa = 0; votesr = 0; votesu = 0;


  if ((*scoreCache)[&s][P][G].size() > 0){
    vector<double> f = (*scoreCache)[&s][P][G];
    for (int i = 0; i < (int)f.size(); i++){
      double probRatio = f[i];
      if (probRatio > T.getTF() ){
        votesa++;
      }
      else if (probRatio < T.getTA()){
        votesr++;
      }
      else{
        votesu++;
      }
    }
    if (DEBUG_LEVEL > 0){
      cout << "\t\tFound score in score cache (" << votesa << ", " << votesr << ", " << votesu << ")\n";
    }
    if (votesa+votesr+votesu < TOSS_VOTES_NUMBER){
      return 0;
    }
    //if added to match perl - also below
    if(HAS_TO_HAVE_MAJORITY && ! (votesa > votesr+votesu || votesr > votesa + votesu)){
      return 0;	
    }
    return (votesa - votesr)/(votesa+votesr+votesu);
  }

  vector<double> * fillProbVector = &(*scoreCache)[&s][P][G];
  
  Set tmpG(G);
  Set tmpP(P);
  if (DEBUG_LEVEL>0){
    cout << "\t\tWe are sorting with inverted " << InvertSortOrder << " P set:" << tmpP << " and G set " << tmpG << "\n";
  }

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
  double rising = 0;
  double seen = 0;
  double nextR = 0;
  double nextS = 0;
  double prob1 = -1;
  double nValue = 0;
  int end = (int)TSDPoint::initialValues->size();
  if (BASIC_FINDBASEPROB){
    for (int i = 0; i <= end; i++){
      //we have to do the very end one for a score
      if (i < end && base == NULL){
        base = NULL;
        rising = 0;
        seen = 0;
        prob1 = -1;
        nValue = 0;
        next = NULL;
        nextR = 0;
        nextS = 0;

        bool isBase = true;
        for(int j = 0; j < P.size() && isBase; j++){
          Specie * st = P.get(j);
          TSDPoint * t = (*TSDPoint::initialValues)[i];
          char valc [2];
          valc[0]= t->rowValues[st->getGeneUID()];
          valc[1] = 0;
          int val = atoi(valc);
          int max = L.getMaxLevel(st);
          if (val == 0){
            if (!P.sortsLowToHigh(st->getGeneUID())){
              isBase = false;
            }
          }
          else if (val == max){
            if (P.sortsLowToHigh(st->getGeneUID())){
              isBase = false;
            }
          }
          else{
            isBase = false;
          }

          if (!isBase){
            cout << "Base " << t->rowValues << " is not a base because of " << st->getGeneUID() << "\n";
            cout << "\tSorts Low To High " << P.sortsLowToHigh(st->getGeneUID()) << " Max " << max << "\n";
          }
        }
        if (isBase){
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
          if (DEBUG_LEVEL > 1){
            cout << "\t\t\t\t\tAt base " << base->rowValues << " there are " << rising << " / " << seen << "\n";	
          }
        }
      }
      else if (i < end && base->sameLevels((*TSDPoint::initialValues)[i])){
        double r = (*TSDPoint::initialValues)[i]->risings[s.getGeneUID()];
        double t = (*TSDPoint::initialValues)[i]->seen[s.getGeneUID()];
        rising += r;
        seen   += t;
        if (DEBUG_LEVEL > 1){
          cout << "\t\t\t\t\tAt same base " << (*TSDPoint::initialValues)[i]->rowValues << " there are " << r << " / " << t << "\n";	
        }
      }
      else if (i < end && next == NULL && base->cannotCompareLevels((*TSDPoint::initialValues)[i])){
        if (DEBUG_LEVEL > 1){
          cout << "\t\t\t\t\tUnable to compare " << base->rowValues << " with " << (*TSDPoint::initialValues)[i]->rowValues << "\n";
        }
        //make sure that we can even compare base to the next one and it hasn't jumped
        base = NULL;
        i--;
      }
      else if (i < end && next == NULL){
        next = (*TSDPoint::initialValues)[i];
        nextR = next->risings[s.getGeneUID()];	
        nextS = next->seen[s.getGeneUID()];
        if (DEBUG_LEVEL > 1){
          cout << "\t\t\t\t\tAt next " << next->rowValues << " there are " << nextR << " / " << nextS << "\n";	
        }
      }
      else if (i < end && next->sameLevels((*TSDPoint::initialValues)[i])){
        double r = (*TSDPoint::initialValues)[i]->risings[s.getGeneUID()];
        double t = (*TSDPoint::initialValues)[i]->seen[s.getGeneUID()];
        nextR += r;
        nextS += t;
        if (DEBUG_LEVEL > 1){
          cout << "\t\t\t\t\tAt same next " << (*TSDPoint::initialValues)[i]->rowValues << " there are " << r << " / " << t << "\n";	
        }
      }
      else{
        //we have valid values, but we have to either start base at this i, or next at this i
        if (i < end){
          i--;
        }
        if (seen == 0 || (seen <= nValue/10.0f && seen < 20)){
          //don't fill it, but let base start from this value
          if (DEBUG_LEVEL>0){
            if (base == NULL){

            }
            else{
              cout << "\t\t\t\tBase prob " << base->rowValues << " = " << rising << " / " << seen << " = " << prob1 << " BUT!!\n";
              if (rising == 0 || seen == 0){
                cout << "\t\t\t\t\t\tNot using current as base because = " << rising << " / " << seen;
              }
              else{
                cout << "\t\t\t\t\t\tNot using current as base because of N Value " << rising << " / " << seen << " and nValue " << nValue;
              }
              cout << " instead trying " << nextR << " / " << nextS << "\n";
            }
          }
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
          if (nextS == 0){
            //cannot compute this difference
            if (DEBUG_LEVEL>0){
              cout << "\t\t\t\t\t\tNot A Valid Ratio Because the Next = " << nextR << " / " << nextS << "\n";
            }
          }
          else{
            double probN = nextR/nextS;
            if (DEBUG_LEVEL>0){
              cout << "\t\t\t\tNext prob " << nextStr << " = " << nextR << " / " << nextS << " = " << probN << "\n";
            }
            double probRatio = 0;
            if (prob1 <= 0.001){
              if (probN <= 0.001){
                probRatio = 1;
              }
            }
            else{
              probRatio = probN / prob1;
            }
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
              if (probRatio > T.getTF() ){
                fillProbVector->push_back(probRatio);
                votesa++;
              }
              else if (probRatio < T.getTA()){
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
  }
  else{
    //advanced find base prob
    int baseI = -1;
    for (int i = 0; i <= end; i++){
      //we have to do the very end one for a score
      if (i < end && base == NULL){
        base = (*TSDPoint::initialValues)[i];
        baseI = i;
        rising = -1;	
        seen = -1;
        prob1 = -1;
        //Find out how many values are at this level for use later
        nValue = seen;
        for (int j = i+1; j < end && ! base->cannotCompareLevels((*TSDPoint::initialValues)[j]); j++){
          nValue += (*TSDPoint::initialValues)[j]->seen[s.getGeneUID()];
        }
        next = NULL;
        nextR = 0;
        nextS = 0;
        i++;
        while(i < end && base->sameLevels((*TSDPoint::initialValues)[i])){
          i++;
        }
        i--;
      }
      else if (i < end && next == NULL && base->cannotCompareLevels((*TSDPoint::initialValues)[i])){
        if (DEBUG_LEVEL > 1){
          cout << "\t\t\t\tFinding a new bin because there is no base to compare to\n";
        }
        base = NULL;
        next = NULL;
        nextR = 0;
        nextS = 0;
        i--;
      }
      else if (i < end && next == NULL){
        next = (*TSDPoint::initialValues)[i];
        nextR = next->risings[s.getGeneUID()];	
        nextS = next->seen[s.getGeneUID()];
        i++;
        if (DEBUG_LEVEL > 1){
          cout << "\t\t\t\tNext = " << next->rowValues << "\n";	
        }
        while(i < end && next->sameLevels((*TSDPoint::initialValues)[i])){
          double r = (*TSDPoint::initialValues)[i]->risings[s.getGeneUID()];
          double t = (*TSDPoint::initialValues)[i]->seen[s.getGeneUID()];
          nextR += r;
          nextS += t;
          if (DEBUG_LEVEL > 1){
            cout << "\t\t\t\tAlso includes " << (*TSDPoint::initialValues)[i]->rowValues << "\n";	
          }
          i++;
        }
        i--;
      }
      else{
        if (DEBUG_LEVEL > 1){
          cout << "\t\t\t\tTrying to create a ratio\n";
        }
        //we have valid values, but we have to either start base at this i, or next at this i
        if (i < end){
          i--;
        }
        if (nextS == 0){
          //cannot compute this difference
          if (DEBUG_LEVEL>0){
            cout << "\t\t\t\t\t\tNot A Valid Ratio Because the Next = " << nextR << " / " << nextS << "\n";
          }
        }
        //We have finished with next and just need to calculate the probabilities
        if (next == NULL){
          if (DEBUG_LEVEL>1){
            cout << "\t\t\t\t\t\tNot A Valid Ratio Because there is no next = \n";
          }
        }
        else{
          //Calculate the base probability
          if (DEBUG_LEVEL > 1){
            cout << "\t\t\t\tCalculating the baseSet\n";
          }
          rising = 0;	
          seen = 0;
          prob1 = -1;
          int lattice_level = (*TSDPoint::initialValues)[baseI]->LatticeLevel(P);
          for (int j = baseI; j < i && lattice_level < next->LatticeLevel(P); j++){
            //if (DEBUG_LEVEL > 1){
            //  cout << "\t\t\t\tTrying to add in base " << i-j << "\n";
            //}
            TSDPoint * basePotential = (*TSDPoint::initialValues)[j];
            if (basePotential->LatticeLevel(P) > lattice_level){
              if (DEBUG_LEVEL > 1){
                cout << "\t\t\t\t\tRead to the end of a lattice level\n";
              }
              //See if lattice_level has enough data
              if (seen <= nValue/10.0f && seen < 20){
                //try the next lattice level
                lattice_level = basePotential->LatticeLevel(P);
                base = basePotential;
                seen = basePotential->seen[s.getGeneUID()];
                rising = basePotential->risings[s.getGeneUID()];
              }
              else{
                j = i;
                if (DEBUG_LEVEL > 1){
                  cout << "\t\t\t\t\tThose are a good base set\n";
                }
              }
            }
            else{
              //see if the current can be added into the set
              if (areConnected(base,basePotential,P,L)){
                if (DEBUG_LEVEL > 1){
                  cout << "\t\t\t\t\t" << basePotential->rowValues <<" is Comparable\n";
                }
                seen += basePotential->seen[s.getGeneUID()];
                rising += basePotential->risings[s.getGeneUID()];
              }
              else{
                if (DEBUG_LEVEL > 1){
                  cout << "\t\t\t\t\t" << basePotential->rowValues <<" not Comparable\n";
                }
                //skip this one
              }
            }
          }
          //See if lattice_level has enough data
          if (!(seen <= nValue/10.0f && seen < 20)){            
            //calculate the probability
            prob1 = rising/seen;
            if (DEBUG_LEVEL > 1){
              cout << "\t\t\t\t\tFound probability of " << prob1 << "\n";
            }
          }

          string nextStr = "";
          nextStr = next->rowValues;
          next = NULL;
          if (prob1 == -1){
            base = NULL;
            if (DEBUG_LEVEL > 1){
              cout << "\t\t\t\tUnable to find a base for " << nextStr << "\n";
            }
          }
          else{
            double probN = nextR/nextS;
            if (DEBUG_LEVEL>0){
              cout << "\t\t\t\tNext prob " << nextStr << " = " << nextR << " / " << nextS << " = " << probN << "\n";
            }
            
            double probRatio = 0;
            if (prob1 <= 0.001){
              if (probN <= 0.001){
                probRatio = 1;
              }
            }
            else{
              probRatio = probN / prob1;
            }
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
              if (probRatio > T.getTF() ){
                fillProbVector->push_back(probRatio);
                votesa++;
              }
              else if (probRatio < T.getTA()){
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
  }
  if (DEBUG_LEVEL>1){
    cout << "\t\t\t\t\t\tCalculated all the ratios\n";
  }
  if (DEBUG_LEVEL > 1){
    fout.close();
  }

  TSDPoint::G = NULL;
  TSDPoint::P = NULL;

  if (DEBUG_LEVEL>0){
    cout << "\t\tvotes (a,r,u) (" << votesa << " " << votesr << " " << votesu << ") or ";
    cout << "(" << votesa << " - " << votesr << ")/(" << votesa+votesr+votesu << ")\n";
  }
  if (votesa+votesr+votesu < TOSS_VOTES_NUMBER){
    return 0;
  }
  //if added to match perl
  if(HAS_TO_HAVE_MAJORITY && ! (votesa > votesr+votesu || votesr > votesa + votesu)){
    return 0;	
  }
  return (votesa - votesr)/(votesa+votesr+votesu);
}

double Score(const Specie& s, const Set& P, const Set& G, const Experiments& E, const Thresholds& T, const Encodings& L){
  cout << "TODO: this needs to be fixed so that n_values work correctly\n";
  exit(0);
  double votesa = 0;  double votesr = 0;  double votesu = 0;
  LevelAssignments lp(G,L);
  if (DEBUG_LEVEL>0){
    cout << "\tScoring using P as: " << P << "\n\t\tand G as " << G << "\n";
  }
  for (int i = 0; i < lp.size(); i++){

    LevelAssignments l(P,L);
    double prob1 = L.getProb(&s,l.get(0),lp.get(i));

    for (int j = 1; j < l.size(); j++){
      double prob2 = L.getProb(&s,l.get(j),lp.get(i));
      double probRatio = prob2 / prob1;

      if (DEBUG_LEVEL>0){
        cout << "\t\t\t\t\tRatio is " << prob1 << " / " << prob2 << " = " << probRatio << "\n";
      }
      if (prob1 == -1 || prob2 == -1){
        if (DEBUG_LEVEL>0){
          cout << "\t\t\t\t\t\tNot A Valid Ratio\n";
        }
      }
      else if (probRatio > T.getTF() ){
        votesa++;
      }
      else if (probRatio < T.getTA()){
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
  if (DEBUG_LEVEL > COMPETITION_LOG){
    competitionLog << "Child " << s << "\n";
  }
  contenders << "Child " << s << "\n";

  ostringstream competitionString;
  ostringstream contendersString;
  ostringstream backgroundIV;
  backgroundIV << "<";
  for (int i = 1; i < Specie::getNumSpecie(); i++){
    backgroundIV << "n";
  }
  backgroundIV << ">";

  Thresholds newT(T);
  bool relaxedTheBounds = false;
  bool canRelaxMore = true;
  bool done_once = false;
  while ((C.getParentsFor(s)->size() < T.getsip_letNThrough() && canRelaxMore) || !done_once){
    done_once = true;

    competitionString.str(""); // clear the string if we need to relax the bounds
    contendersString.str("");

    C.getParentsFor(s)->clearAllSets();
    for (int i = 0; i < S.size(); i++){
      Specie * p = S.get(i);
      if (*p != s){
        cout << "\tTesting specie " << *p << " as a parent\n";
        double alpha = ScoreBetter(s,*p->toSet(),*s.toSet(),E,newT,L);
  			
        cout << "\tScore of " << alpha << " and threshold +-" << newT.getTI() << "\n";
        if (alpha >= newT.getTI()){
          cout << "\t\tMeans an activation parent\n";
          C.unionIt(*p->toSet(),C.ACTIVATION,s,alpha);
          if (DEBUG_LEVEL > COMPETITION_LOG){
            competitionString << "\t" << *p << " passes as activation with " << alpha << "\n";
          }
          contendersString << "\t" << p->toIV('r') << " " << -alpha << " spot 1r " << backgroundIV.str() << " " << newT.getTI() <<"\n";
        }
        else if (alpha <= -newT.getTI()){
          cout << "\t\tMeans a represion parent\n";
          C.unionIt(*p->toSet(),C.REPRESSION,s,alpha);
          if (DEBUG_LEVEL > COMPETITION_LOG){
            competitionString << "\t" << *p << " passes as repression with " << alpha << "\n";
          }
          contendersString << "\t" << p->toIV('a') << " " << alpha << " spot 1a "  << backgroundIV.str() << " " << newT.getTI() <<"\n";
        }
        else{
          if (DEBUG_LEVEL > COMPETITION_LOG){
            competitionString << "\t" << *p << " fails with " << alpha <<"\n";
          }	
          contendersString << "\t" << p->toIV('a') << " " << alpha << " spot 1a "  << backgroundIV.str() << " " << newT.getTI() <<"\n";
          contendersString << "\t" << p->toIV('r') << " " << -alpha << " spot 1r "  << backgroundIV.str() << " " << newT.getTI() <<"\n";
        }
      }
    }
    if (C.getParentsFor(s)->size() < T.getsip_letNThrough()){
      relaxedTheBounds = true;
      newT.relaxInitialParentsThresholds();
      cout << "There are not enough parents for " << s << ", relaxing the thresholds to [" << newT.getTA() << ", " << newT.getTF() << "] and Ti " << newT.getTI() << "\n";
      if (newT.getTF() <= 1+doubleCompareValue && newT.getTA() >= 1 - doubleCompareValue){
        if (newT.getTI() <= 0.0001){
          cout << "CANNOT RELAX BOUNDS MORE TO LET " << T.getsip_letNThrough() << " parent through\n";
          canRelaxMore = false;
        }
        newT.setTI(newT.getTI() - newT.getRelaxInitialParentsDelta());
        if (newT.getTI() < 0.0){
          newT.setTI(0.0);
        }
      }
      if (newT.getTF() < 1){
        newT.setTF(1.0);
      }
      if (newT.getTA() > 1){
        newT.setTA(1.0);
      }
    }
  }
  
  if (relaxedTheBounds){
    if (DEBUG_LEVEL > COMPETITION_LOG){
      competitionLog << "The bounds were relaxed\n";
    }
    cout << "Rescoring parents with harsher numbers, as we relaxed the bounds\n";
    DoubleSet * rescore = C.getParentsFor(s);
    for (int i = 0; i < rescore->size(); i++){
      Set * rescoreSet = rescore->get(i);
      assert(rescoreSet->size() == 1);
      Specie * p = rescoreSet->get(0);
      double alpha = ScoreBetter(s,*p->toSet(),*s.toSet(),E,T,L);
      cout << "\tScore of " << alpha << " and threshold +- " << T.getTI() << " for " << *p << "\n";
      rescoreSet->setScore(-1, alpha);
    }
  }
  
  if (DEBUG_LEVEL > COMPETITION_LOG){
    competitionLog << competitionString.str() <<"\n";
  }
  contenders << contendersString.str();
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
  if (DEBUG_LEVEL > COMPETITION_LOG){
    competitionLog << "Multiple Parents for " << s << "\n";
  }
  int currentNumOfBasesUsed = 2;
  while(currentNumOfBasesUsed <= T.getMaxParentSetSize()){
    cout << "\tCreating parents sets of " << currentNumOfBasesUsed << " bases\n";
    int * currentBases = new int[currentNumOfBasesUsed];
    //fill the current with the first few
    for (int i = 0; i < currentNumOfBasesUsed-1; i++){
      currentBases[i] = i;
    }
    //set the last one to one lower, so that increment works the first time
    currentBases[currentNumOfBasesUsed-1] = currentNumOfBasesUsed-2;
    while(incrementBaseSet(currentBases,currentNumOfBasesUsed,baseSet.size())){
      Set currentWorking;
      double min = 999;
      double max = -999;
      for (int i = 0; i < currentNumOfBasesUsed; i++){
        double m = baseSet.getIndividualScore(baseSet.get(currentBases[i])->getGeneUID());
        currentWorking.insert(baseSet.get(currentBases[i]),m);
        if (fabs(m) < min){
          min = fabs(m);
        }
        if (fabs(m) > max){
          max = fabs(m);
        }
      }
      if (min + T.getTM() >= max){
        if (DEBUG_LEVEL>0){
          cout << "Checking if set " << currentWorking << " is better than the subsets\n";
        }

        //bug to match perl
        HAS_TO_HAVE_MAJORITY = true;
        if (CMP_NO_MAJORITY){
          HAS_TO_HAVE_MAJORITY = false;
        }
        double score = ScoreBetter(s,currentWorking,*s.toSet(),E,T,L);
        HAS_TO_HAVE_MAJORITY = false;
        InvertSortOrder = false;

        currentWorking.setScore(-1,score);
        if (C.addIfScoreBetterThanSubsets(s,currentWorking)){
          if (DEBUG_LEVEL>0){
            cout << "\tIt Was Better than the subsets!\n";
            if (DEBUG_LEVEL > COMPETITION_LOG){
              competitionLog << currentWorking << " passes with " << score << "\n";
            }            
          }
        }
        else{
          if (DEBUG_LEVEL>0){
            cout << "\tNot Better\n";	
            if (DEBUG_LEVEL > COMPETITION_LOG){
              competitionLog << currentWorking << " fails with " << score << "\n";
            }
          }
          contenders << "\t" << currentWorking.toIV() << " " << fabs(score) << " spot 3 ";
          contenders << currentWorking.toIndividualIV() << "\n";
        }
      }
      else{
        if (DEBUG_LEVEL>0){
          cout << "NOT Checking if set " << currentWorking << " is better than the subsets, as the influences are not similar enough. (within " << T.getTM() << ")\n";
          if (DEBUG_LEVEL > COMPETITION_LOG){
            competitionLog << currentWorking << " fails because of similar threshold within " << T.getTM() << "\n";
          }
        }

        /*
        //Check the merged score always now
        HAS_TO_HAVE_MAJORITY = true;
        if (CMP_NO_MAJORITY){
          HAS_TO_HAVE_MAJORITY = false;
        }
        double score = ScoreBetter(s,currentWorking,*s.toSet(),E,T,L);
        HAS_TO_HAVE_MAJORITY = false;
        InvertSortOrder = false;
        //done checking score
        */
        double score = T.getTM();

        contenders << "\t" << currentWorking.toIV() << " " << score << " spot 2 ";
        contenders << currentWorking.toIndividualIV() << "\n";
      }
    }
    contenders << C.removeSubsets(s);
    delete [] currentBases;
    currentNumOfBasesUsed++;
  }
  contenders << C.filterByScore(s,T.getTI());
  
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
      double alpha1 = ScoreBetter(s,*p1,*s.toSet(),E,T,L);
      cout << "\tCompeting " << *p1 << " with score of " << alpha1 << " with the following:\n";
      for (int j = 0; j < workingSet->size(); j++){
        if (j != i){
          Set * p2 = workingSet->get(j); //getParent
          double alpha2 = ScoreBetter(s,*p2,*s.toSet(),E,T,L);
          double alphab = ScoreBetter(s,unionIt(*p1,*p2),*s.toSet(),E,T,L);
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
  bool progress = C.totalParents(s) > 1;
  Thresholds T(T_old);
  if (DEBUG_LEVEL > COMPETITION_LOG){
    competitionLog << "Competition for " << s << "\n";
  }
  while (progress){
    int currentNumParents = C.totalParents(s);
    vector<DoubleSet> matchups = assignMatchups(s,S,E,C,T,L);
    for (int i = 0; i < (int)matchups.size(); i++){
      DoubleSet Q = matchups[i];
      cout << "\tCompeting " << Q << "\n";
      double * Scores = new double[Q.size()];
      for (int i = 0; i < Q.size(); i++){
        Set * q = Q.get(i);
        Set tmp = Q.colapseToSet();
        tmp = tmp - *q;
        tmp = unionIt(tmp,*s.toSet());
        //Bug introduction to match perls
        if(q->size()>1){
          double a = q->getIndividualScore(q->get(0)->getGeneUID());
          double b = q->getIndividualScore(q->get(1)->getGeneUID());
          if( (a > 0 && b < 0) || (a < 0 && b > 0) && KEEP_SORT_ORDER_INVERTED){
            InvertSortOrder = true;
          }
        }
        Scores[i] = ScoreBetter(s,*q,tmp,E,T,L);
        for (int j = 0; j < C.getParentsFor(s)->size(); j++){
          Set * a = C.getParentsFor(s)->get(i);
          if (*a == *q){
            a->setCompetitionScore(Scores[i]);
          }
        }
        InvertSortOrder = false;
      }
      cout << "\tScores: ";
      for (int i = 0; i < Q.size(); i++){
        cout << Scores[i] << " ";
      }
      cout << "\n";
      ostringstream cS;
      ostringstream contendersString;
      cS.str(C.removeLosers(s,Q,Scores,contendersString));
      if (DEBUG_LEVEL > COMPETITION_LOG){
        competitionLog << cS.str();
      }
      contenders << contendersString.str();
      delete [] Scores;
    }
    if (CPP_USE_HARSHER_BOUNDS){
      if (currentNumParents == C.totalParents(s)){
        bool f = T.harshenInitialParentsThresholds();
        cout << "\tUsing harsher numbers, as " << currentNumParents << " == " << C.totalParents(s) << ", to " << T.getTF() << " " << T.getTA() << "\n";
        if (f == false){
          //exit, as we cannot shrink any more
          progress = false;	
        }
      }
      if (C.totalParents(s) == 1){
        //exit while loop
        progress = false;	
      }
    }
    else{
      progress = currentNumParents != C.totalParents(s);
    }
  }
  if (DEBUG_LEVEL > COMPETITION_LOG){
    competitionLog << "Winner " << *C.getParentsFor(s) <<  "\n\n\n\n";
  }
  cout << "After Competion " << *C.getParentsFor(s) << " is the winner set for child " << s << "\n";
  
  contenders << "\t" << C.getParentsFor(s)->toIV() << " winner\n";
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
  //seeding by score?
  //std::sort(b.begin(), b.end(), &setScoreSort);
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
  double Prob(const Specie s, const std::vector<int> * l1, const std::vector<int> * l2){
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
    Set printed;
    for (int j = 0; j < d->size(); j++){
      Set * p = d->get(j);
      double initialScore = p->getScore();
      double competitionScore = p->getCompetitionScore();
      double pScore = fabs(initialScore);
      //rescoring to match perls
      double mpa = 0;
      double mpb = 0;
      if (p->size() > 1){
        mpa = p->getIndividualScore(p->get(0)->getGeneUID());
        mpb = p->getIndividualScore(p->get(1)->getGeneUID());
        if((mpa > 0 && mpb < 0) || (mpa < 0 && mpb > 0) && KEEP_SORT_ORDER_INVERTED){
          InvertSortOrder = true;
          cout << "Tring to get a different score than " << pScore << " with " << *s << " " << *p << "\n";
          pScore = ScoreBetter(*s,*p,*s->toSet(),E,T,L);
          cout << "After the score, with new score " << pScore << "\n";
        }
      }
      //format pScore
      pScore = ((double)((int)(pScore * 1000.0)))/ 1000.0;
      InvertSortOrder = false;
      for (int k = 0; k < p->size(); k++){
        Specie * parent = p->get(k);
        if (!printed.containsSpecieID(parent->getGeneUID())){
          printed.insert(parent,pScore);
          ofile << "s" << parent->getGeneUID() << " -> s" << i << " ";
          string color;
          string arrowhead;
          bool isActivator = true;
          //the direction of the arc is based on the individual score
          if(p->size() == 1 && TOSS_CHANGED_SINGLE_INFLUENCE && ((initialScore < 0 && competitionScore > 0)  || (initialScore > 0 && competitionScore < 0))){
            ofile << "[color=\"gray\",label=\"" << pScore << "\",competitionScore=\"" << competitionScore << "\"]\n";
          }
          //					else if(p->size() == 2 && ((mpa > 0 && mpb > 0) || (mpa < 0 && mpb < 0)) && ((initialScore < 0 && competitionScore > 0)  || (initialScore > 0 && competitionScore < 0))){
          //						ofile << "[color=\"gray\",label=\"" << pScore << "\",competitionScore=\"" << competitionScore << "\"]\n";
          //					}					
          else{
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
            ofile << "\",competionScore=\"" << competitionScore << "\"";
            ofile << ",arrowhead=" << arrowhead << "]\n";
          }
        }
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
  double histogram_size = 5;
  vector<vector<int> > seen;
  vector<vector<int> > rose;
  vector<int> max_seen;
  int num_entries = 0;
  const double h_offset = 1.05;
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
      std::vector<double> * current = E.getRow(i, j);
      std::vector<double> * next = E.getRow(i, j+T.getWindowSize());
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
  for (int i = 1; i <= L.totalSpecies(); i++){
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
    //const int num_boxes = 20;
    const int num_boxes = 15;
    const double offset = 2.5;
    //const double offset = 0;
    double h_size = ((double)(seen[i].size())/(double)num_boxes); //The size of boxes in normal values
    int addInNum = (int)(((int)seen[i].size()) - h_size/2); //Add in the last few if things don't set exactly right
    double cur_h_size = h_size;
    for (int j = 0; j < (int)seen[i].size(); ){
      int seen_i = 0;
      int rose_i = 0;
      for (; (j < (int)cur_h_size || j > addInNum) && j < (int)seen[i].size(); j++){
        seen_i += seen[i][j];
        rose_i += rose[i][j];
      }
      histogram << ((double)((((cur_h_size-h_size)/h_size)*((double)(100.0f/(double)num_boxes))))+offset)<< " " << seen_i << " " << rose_i << "\n";
      if(max_seen[i] < seen_i * h_offset){
        max_seen[i] = (int)(((double)seen_i) * h_offset);
      }
      cur_h_size += h_size;
    }
    histogram.close();
  }


  for (int i = 0; i <= L.totalSpecies(); i++){
    Specie * p = Specie::getInstance("??",i);
    lvl_file << p->getGeneName() << "";
    std::vector<double> v = L.getLevels(p);
		
    //write the number of levels
    lvl_file << ", " << ((int) v.size()+1);
    //write the level file
    for (int j = 0; j < (int) v.size(); j++){
      lvl_file << ", " << v.at(j);
    }
    lvl_file << "\n";

    //write to the other histogram files
    string t = dir;
    t.append("/histogram_");
    t.append(p->getGeneName());
    t.append(".gnuplot");

    string u = "histogram_";
    u.append(p->getGeneName());
    string w = u;
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
    histogram << "set ytics " << ((int)max_seen[i]/4) << "\n";
    histogram << "set style fill solid border -1\n";
    histogram << "set boxwidth " << histogram_size-1 << "\n";
    histogram << "\n#Use the nearest 5th for the line, but also include the real histogram level\n";
    for (int k = 0; k < (int) v.size(); k++){
      int a = (int)((double)100*(double)v.at(k)/(double)seen[i].size());
      histogram <<  "#set arrow from " << a << ",0 to " << a << "," << max_seen[i] << " nohead lt 1 front\n";
      double b = (int)a%5;
      a -= (int)b;
      if (b > 2.5){
        a += 5;
      }
      histogram <<  "set arrow from " << a << ",0 to " << a << "," << max_seen[i] << " nohead lt 1 front\n";
    }
    histogram <<  "set arrow from 33.3333333333333,0 to 33.3333333333333," << max_seen[i] << " nohead lt 2 front\n";
    histogram <<  "set arrow from 66.6666666666666,0 to 66.6666666666666," << max_seen[i] << " nohead lt 2 front\n";
    histogram << "\nplot ";
    histogram << " \"" << u << "\" using 1:2 title '' with boxes lt 3\n";
    histogram.close();
  }

  lvl_file.close();
}


void readLevels(const char dir[], Encodings & L, Experiments & E, Thresholds & T){
  string s = dir;
  s.append("/levels.lvl");
  cout << "Opening " << s << " for read\n";
  cout << "Reading file for levels\n";
  ifstream lvl_file(s.c_str(),ios::in);
  if(! L.useFile(lvl_file, !WRITE_LEVELS, SUCC,PRED)){
    cout << "ERROR: Unable to read levels file\n";
    exit(0);
  }
  lvl_file.close();
}



bool areConnected(TSDPoint * bottom, TSDPoint * top, const Set& P, const Encodings& L){
  //if (DEBUG_LEVEL > 1){
  //  cout << "\t\t\t\tDetermining if two points are connected\n";
  //}
  bool c = true;
  for(int j = 0; j < P.size() && c; j++){
    Specie * st = P.get(j);
    char valc [2];
    valc[0]= bottom->rowValues[st->getGeneUID()];
    valc[1] = 0;
    int val1 = atoi(valc);
    valc[0]= top->rowValues[st->getGeneUID()];
    int val2 = atoi(valc);

    if (P.sortsLowToHigh(st->getGeneUID())){
      if (val1 > val2){
        c = false;
      }
    }
    else{
      if (val2 > val1){
        c = false;
      }
    }
  }
  return c;
}
