#include "Thresholds.h"
#include <iostream>

extern double doubleCompareValue;

Thresholds::Thresholds(double A, double R, double V, int RisingAmount, int WindowSize, int NumBins, double InfluenceLevelDelta, double RelaxInitialParentsDelta, int MaxParentSetSize, bool CompeteMultipleHighLowBool)
{
	tf = A;
	ta = R;
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
	tf -= relaxInitialParentsDelta;
	ta += relaxInitialParentsDelta;
	
	if (tf < 1){
		tf = 1;	
	}
	if (ta > 1){
		ta = 1;	
	}
	
	if (tf <= 1+doubleCompareValue && ta <= 1+doubleCompareValue){
		std::cout << "ERROR:  Activation or Represison thresholds reduced too much: " << tf << " " << ta << "\n";
		//exit(0);
	}
}

bool Thresholds::harshenInitialParentsThresholds(){
	tf += relaxInitialParentsDelta;
	ta -= relaxInitialParentsDelta;
	if (tf > 5 || ta < 0){
		std::cout << "ERROR:  Activation or Represison thresholds harshened too much: " << tf << " " << ta << "\n";
		return false;
	}
	return true;
}

