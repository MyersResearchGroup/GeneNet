#ifndef THRESHOLDS_H_
#define THRESHOLDS_H_


class Thresholds
{
public:
	Thresholds(float A, float R, float V, int RisingAmount, int WindowSize, int NumBins, float InfluenceLevelDelta, float RelaxInitialParentsDelta, int MaxParentSetSize, bool CompeteMultipleHighLowBool);
	float getTF() const {return tf;}
	float getTA() const {return ta;}
	float getTI() const {return ti;}
	int getBins() const {return numBins;}
	int getRisingAmount() const {return risingAmount;}
	int getWindowSize() const {return windowSize;}
	float getTM() const {return tm;}
	float getRelaxInitialParentsDelta() const {return relaxInitialParentsDelta;}
	void relaxInitialParentsThresholds();
	bool harshenInitialParentsThresholds();
	int getMaxParentSetSize() const {return maxParentSetSize;}
	bool competeMultipleHighLow() const {return competeMultipleHighLowBool;}
	void setCompeteMultipleHighLow(bool a) {competeMultipleHighLowBool = a;}


	void setTF(float F) {tf = F;}
	void setTA(float F) {ta = F;}
	void setTI(float F) {ti = F;}
	void setBins(int F) {numBins = F;}
	void setRisingAmount(int F) {risingAmount = F;}
	void setWindowSize(int F) {windowSize = F;}
	void setTM(float F) {tm = F;}
	void setRelaxInitialParentsDelta(float F) {relaxInitialParentsDelta = F;}
	void setMaxParentSetSize(int F ) {maxParentSetSize = F;}
	
	void setsip_letNThrough(int i){sip_letNThrough = i;}
	int getsip_letNThrough() const {return sip_letNThrough;}

	//float getPostFilter(){return postFilter;}
	virtual ~Thresholds();
private:
	float tf;
	float ta;
	float ti;
	int risingAmount;
	int windowSize;
	int numBins;
	float tm;
	float relaxInitialParentsDelta;
	int maxParentSetSize;
	bool competeMultipleHighLowBool;
	//float postFilter
	int sip_letNThrough;
	float tp;
};

#endif /*THRESHOLDS_H_*/
