#include "Thresholds.h"
#include <iostream>

extern float floatCompareValue;

Thresholds::Thresholds(float A, float R, float V, int RisingAmount, int WindowSize, int NumBins, float InfluenceLevelDelta, float RelaxInitialParentsDelta, int MaxParentSetSize, bool CompeteMultipleHighLowBool)
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
	sip_letNThrough = 1;
	tp = 0.51;
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
	
	if (tf <= 1+floatCompareValue && ta <= 1+floatCompareValue){
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

