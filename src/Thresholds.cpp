#include "Thresholds.h"
#include <iostream>

extern double doubleCompareValue;
extern int DEBUG_LEVEL;

Thresholds::Thresholds(double A, double R, double V, int RisingAmount, int WindowSize, int NumBins, double InfluenceLevelDelta, double RelaxInitialParentsDelta, int MaxParentSetSize, bool CompeteMultipleHighLowBool)
{
	ta = A;
	tr = R;
	ti = V;
	risingAmount = RisingAmount;
	windowSize = WindowSize;
	numBins = NumBins;
	tm = InfluenceLevelDelta;
	relaxInitialParentsDelta = RelaxInitialParentsDelta;
	maxParentSetSize = MaxParentSetSize;
	competeMultipleHighLowBool = CompeteMultipleHighLowBool;
	//postFilter = PostFilter;
	sip_letNThrough = 2;
	//tp = 0.51;
}

Thresholds::~Thresholds()
{
}

void Thresholds::relaxInitialParentsThresholds(){
	ta -= relaxInitialParentsDelta;
	tr += relaxInitialParentsDelta;
	
	if (ta < 1){
		ta = 1;	
	}
	if (tr > 1){
		tr = 1;	
	}
	
	if (ta <= 1+doubleCompareValue && tr <= 1+doubleCompareValue){
          if (DEBUG_LEVEL>0){
            std::cout << "ERROR: Activation or Represison thresholds reduced too much: " << ta << " " << tr << "\n";
            //exit(0);
          }
	}
}

bool Thresholds::harshenInitialParentsThresholds(){
	ta += relaxInitialParentsDelta;
	tr -= relaxInitialParentsDelta;
	if (ta > 5 || tr < 0){
          if (DEBUG_LEVEL>0){
            std::cout << "ERROR:  Activation or Represison thresholds harshened too much: " << ta << " " << tr << "\n";
          }
          return false;
	}
	return true;
}

