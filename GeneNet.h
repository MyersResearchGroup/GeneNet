#ifndef GENENET_H_
#define GENENET_H_



void GeneNet(Species& S, Experiments& E, NetCon& C, Thresholds& T, Encodings& L);
void EncodeExpts(Species& S, Experiments& E, Thresholds& T, Encodings& L);
float Score(const Specie& S, const Set& P, const Set& G, const Experiments& E, const Thresholds& T, const Encodings& L);
float ScoreBetter(Specie& s, const Set& P, const Set& G, const Experiments& E, const Thresholds& T, const Encodings& L);
void SelectInitialParents (Specie& s,const Species& S,const Experiments& E, NetCon& C,const Thresholds& T, const Encodings& L);
void CreateMultipleParents(Specie& s, const Species& S, const Experiments& E, NetCon& C, const Thresholds& T, const Encodings& L);
void CompetePossibleParents(Specie& s, const Species& S, const Experiments& E, NetCon& C, const Thresholds& T, const Encodings& L);
vector<DoubleSet> assignMatchups(const Specie& s, const Species& S, const Experiments& E,const NetCon& C, const Thresholds& T, const Encodings& L);
//float Prob(const Specie s, const std::vector<int> * l1, const std::vector<int> * l2);
bool fillFromTSD(const char dir[], Species * S, Experiments * E, NetCon * C, Thresholds* T, Encodings*L);
void writeDot(const char dir[], NetCon * C);
bool yaccParse(FILE * f, Species * S, Experiments * E, int experimentNum);
void callGeneNet(const char * dir, Thresholds & T);



bool incrementBaseSet(int * c, int arraySize, int numberOfBases);
void CreateMultipleParents_Too_Costly(const Specie& s, const Species& S, const Experiments& E, NetCon& C, const Thresholds& T, const Encodings& L);


#endif /*GENENET_H_*/
