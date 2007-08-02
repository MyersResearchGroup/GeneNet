#ifndef LEVELASSIGNMENTS_H_
#define LEVELASSIGNMENTS_H_
#include <vector>

class Set;
class Encodings;
class DoubleSet;

class LevelAssignments
{
public:
	LevelAssignments();
	LevelAssignments(const LevelAssignments & s);
	LevelAssignments(const Set& P, const Encodings& L);
	LevelAssignments(const DoubleSet& G, const Encodings& L);
	int size();
	std::vector<int> * get(int i);
	virtual ~LevelAssignments();
	
	const LevelAssignments & operator=(const LevelAssignments & s);
	
private:
	void deleteAssignments(void);
	void fillAssignments(const LevelAssignments & s);
	void creationHelper(const Set& P, const Encodings& L);
	bool incrementLevel(int * currentAssignment, std::vector<double> * info, int size);
	std::vector<std::vector<int> * > myAssignments;
	//Set * mySet;
};

#endif /*LEVELASSIGNMENTS_H_*/
