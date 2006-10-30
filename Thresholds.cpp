#include "Thresholds.h"
#include <iostream>

Thresholds::Thresholds(float A, float R, float V, int RisingAmount, int WindowSize, int NumBins, float InfluenceLevelDelta, float RelaxInitialParentsDelta, int MaxParentSetSize, bool CompeteMultipleHighLowBool)
{
	a = A;
	r = R;
	v = V;
	risingAmount = RisingAmount;
	windowSize = WindowSize;
	numBins = NumBins;
	influenceLevelDelta = InfluenceLevelDelta;
	relaxInitialParentsDelta = RelaxInitialParentsDelta;
	maxParentSetSize = MaxParentSetSize;
	competeMultipleHighLowBool = CompeteMultipleHighLowBool;
	//postFilter = PostFilter;
	sip_letNThrough = 1;
}

Thresholds::~Thresholds()
{
}

void Thresholds::relaxInitialParentsThresholds(){
	a -= relaxInitialParentsDelta;
	r += relaxInitialParentsDelta;
	
	if (a < 1){
		a = 1;	
	}
	if (r > 1){
		r = 1;	
	}
	
	if (a == 1 && r == 1){
		std::cout << "ERROR:  Activation or Represison thresholds reduced too much: " << a << " " << r << "\n";
		exit(0);
	}
}

void Thresholds::harshenInitialParentsThresholds(){
	a += relaxInitialParentsDelta;
	r -= relaxInitialParentsDelta;
	if (a > 5 || r < 0){
		std::cout << "ERROR:  Activation or Represison thresholds harshened too much: " << a << " " << r << "\n";
		exit(0);
	}
}

