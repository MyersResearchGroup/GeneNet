#ifndef TSDPOINT_H_
#define TSDPOINT_H_
#include <string>
#include <set>
#include <vector>
#include <map>
#include <algorithm>

class Specie;
class Set;
class Experiments;
class Thresholds;
class Encodings;

class TSDPoint
{
public:
	static void clearAll();
	static void InitializePoints();
	static TSDPoint * getTSDPoint(std::string rowValue);
	static double calculateProbability(int child, int * holdSteady, int * totalSize, int size);
	void updateValue(int * childrenSeen, int * childrenRose, int size);
	bool sameLevels(TSDPoint * i);
	bool cannotCompareLevels(TSDPoint* i);

	std::vector<int> seen;
	std::vector<int> risings;
	friend double ScoreBetter(Specie& s, const Set& P, const Set& G, const Experiments& E, const Thresholds& T, const Encodings& L);
	friend bool areConnected(TSDPoint * bottom, TSDPoint * top, const Set& P, const Encodings& L);
        int LatticeLevel(const Set& P);
        static void setMaxEncodings(const Encodings & L);
protected:
	TSDPoint(std::string rowValues);
	virtual ~TSDPoint();
private:
	bool hasBeenInitialized;
	void updateValueStop(int * childrenSeen, int * childrenRose, int size);
	static bool incrementLevel(int * currentAssignment, int * holdSteady, int * totalSize, int size);
	static bool sortTSDPoints(const TSDPoint * a, const TSDPoint * b);
	static int sortTSDPointsHelper(const TSDPoint * a, const TSDPoint * b, const Set * p);
	static std::map<std::string,TSDPoint*> * thePoints;
	static std::vector<TSDPoint*> * initialValues;
	static int numS;
	bool increaseMinuses(int * v, int stringSize, int size);
	static Set * G;
	static Set * P;
        static std::vector<int> maxEncodings;
	std::string rowValues;
};

#endif /*TSDPOINT_H_*/
