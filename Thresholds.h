#ifndef THRESHOLDS_H_
#define THRESHOLDS_H_


class Thresholds
{
public:
	Thresholds(double A, double R, double V, int RisingAmount, int WindowSize, int NumBins, double InfluenceLevelDelta, double RelaxInitialParentsDelta, int MaxParentSetSize, bool CompeteMultipleHighLowBool);
	double getTF() const {return tf;}
	double getTA() const {return ta;}
	double getTI() const {return ti;}
	int getBins() const {return numBins;}
	int getRisingAmount() const {return risingAmount;}
	int getWindowSize() const {return windowSize;}
	double getTM() const {return tm;}
	double getRelaxInitialParentsDelta() const {return relaxInitialParentsDelta;}
	void relaxInitialParentsThresholds();
	bool harshenInitialParentsThresholds();
	int getMaxParentSetSize() const {return maxParentSetSize;}
	bool competeMultipleHighLow() const {return competeMultipleHighLowBool;}
	void setCompeteMultipleHighLow(bool a) {competeMultipleHighLowBool = a;}


	void setTF(double F) {tf = F;}
	void setTA(double F) {ta = F;}
	void setTI(double F) {ti = F;}
	void setBins(int F) {numBins = F;}
	void setRisingAmount(int F) {risingAmount = F;}
	void setWindowSize(int F) {windowSize = F;}
	void setTM(double F) {tm = F;}
	void setRelaxInitialParentsDelta(double F) {relaxInitialParentsDelta = F;}
	void setMaxParentSetSize(int F ) {maxParentSetSize = F;}
	
	void setsip_letNThrough(int i){sip_letNThrough = i;}
	int getsip_letNThrough() const {return sip_letNThrough;}

	//double getPostFilter(){return postFilter;}
	virtual ~Thresholds();
private:
	double tf;
	double ta;
	double ti;
	double tm;
	double tp;
	int risingAmount;
	int windowSize;
	int numBins;
	double relaxInitialParentsDelta;
	int maxParentSetSize;
	bool competeMultipleHighLowBool;
	//double postFilter
	int sip_letNThrough;
};

#endif /*THRESHOLDS_H_*/
