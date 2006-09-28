#ifndef THRESHOLDS_H_
#define THRESHOLDS_H_

class Thresholds
{
public:
	Thresholds(float A, float R, float V, int RisingAmount, int WindowSize, int NumBins, float InfluenceLevelDelta, float RelaxInitialParentsDelta);
	float getA() const {return a;}
	float getR() const {return r;}
	float getV() const {return v;}
	int getBins() const {return numBins;}
	int getRisingAmount() const {return risingAmount;}
	int getWindowSize() const {return windowSize;}
	float getInfluenceLevelDelta() const {return influenceLevelDelta;}
	float getRelaxInitialParentsDelta() const {return relaxInitialParentsDelta;}
	void relaxInitialParentsThresholds();
	void harshenInitialParentsThresholds();

	void setA(float F) {a = F;}
	void setR(float F) {r = F;}
	void setV(float F) {v = F;}
	void setBins(int F) {numBins = F;}
	void setRisingAmount(int F) {risingAmount = F;}
	void setWindowSize(int F) {windowSize = F;}
	void setInfluenceLevelDelta(float F) {influenceLevelDelta = F;}
	void setRelaxInitialParentsDelta(float F) {relaxInitialParentsDelta = F;}

	//float getPostFilter(){return postFilter;}
	virtual ~Thresholds();
private:
	float a;
	float r;
	float v;
	int risingAmount;
	int windowSize;
	int numBins;
	float influenceLevelDelta;
	float relaxInitialParentsDelta;
	//float postFilter
};

#endif /*THRESHOLDS_H_*/
