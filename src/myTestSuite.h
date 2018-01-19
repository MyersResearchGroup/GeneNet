#ifndef MYTESTSUITE_H_
#define MYTESTSUITE_H_

#endif /*MYTESTSUITE_H_*/

#include <cxxtest/TestSuite.h>
#include <iostream>
#include "Thresholds.h"
#include "Experiments.h"
#include "Specie.h"
#include "Species.h"
#include "Set.h"
#include "DoubleSet.h"
#include "Encodings.h"
#include "NetCon.h"
#include "LevelAssignments.h"
#include "TSDPoint.h"
#include "GeneNet.h"


class MyTestSuite : public CxxTest::TestSuite 
{
public:
	void tearDown(void){
		Specie::clearAllInstances();
		TSDPoint::clearAll();
	}
	
	void testThresholds(void){
		Thresholds t(1,2,3,4,5,-1, 0.01,0.025,2,false);
		TS_ASSERT_DELTA(t.getTF(),1,0.0001);
		TS_ASSERT_DELTA(t.getTA(),2,0.0001);
		TS_ASSERT_DELTA(t.getTI(),3,0.0001);
		TS_ASSERT_EQUALS(t.getRisingAmount(),4);
		TS_ASSERT_EQUALS(t.getWindowSize(),5);
	}
	void testSpecie(void){
		//specie creation
		Specie * s = NULL;
		Specie * p = NULL;
		s = Specie::getInstance("test",45);
		TS_ASSERT_EQUALS(s->getGeneUID(),45);
		TS_ASSERT_EQUALS(s->getGeneName(),"test");
		//same specie
		TS_ASSERT(p = Specie::getInstance("test",45));
		TS_ASSERT_EQUALS(*p == *s, true);

		//different species
		p = Specie::getInstance("test",44);
		TS_ASSERT_EQUALS(*p == *s, false);
		
		
		//Test the sets and double sets
		TS_ASSERT(p->toSet()->containsSpecieID(p->getGeneUID()));
		TS_ASSERT(s->toSet()->containsSpecieID(s->getGeneUID()));
		TS_ASSERT(p->toSet()->contains(*p->toSet()));
		TS_ASSERT(s->toSet()->contains(*s->toSet()));
		TS_ASSERT(p->toDoubleSet().contains(*p->toSet()));
		TS_ASSERT(s->toDoubleSet().contains(*s->toSet()));

	}
	
	void testSpecies(void){
		Species all;
		Specie * one = Specie::getInstance("one",0);
		Specie * two = Specie::getInstance("two",1);
		Specie * three = Specie::getInstance("three",2);
		TS_ASSERT(all.addSpecie(one));
		TS_ASSERT_EQUALS(all.size(),1);
		TS_ASSERT_EQUALS(all.addSpecie(three),false);
		TS_ASSERT_EQUALS(all.size(),1);
		TS_ASSERT(all.addSpecie(two));
		TS_ASSERT_EQUALS(all.size(),2);
		TS_ASSERT(all.addSpecie(three));	
		TS_ASSERT_EQUALS(all.size(),3);
		TS_ASSERT_EQUALS(one,all.get(0));
		TS_ASSERT_EQUALS(two,all.get(1));
		TS_ASSERT_EQUALS(three,all.get(2));
		TS_ASSERT_EQUALS(all.size(),3);
		//allow adding in the same species
		Specie * o = Specie::getInstance("one",0);
		TS_ASSERT(all.addSpecie(o));
		TS_ASSERT(all.addSpecie(one));
		TS_ASSERT(all.addSpecie(two));
		TS_ASSERT(all.addSpecie(three));

	}

	void testExperiments( void )
    {
    	
   		Experiments exp;
   		//adding in bogus time points and values
   		TS_ASSERT_EQUALS(exp.addTimePoint(-1,0,0,1), false);
   		TS_ASSERT_EQUALS(exp.addTimePoint(0,0,0,-0.5),false);
   		TS_ASSERT_EQUALS(exp.addTimePoint(5,5,5,1),false);
   		TS_ASSERT(exp.getRow(0,0) == NULL);
   		TS_ASSERT(exp.getRow(-1,0) == NULL);
   		TS_ASSERT(exp.getRow(0,-1) == NULL);
   		TS_ASSERT(exp.getRow(2,0) == NULL);
   		TS_ASSERT(exp.getRow(0,2) == NULL);
		//adding in good values
   		TS_ASSERT(exp.addTimePoint(0,0,0,7));
   		TS_ASSERT(exp.addTimePoint(0,0,1,8.9));
   		TS_ASSERT(exp.addTimePoint(0,1,0,1.8));
   		TS_ASSERT(exp.addTimePoint(0,1,1,1.8));
   		TS_ASSERT(exp.addTimePoint(1,0,0,100.05));
   		TS_ASSERT(exp.addTimePoint(1,0,1,100.05));
   		TS_ASSERT(exp.addTimePoint(2,0,0,100.06));
   		TS_ASSERT(exp.addTimePoint(2,0,1,100.06));
   		TS_ASSERT(exp.addTimePoint(3,0,0,0));
   		//retrying same point
   		TS_ASSERT_EQUALS(exp.addTimePoint(0,0,0,7),false);
   		//trying to skip data points
   		TS_ASSERT(!exp.addTimePoint(4,0,0,0));
   		TS_ASSERT(!exp.addTimePoint(3,1,0,0));
   		//getting good and bad data
   		TS_ASSERT_DELTA(exp.getTimePoint(0,0,0),7,0.001);
   		TS_ASSERT_DELTA(exp.getTimePoint(0,0,-1),-1,0.001);
   		TS_ASSERT_DELTA(exp.getTimePoint(0,-1,0),-1,0.001);
   		TS_ASSERT_DELTA(exp.getTimePoint(-1,0,0),-1,0.001);
   		TS_ASSERT_DELTA(exp.getTimePoint(0,0,1),8.9,0.001);
   		TS_ASSERT_DELTA(exp.getTimePoint(0,1,0),1.8,0.001);
   		TS_ASSERT_DELTA(exp.getTimePoint(1,0,0),100.05,0.001);
   		TS_ASSERT_DELTA(exp.getTimePoint(1,0,2),-1,0.001);

		//Testing the total Experiments
		TS_ASSERT_EQUALS(exp.totalExperiments(),4);
		//Testing the rows for each experiment;
		TS_ASSERT_EQUALS(exp.totalRows(-1),0);
		TS_ASSERT_EQUALS(exp.totalRows(0),2);
		TS_ASSERT_EQUALS(exp.totalRows(1),1);
		TS_ASSERT_EQUALS(exp.totalRows(2),1);
		TS_ASSERT_EQUALS(exp.totalRows(3),1);
		TS_ASSERT_EQUALS(exp.totalRows(4),0);

		//Test the get row command
   		TS_ASSERT(exp.getRow(-1,0) == NULL);
   		TS_ASSERT(exp.getRow(0,-1) == NULL);
   		TS_ASSERT(exp.getRow(4,0) == NULL);
   		TS_ASSERT(exp.getRow(0,4) == NULL);
		std::vector<double> * row = exp.getRow(0,0);
		TS_ASSERT(row != NULL);
		TS_ASSERT_EQUALS((int)row->size(),2);
		TS_ASSERT_DELTA(row->at(0),7,0.0001);
		TS_ASSERT_DELTA(row->at(1),8.9,0.0001);
		row = exp.getRow(0,1);
		TS_ASSERT(row != NULL);
		TS_ASSERT_EQUALS((int)row->size(),2);
		TS_ASSERT_DELTA(row->at(0),1.8,0.0001);
		TS_ASSERT_DELTA(row->at(1),1.8,0.0001);
		row = exp.getRow(1,0);
		TS_ASSERT(row != NULL);
		TS_ASSERT_EQUALS((int)row->size(),2);
		TS_ASSERT_DELTA(row->at(0),100.05,0.0001);
		TS_ASSERT_DELTA(row->at(1),100.05,0.0001);
		row = exp.getRow(2,0);
		TS_ASSERT(row != NULL);
		TS_ASSERT_EQUALS((int)row->size(),2);
		TS_ASSERT_DELTA(row->at(0),100.06,0.0001);
		TS_ASSERT_DELTA(row->at(1),100.06,0.0001);

   		
   		//Test the bins
   		Experiments exp1;
   		TS_ASSERT(exp1.addTimePoint(0,0,0,9.9));
   		TS_ASSERT(exp1.addTimePoint(0,0,1,4.4)); //a different species at column 1
   		TS_ASSERT(exp1.addTimePoint(0,1,0,3.3));
   		TS_ASSERT(exp1.addTimePoint(0,1,1,4.5)); //a different species at column 1
   		TS_ASSERT(exp1.addTimePoint(1,0,0,100.9));
   		TS_ASSERT(exp1.addTimePoint(1,0,1,4.6));
   		TS_ASSERT(exp1.addTimePoint(2,0,0,9.9));
   		TS_ASSERT(exp1.addTimePoint(2,0,1,4.7)); //a different species at column 1
   		std::vector<double> f = exp1.getSortedValues(0);
   		TS_ASSERT_EQUALS((int)f.size(),4);
   		TS_ASSERT_DELTA(f.at(0),3.3,0.0001);
   		TS_ASSERT_DELTA(f.at(1),9.9,0.0001);
   		TS_ASSERT_DELTA(f.at(2),9.9,0.0001);
   		TS_ASSERT_DELTA(f.at(3),100.9,0.0001);
   		//cout << "Sorted test\n";
   		f = exp1.getSortedValues(1);
   		TS_ASSERT_EQUALS((int)f.size(),4);
   		TS_ASSERT_DELTA(f.at(0),4.4,0.0001);
   		TS_ASSERT_DELTA(f.at(1),4.5,0.0001);
   		TS_ASSERT_DELTA(f.at(2),4.6,0.0001);
   		TS_ASSERT_DELTA(f.at(3),4.7,0.0001);
   		
   		//TODO:
   		//Test the mutations
   		Specie * s = Specie::getInstance("test",0);
   		Experiments mutations = exp1.removeMutations(s);

   		//Test some assignments
   		exp1 = exp1;
   		Experiments tmp(exp1);
   		exp1 = tmp;


    }
    
    void testSet(void){
		cout << "Starting set\n";
    	Specie * s[10];
    	Set evenSet;
    	Set oddSet;
    	
    	
    	//Adding things into a set
    	for (int i = 0; i < 10; i++){
    		s[i] = Specie::getInstance("test",i);
    	}
    	for (int i = 0; i < 10; i = i + 2){
    		evenSet.insert(s[i],0.001);
    	}
    	for (int i = 1; i < 10; i = i + 2){
    		oddSet.insert(s[i],0.001);
    	}
    	for (int i = 0; i < 10; i = i + 2){
    		TS_ASSERT_EQUALS(s[i],evenSet.get((int) i/2));
    	}
    	//Two sets are different
		TS_ASSERT_DIFFERS(oddSet,evenSet);
		
		//Two sets contain 5 items
		TS_ASSERT_EQUALS(oddSet.size(),5);
		TS_ASSERT_EQUALS(evenSet.size(),5);
		
		//test the contains specie function
    	for (int i = 0; i < 10; i = i + 2){
			TS_ASSERT(evenSet.containsSpecieID(i));
    	}
    	for (int i = 1; i < 10; i = i + 2){
			TS_ASSERT(!evenSet.containsSpecieID(i));
    	}

		//testing the contains set function
		Set someEven;
		Set someOdd;
		for (int i = 0; i < 5; i=i+2){
			someEven.insert(s[i],-1);
		}
		someOdd.insert(s[9],-1);
		someOdd.insert(s[3],-1);
		someOdd.insert(s[5],-1);
		TS_ASSERT(evenSet.contains(someEven));
		TS_ASSERT(oddSet.contains(someOdd));
		Set odd2(someOdd);
		odd2.insert(s[1],-1);
		odd2.insert(s[7],-1);
		TS_ASSERT(oddSet.contains(odd2));
		TS_ASSERT(odd2.contains(oddSet));
		TS_ASSERT(oddSet.contains(oddSet));

		//testing some overloaded functions
		TS_ASSERT(oddSet == odd2);

		//testing non inclusion
		Set mixed2(someOdd);
		mixed2.insert(s[2],-1);
		TS_ASSERT(!oddSet.contains(mixed2));
		
		//testing the overloaded functions
		Set tmp = oddSet - evenSet;
		TS_ASSERT_EQUALS(oddSet.size(),5);
		TS_ASSERT_EQUALS(evenSet.size(),5);
		TS_ASSERT_EQUALS(tmp.size(),5);
		for (int i = 1; i < 10; i+=2){
			TS_ASSERT(tmp.containsSpecieID(i));
			TS_ASSERT(oddSet.containsSpecieID(i));
			TS_ASSERT(evenSet.containsSpecieID(i-1));
		}
		tmp = oddSet - someOdd;
		TS_ASSERT_EQUALS(tmp.size(),2);
		TS_ASSERT(tmp.containsSpecieID(1));
		TS_ASSERT(tmp.containsSpecieID(7));
		
		someOdd = oddSet;
		TS_ASSERT(someOdd.size() == 5);
		TS_ASSERT(oddSet == someOdd);
		TS_ASSERT(someEven < evenSet);
		TS_ASSERT(! (someEven > evenSet));
		TS_ASSERT(evenSet > someEven);
		TS_ASSERT(someEven != evenSet);
		TS_ASSERT(!(someOdd < oddSet));
		TS_ASSERT(!(someOdd > oddSet));
		TS_ASSERT(!(oddSet < someOdd));
		TS_ASSERT(!(oddSet > someOdd));
		
		//Test some assignments
		Set tmp2(someOdd);
		someOdd = someEven;
		tmp2 = tmp2;
		
		//Test the set and get score
		TS_ASSERT_DELTA(someOdd.getScore(),0,0.0001);
		TS_ASSERT_DELTA(someEven.getScore(),0,0.0001);
		TS_ASSERT_DELTA(oddSet.getScore(),0,0.0001);
		TS_ASSERT_DELTA(evenSet.getScore(),0,0.0001);
		someOdd.setScore(-1,-1);
		someEven.setScore(-1,1);
		TS_ASSERT_DELTA(someOdd.getScore(),-1,0.0001);
		TS_ASSERT_DELTA(someEven.getScore(),1,0.0001);
		Set tmp3(someOdd);
		TS_ASSERT_DELTA(tmp3.getScore(),-1,0.0001);
		tmp3 = someEven;
		TS_ASSERT_DELTA(tmp3.getScore(),1,0.0001);
		
		//Test some of the delta equals and < tests
		Set e1(oddSet);
		Set e2(oddSet);
		Set e3(oddSet);
		Set e4(oddSet);
		e1.setScore(-1,0.1);
		e2.setScore(-1,0.1+0.9*Set::DELTA);
		e3.setScore(-1,0.1-0.9*Set::DELTA);
		e4.setScore(-1,0.1 + 1.1*Set::DELTA);
		TS_ASSERT_EQUALS(e1,e2);
		TS_ASSERT_EQUALS(e1,e3);
		TS_ASSERT_DIFFERS(e1, e4);
		TS_ASSERT(! (e1 < e2));
		TS_ASSERT(! (e2 < e1));
		TS_ASSERT(! (e1 < e3));
		TS_ASSERT(! (e3 < e1));
		TS_ASSERT(e1 < e4);
		
		//test the score functions
		Set score;
		score.insert(s[1],0.5);
		score.insert(s[2],-0.4);
		score.setScore(-1,0.2);
		TS_ASSERT_DELTA(0.5,score.getIndividualScore(1),0.0001);
		TS_ASSERT_DELTA(-0.4,score.getIndividualScore(2),0.0001);
		TS_ASSERT_DELTA(0.2,score.getScore(),0.0001);
		score.setScore(1,-0.9);
		score.setScore(2,0.8);
		score.setScore(-1,-0.3);
		TS_ASSERT_DELTA(-0.9,score.getIndividualScore(1),0.0001);
		TS_ASSERT_DELTA(0.8,score.getIndividualScore(2),0.0001);
		TS_ASSERT_DELTA(-0.3,score.getScore(),0.0001);
		
		//test the activator function
		TS_ASSERT(score.sortsLowToHigh(2));
		TS_ASSERT(!score.sortsLowToHigh(1));
    }
    void testDoubleSet(void){
    	Specie * s[10];
    	Set lowerEven, lowerOdd, allEven, allOdd, mixed;
    	
    	//setup the species
    	for (int i = 0; i < 10; i++){
    		s[i] = Specie::getInstance("test",i);
    	}
		lowerEven.insert(s[0],0.001);
		lowerEven.insert(s[2],0.001);
		lowerEven.insert(s[4],0.001);
		allEven = lowerEven;
		allEven.insert(s[6],0.001);
		allEven.insert(s[8],0.001);
		lowerOdd.insert(s[1],0.001);
		lowerOdd.insert(s[3],0.001);
		lowerOdd.insert(s[5],0.001);
		allOdd = lowerOdd;
		allOdd.insert(s[7],0.001);
		allOdd.insert(s[9],0.001);
		mixed.insert(s[8],0.001);
		mixed.insert(s[4],0.001);
		mixed.insert(s[7],0.001);
		mixed.insert(s[1],0.001);

		DoubleSet evens;
		DoubleSet odds;
		DoubleSet mix;

		//Test the adding to the set
		evens.unionIt(lowerEven);
		TS_ASSERT(evens.size() == 1);
		evens.unionIt(allEven);
		TS_ASSERT(evens.size() == 2);
		//Test the union
		odds.unionIt(lowerOdd);		
		odds.unionIt(allOdd);
		TS_ASSERT(odds.size() == 2);
		TS_ASSERT(odds.contains(allOdd));
		TS_ASSERT(odds.contains(lowerOdd));
		//More advanced union
		odds.unionIt(lowerOdd);		
		odds.unionIt(allOdd);
		TS_ASSERT(odds.size() == 2);
		TS_ASSERT(odds.contains(allOdd));
		TS_ASSERT(odds.contains(lowerOdd));

		//Test the double union return
		mix = unionIt(odds, mixed);
		mix = unionIt(mix, lowerEven);
		mix = unionIt(mix, allEven);
		TS_ASSERT_EQUALS(mix.size(), 5);
		TS_ASSERT(mix.contains(lowerOdd));
		TS_ASSERT(mix.contains(allOdd));
		TS_ASSERT(mix.contains(lowerEven));
		TS_ASSERT(mix.contains(allEven));
		TS_ASSERT(mix.contains(mixed));
		

		//Test the subsets
		odds.removeSubsets();
		TS_ASSERT(odds.size() == 1);
		TS_ASSERT(odds.contains(allOdd));

		//Test the remove
		odds.unionIt(lowerOdd);
		TS_ASSERT(odds.size() == 2);
		odds.remove(allOdd);
		TS_ASSERT(odds.size() == 1);
		TS_ASSERT(odds.contains(lowerOdd));

		//Test the get
		odds.unionIt(allOdd);
		TS_ASSERT(lowerOdd == *odds.get(0));
		TS_ASSERT(odds.size() == 2);
		//Test remove
		TS_ASSERT(lowerOdd == odds.remove(0));
		TS_ASSERT(odds.size() == 1);
		
		//Test assignment
		DoubleSet assign;
		assign.unionIt(allOdd);
		TS_ASSERT(assign.contains(allOdd));
		assign = evens;
		TS_ASSERT_EQUALS(assign.size(), 2);
		TS_ASSERT(assign.contains(lowerEven));
		TS_ASSERT(assign.contains(allEven));
		
		
		//Test the colapseToSet function
		TS_ASSERT_EQUALS(allEven,evens.colapseToSet());
		odds.unionIt(lowerOdd);
		odds.unionIt(allOdd);
		TS_ASSERT_EQUALS(allOdd,odds.colapseToSet());
		
		//Test the addIfScoreBetterThanSubsets(const Set & s);
    	lowerEven.setScore(-1,0);
    	lowerOdd.setScore(-1,1);
    	allEven.setScore(-1,1.1);
    	allOdd.setScore(-1,0.9);
    	mixed.setScore(-1,5);
		DoubleSet scoreTest;
		scoreTest.addIfScoreBetterThanSubsets(lowerEven);
		TS_ASSERT_EQUALS(scoreTest.size(),1);
		TS_ASSERT_EQUALS(*scoreTest.get(0),lowerEven);
		TS_ASSERT_DELTA(scoreTest.get(0)->getScore(),0,0.001);
		//The same one, but a better score
    	lowerEven.setScore(-1,1);
		scoreTest.addIfScoreBetterThanSubsets(lowerEven);
		TS_ASSERT_EQUALS(scoreTest.size(),2);
		scoreTest.removeSubsets();
		TS_ASSERT_EQUALS(scoreTest.size(),1);
		TS_ASSERT_EQUALS(*scoreTest.get(0),lowerEven);
		TS_ASSERT_DELTA(scoreTest.get(0)->getScore(),1,0.001);
		//Do the same thing, but different now
    	lowerEven.setScore(-1,0);
		scoreTest.unionIt(lowerEven);
		TS_ASSERT_EQUALS(scoreTest.size(),2);
		scoreTest.removeSubsets();
		TS_ASSERT_EQUALS(scoreTest.size(),1);
		TS_ASSERT_DELTA(scoreTest.get(0)->getScore(),1,0.001);
		//Put in a lot of lowerEvens
    	lowerEven.setScore(-1,0.2);
		scoreTest.unionIt(lowerEven);
		TS_ASSERT_EQUALS(scoreTest.size(),2);
    	lowerEven.setScore(-1,0.3);
		scoreTest.unionIt(lowerEven);
		TS_ASSERT_EQUALS(scoreTest.size(),3);
    	lowerEven.setScore(-1,0.4);
		scoreTest.unionIt(lowerEven);
		TS_ASSERT_EQUALS(scoreTest.size(),4);
    	lowerEven.setScore(-1,1);
		scoreTest.unionIt(lowerEven);
		TS_ASSERT_EQUALS(scoreTest.size(),4);
		scoreTest.removeSubsets();
		TS_ASSERT_EQUALS(scoreTest.size(),1);
		TS_ASSERT_DELTA(scoreTest.get(0)->getScore(),1,0.001);
		
		//Two different ones
		scoreTest.addIfScoreBetterThanSubsets(lowerOdd);
		TS_ASSERT_EQUALS(scoreTest.size(),2);
		TS_ASSERT_EQUALS(*scoreTest.get(0),lowerEven);
		TS_ASSERT_EQUALS(*scoreTest.get(1),lowerOdd);
		//Larger Set that is better
		scoreTest.addIfScoreBetterThanSubsets(allEven);
		TS_ASSERT_EQUALS(scoreTest.size(),3);
		TS_ASSERT_EQUALS(*scoreTest.get(2),allEven);
		//Larget set that is worse
		scoreTest.addIfScoreBetterThanSubsets(allOdd);
		TS_ASSERT_EQUALS(scoreTest.size(),3);
		TS_ASSERT_EQUALS(*scoreTest.get(0),lowerEven);
		TS_ASSERT_EQUALS(*scoreTest.get(1),lowerOdd);
		TS_ASSERT_EQUALS(*scoreTest.get(2),allEven);
		scoreTest.addIfScoreBetterThanSubsets(mixed);
		TS_ASSERT_EQUALS(scoreTest.size(),4);
		TS_ASSERT(scoreTest.contains(mixed));
		
		//Test some assignments
		DoubleSet tmp(odds);
		odds = tmp;
		tmp = tmp;
		
    }

    void testEncodings(void){
    	//test the bins
   		Experiments exp1;
		Thresholds T(1.15,0.75,0.00,1,1,-1,0.01,0.025,2,false);
   		TS_ASSERT(exp1.addTimePoint(0,0,0,1));
   		TS_ASSERT(exp1.addTimePoint(0,0,1,1)); //different species at 1 and 2
   		TS_ASSERT(exp1.addTimePoint(0,0,2,4.4));
   		TS_ASSERT(exp1.addTimePoint(0,1,0,2));
   		TS_ASSERT(exp1.addTimePoint(0,1,1,2));
   		TS_ASSERT(exp1.addTimePoint(0,1,2,4.5));
   		TS_ASSERT(exp1.addTimePoint(1,0,0,3));
   		TS_ASSERT(exp1.addTimePoint(1,0,1,3));
   		TS_ASSERT(exp1.addTimePoint(1,0,2,100.9));
   		TS_ASSERT(exp1.addTimePoint(2,0,0,4));
   		TS_ASSERT(exp1.addTimePoint(2,0,1,4));
   		TS_ASSERT(exp1.addTimePoint(2,0,2,9.9));
   		TS_ASSERT(exp1.addTimePoint(2,1,0,5));
   		TS_ASSERT(exp1.addTimePoint(2,1,1,5));
   		TS_ASSERT(exp1.addTimePoint(2,1,2,110.9));
   		TS_ASSERT(exp1.addTimePoint(2,2,0,6));
   		TS_ASSERT(exp1.addTimePoint(2,2,1,6));
   		TS_ASSERT(exp1.addTimePoint(2,2,2,101.9));
   		Specie * time = Specie::getInstance("time",0);
   		Specie * one = Specie::getInstance("one",1);
   		Specie * two = Specie::getInstance("two",2);
   		Species s;
   		s.addSpecie(time);
   		s.addSpecie(one);
   		s.addSpecie(two);
   		s.removeTimeSpecie();
   		Encodings e;
   		//check for false startup
   		TS_ASSERT(!e.useBins(2));
   		//two levels
   		e.initialize(&s,&exp1,&T);
   		TS_ASSERT_EQUALS(e.totalSpecies(), 2);
   		TS_ASSERT(e.useBins(2));
   		std::vector<double> f1 = e.getLevels(one);
   		std::vector<double> f2 = e.getLevels(two);
   		TS_ASSERT_EQUALS((int)f1.size(),1);
   		TS_ASSERT_EQUALS(f1.at(0),3);
   		TS_ASSERT_EQUALS((int)f2.size(),1);
   		TS_ASSERT_DELTA(f2.at(0),9.9,0.0001);
   		//Check for the probabilities;
   		Set some;
   		some.insert(one,0.001);
   		some.insert(two,0.001);
   		std::vector<int> levels;
   		levels.push_back(-1);
   		levels.push_back(0);
   		levels.push_back(-1);
   		TS_ASSERT_DELTA(e.getProb(one,&levels, &levels),1.0,0.0001);
   		levels.clear(); levels.push_back(-1); levels.push_back(1); levels.push_back(-1);
   		TS_ASSERT_DELTA(e.getProb(one,&levels, &levels),1.0,0.0001);
   		levels.clear(); levels.push_back(-1); levels.push_back(-1); levels.push_back(0);
   		TS_ASSERT_DELTA(e.getProb(two,&levels, &levels),0.5,0.0001);
   		levels.clear(); levels.push_back(-1); levels.push_back(-1); levels.push_back(1);
   		TS_ASSERT_DELTA(e.getProb(two,&levels, &levels),0.0,0.0001);
   		levels.clear(); levels.push_back(-1); levels.push_back(0); levels.push_back(0);
   		TS_ASSERT_DELTA(e.getProb(one,&levels, &levels),1.0,0.0001);
   		TS_ASSERT_DELTA(e.getProb(two,&levels, &levels),0.0,0.0001);
   		levels.clear(); levels.push_back(-1); levels.push_back(0); levels.push_back(1);
   		TS_ASSERT_DELTA(e.getProb(one,&levels, &levels),-1.0,0.0001);
   		TS_ASSERT_DELTA(e.getProb(two,&levels, &levels),-1.0,0.0001);
   		levels.clear(); levels.push_back(-1); levels.push_back(1); levels.push_back(0);
   		TS_ASSERT_DELTA(e.getProb(one,&levels, &levels),1.0,0.0001);
   		TS_ASSERT_DELTA(e.getProb(two,&levels, &levels),1.0,0.0001);
   		levels.clear(); levels.push_back(-1); levels.push_back(1); levels.push_back(1);
   		TS_ASSERT_DELTA(e.getProb(one,&levels, &levels),1.0,0.0001);
   		TS_ASSERT_DELTA(e.getProb(two,&levels, &levels),0.0,0.0001);
   		
   		//four levels
   		TS_ASSERT(e.useBins(3));
   		f1 = e.getLevels(one);
   		TS_ASSERT_EQUALS((int)f1.size(),2);
   		TS_ASSERT_DELTA(f1.at(0),2,0.0001);
   		TS_ASSERT_DELTA(f1.at(1),4,0.0001);
   		f2 = e.getLevels(two);
   		TS_ASSERT_EQUALS((int)f2.size(),2);
   		TS_ASSERT_DELTA(f2.at(0),4.5,0.0001);
   		TS_ASSERT_DELTA(f2.at(1),100.9,0.0001);
   		//too many levels
   		TS_ASSERT(!e.useBins(7));
   		
   		//check the numbers function
   		TS_ASSERT(e.useNumbers(2))
   		f1 = e.getLevels(one);
   		f2 = e.getLevels(two);
		TS_ASSERT_EQUALS((int)f1.size(),1);
		TS_ASSERT_EQUALS((int)f2.size(),1);
		TS_ASSERT_DELTA(f1.at(0),3.5,0.0001);
		TS_ASSERT_DELTA(f2.at(0),57.65,0.0001);
		//a little harder one
   		TS_ASSERT(e.useNumbers(4))
   		f1 = e.getLevels(one);
   		f2 = e.getLevels(two);
		TS_ASSERT_EQUALS((int)f1.size(),3);
		TS_ASSERT_EQUALS((int)f2.size(),3);
		TS_ASSERT_DELTA(f1.at(0),2.25,0.0001);
		TS_ASSERT_DELTA(f1.at(1),3.5,0.0001);
		TS_ASSERT_DELTA(f1.at(2),4.75,0.0001);
		TS_ASSERT_DELTA(f2.at(0),26.625 + 4.4,0.0001);
		TS_ASSERT_DELTA(f2.at(1),26.625*2 + 4.4,0.0001);
		TS_ASSERT_DELTA(f2.at(2),26.625*3 + 4.4,0.0001);

		//let do a harder one
   		//two levels
   		Experiments harder;
		for (int i = 0; i < 10; i++){
			for (int j = 0; j < 10; j++){
		   		TS_ASSERT(harder.addTimePoint(i,j,0,j));
		   		TS_ASSERT(harder.addTimePoint(i,j,1,i));
		   		TS_ASSERT(harder.addTimePoint(i,j,2,j));
			}
		}
   		e.initialize(&s,&harder,&T);
   		TS_ASSERT_EQUALS(e.totalSpecies(), 2);
   		TS_ASSERT(e.useBins(5));
		for (int i = 0; i < 5; i++){
				levels.clear();
				levels.push_back(-1);
				levels.push_back(i);
				levels.push_back(-1);
		   		TS_ASSERT_DELTA(e.getProb(one  ,&levels, &levels),0.0,0.0001);
				levels.clear();
				levels.push_back(-1);
				levels.push_back(-1);
				levels.push_back(i);
		   		TS_ASSERT_DELTA(e.getProb(two  ,&levels, &levels),1.0,0.0001);
		}
		for (int i = 0; i < 5; i++){
			for (int j = 0; j < 5; j++){
				levels.clear();
				levels.push_back(-1);
				levels.push_back(i);
				levels.push_back(j);
		   		TS_ASSERT_DELTA(e.getProb(one  ,&levels, &levels),0.0,0.0001);
		   		TS_ASSERT_DELTA(e.getProb(two  ,&levels, &levels),1.0,0.0001);
			}
		}		
    }
    void testNetCon(void){
    	Specie * s[10];
    	Set sets[10];
    	for (int i = 0; i < 10; i++){
    		s[i] = Specie::getInstance("test",i);
    		sets[i].insert(s[i],0.001);
    	}

		//Test the empty cases
		NetCon netTest;
		TS_ASSERT(!netTest.containsAnyEdges(*s[0]));
		TS_ASSERT(!netTest.containsAnyEdges(*s[9]));
		TS_ASSERT_EQUALS(netTest.getSingleParentsFor(*s[0]).size(),0);
		TS_ASSERT_EQUALS(netTest.getSingleParentsFor(*s[4]).size(),0);
		TS_ASSERT_EQUALS(netTest.totalParents(*s[0]),0);
		TS_ASSERT_EQUALS(netTest.totalParents(*s[4]),0);

		//Test the union of single sets
		netTest.unionIt(sets[1],NetCon::ACTIVATION,*s[4],-1);
		TS_ASSERT(netTest.containsAnyEdges(*s[4]));
		netTest.unionIt(sets[2],NetCon::ACTIVATION,*s[4],-1);
		netTest.unionIt(sets[3],NetCon::ACTIVATION,*s[4],-1);
		TS_ASSERT(netTest.containsAnyEdges(*s[4]));
		TS_ASSERT_EQUALS(netTest.totalParents(*s[4]),3);
		Set single = netTest.getSingleParentsFor(*s[4]);
		TS_ASSERT_EQUALS(single.size(),3);
		TS_ASSERT_EQUALS(single.get(0),s[1]);
		TS_ASSERT_EQUALS(single.get(1),s[2]);
		TS_ASSERT_EQUALS(single.get(2),s[3]);
		TS_ASSERT_EQUALS(netTest.totalParents(*s[4]),3);
		
		//Test the union with same things, but different influences
		//TODO
		
		//Test a larger thing;
		Set lowerOdd;
		lowerOdd.insert(s[1],0.001);
		lowerOdd.insert(s[3],0.001);
		lowerOdd.insert(s[5],0.001);
		netTest.unionIt(lowerOdd,NetCon::REPRESSION,*s[4],0);		
		TS_ASSERT_EQUALS(netTest.totalParents(*s[4]),4);
		cout << netTest << "\n";
		netTest.removeSubsets(*s[4]);
		cout << netTest << "\n";
		TS_ASSERT_EQUALS(netTest.totalParents(*s[4]),2);
		
		//test the remove losers function
		netTest.unionIt(sets[7],NetCon::ACTIVATION,*s[4],0);
		DoubleSet d;
		d.unionIt(lowerOdd);
		d.unionIt(sets[2]);
		double f[2];
		f[0] = 0.2;
		f[1] = 0.1;
		netTest.removeLosers(*s[4],d,f);
		single = netTest.getSingleParentsFor(*s[4]);
		TS_ASSERT_EQUALS(single.size(),2);
		TS_ASSERT_EQUALS(single.get(0),s[2]);
		TS_ASSERT_EQUALS(single.get(1),s[7]);
		
		//TODO, test the following function somehow
		//friend DoubleSet getContenders(const Specie& s, const Species& S, const Experiments& E,const NetCon& C, const Thresholds& T, const Encodings& L);

		//Test some assignments
		NetCon tmp(netTest);
		netTest = tmp;
		tmp = tmp;
    }
    void testLevelAssignments(void){
		Thresholds T(1.15,0.75,0.00,1,1,-1,0.01,0.025,2,false);
    	Specie * s[10];
    	Set sets[10];
    	for (int i = 0; i < 10; i++){
    		s[i] = Specie::getInstance("time",i);
    		sets[i].insert(s[i],0.001);
    	}
		
		Set two;
		two.insert(s[1],0.001);
		two.insert(s[2],0.001);
		
		//Test the empty cases
		LevelAssignments l;
		TS_ASSERT_EQUALS(l.size(),0);
		TS_ASSERT(l.get(0) == NULL);
		TS_ASSERT(l.get(-1) == NULL);
		TS_ASSERT(l.get(10) == NULL);

		//Create an experiment to initialize
		Species sp;
		sp.addSpecie(s[0]);
		sp.addSpecie(s[1]);
		sp.addSpecie(s[2]);
		sp.removeTimeSpecie();
   		Experiments exp1;
   		TS_ASSERT(exp1.addTimePoint(0,0,0,1));
   		TS_ASSERT(exp1.addTimePoint(0,0,1,1));
   		TS_ASSERT(exp1.addTimePoint(0,0,2,10)); //a different species at column 1
   		TS_ASSERT(exp1.addTimePoint(0,1,0,2));
   		TS_ASSERT(exp1.addTimePoint(0,1,1,5));
   		TS_ASSERT(exp1.addTimePoint(0,1,2,15));
   		TS_ASSERT(exp1.addTimePoint(0,2,0,3));
   		TS_ASSERT(exp1.addTimePoint(0,2,1,10));
   		TS_ASSERT(exp1.addTimePoint(0,2,2,20));
   		Encodings e;
   		e.initialize(&sp,&exp1,&T);
   		TS_ASSERT_EQUALS(e.totalSpecies(), 2);
   		TS_ASSERT(e.useNumbers(2));

		//initialize the experiment
		LevelAssignments k(two,e);
		//Check some gets, and the size
		TS_ASSERT_EQUALS(k.size(),4);
		TS_ASSERT(k.get(-1) == NULL);
		TS_ASSERT(k.get(10) == NULL);
		//check each assignment for correctness
		std::vector<int> * p = k.get(0);
		TS_ASSERT_EQUALS((int)p->size(),3);
		TS_ASSERT_EQUALS(p->at(0),-1);
		TS_ASSERT_EQUALS(p->at(1),0);
		TS_ASSERT_EQUALS(p->at(2),0);
		p = k.get(1);
		TS_ASSERT_EQUALS((int)p->size(),3);		
		TS_ASSERT_EQUALS(p->at(0),-1);
		TS_ASSERT_EQUALS(p->at(1),0);
		TS_ASSERT_EQUALS(p->at(2),1);
		p = k.get(2);
		TS_ASSERT_EQUALS((int)p->size(),3);		
		TS_ASSERT_EQUALS(p->at(0),-1);
		TS_ASSERT_EQUALS(p->at(1),1);
		TS_ASSERT_EQUALS(p->at(2),0);
		p = k.get(3);
		TS_ASSERT_EQUALS((int)p->size(),3);		
		TS_ASSERT_EQUALS(p->at(0),-1);
		TS_ASSERT_EQUALS(p->at(1),1);
		TS_ASSERT_EQUALS(p->at(2),1);

		//check some other initializations of single sets
		LevelAssignments k2(sets[1],e);
		k = k2;
		TS_ASSERT_EQUALS(k.size(),2);
		p = k.get(0);
		TS_ASSERT_EQUALS((int)p->size(),3);		
		TS_ASSERT_EQUALS(p->at(0),-1);
		TS_ASSERT_EQUALS(p->at(1),0);
		TS_ASSERT_EQUALS(p->at(2),-1);
		p = k.get(1);
		TS_ASSERT_EQUALS((int)p->size(),3);		
		TS_ASSERT_EQUALS(p->at(0),-1);
		TS_ASSERT_EQUALS(p->at(1),1);
		TS_ASSERT_EQUALS(p->at(2),-1);
		//the other way
		LevelAssignments k3(sets[2],e);
		k = k3;
		TS_ASSERT_EQUALS(k.size(),2);
		p = k.get(0);
		TS_ASSERT_EQUALS((int)p->size(),3);		
		TS_ASSERT_EQUALS(p->at(0),-1);
		TS_ASSERT_EQUALS(p->at(1),-1);
		TS_ASSERT_EQUALS(p->at(2),0);
		p = k.get(1);
		TS_ASSERT_EQUALS((int)p->size(),3);		
		TS_ASSERT_EQUALS(p->at(0),-1);
		TS_ASSERT_EQUALS(p->at(1),-1);
		TS_ASSERT_EQUALS(p->at(2),1);

		//Check the double set assignments
		DoubleSet d;
		d.unionIt(sets[1]);
		d.unionIt(sets[2]);
		LevelAssignments k4(d,e);
		k = k4;
		TS_ASSERT_EQUALS(k.size(),4);
		TS_ASSERT(k.get(-1) == NULL);
		TS_ASSERT(k.get(10) == NULL);
		p = k.get(0);
		TS_ASSERT_EQUALS((int)p->size(),3);		
		TS_ASSERT_EQUALS(p->at(0),-1);		
		TS_ASSERT_EQUALS(p->at(1),0);		
		TS_ASSERT_EQUALS(p->at(2),0);		
		p = k.get(1);
		TS_ASSERT_EQUALS((int)p->size(),3);		
		TS_ASSERT_EQUALS(p->at(0),-1);		
		TS_ASSERT_EQUALS(p->at(1),0);
		TS_ASSERT_EQUALS(p->at(2),1);
		p = k.get(2);
		TS_ASSERT_EQUALS((int)p->size(),3);		
		TS_ASSERT_EQUALS(p->at(0),-1);		
		TS_ASSERT_EQUALS(p->at(1),1);
		TS_ASSERT_EQUALS(p->at(2),0);
		p = k.get(3);
		TS_ASSERT_EQUALS((int)p->size(),3);		
		TS_ASSERT_EQUALS(p->at(0),-1);		
		TS_ASSERT_EQUALS(p->at(1),1);
		TS_ASSERT_EQUALS(p->at(2),1);

		DoubleSet d2;
		d2.unionIt(two);
		d2.unionIt(sets[1]);
		LevelAssignments k5(d2,e);
		k = k5;
		TS_ASSERT_EQUALS(k.size(),4);
		TS_ASSERT(k.get(-1) == NULL);
		TS_ASSERT(k.get(10) == NULL);
		p = k.get(0);
		TS_ASSERT_EQUALS((int)p->size(),3);		
		TS_ASSERT_EQUALS(p->at(0),-1);		
		TS_ASSERT_EQUALS(p->at(1),0);		
		TS_ASSERT_EQUALS(p->at(2),0);		
		p = k.get(1);
		TS_ASSERT_EQUALS((int)p->size(),3);		
		TS_ASSERT_EQUALS(p->at(0),-1);		
		TS_ASSERT_EQUALS(p->at(1),0);
		TS_ASSERT_EQUALS(p->at(2),1);
		p = k.get(2);
		TS_ASSERT_EQUALS((int)p->size(),3);		
		TS_ASSERT_EQUALS(p->at(0),-1);		
		TS_ASSERT_EQUALS(p->at(1),1);
		TS_ASSERT_EQUALS(p->at(2),0);
		p = k.get(3);
		TS_ASSERT_EQUALS((int)p->size(),3);		
		TS_ASSERT_EQUALS(p->at(0),-1);		
		TS_ASSERT_EQUALS(p->at(1),1);
		TS_ASSERT_EQUALS(p->at(2),1);

    }
    void testincrementBaseSet(void){
  		Set baseSet;
		for (int i = 0; i < 4; i++){
			baseSet.insert(Specie::getInstance("tmp",i),0.001);
		}
		int answers[100];
		answers[ 0] = 0;
		answers[ 1] = 1;
		answers[ 2] = 2;
		answers[ 3] = 3;
		answers[ 4] =  1;
		answers[ 5] =  2;
		answers[ 6] =  3;
		answers[ 7] = 12;
		answers[ 8] = 13;
		answers[ 9] = 23;
		answers[10] =  12;
		answers[11] =  13;
		answers[12] =  23;
		answers[13] = 123;
		answers[14] =  123;

		int answerIndex = 0;

		for (int i = 1; i <= 4; i++){
			cout << "Using " << i << " bases\n";
			int currentNumOfBasesUsed = i;
			int * currentBases = new int[currentNumOfBasesUsed];
			//fill the current with the first few
			for (int i = 0; i < currentNumOfBasesUsed-1; i++){
				currentBases[i] = i;
			}
			//set the last one to one lower, so that increment works the first time
			currentBases[currentNumOfBasesUsed-1] = currentNumOfBasesUsed-2;
			while(incrementBaseSet(currentBases,currentNumOfBasesUsed,baseSet.size())){
				int guess = 0;
				for (int i = 0; i < currentNumOfBasesUsed; i++){
					guess = guess*10 + currentBases[i];
				}
				cout << "\tcurrent guess " << guess << " and answer " << answers[answerIndex] << "\n";
				TS_ASSERT_EQUALS(guess,answers[answerIndex]);
				TS_ASSERT(answerIndex <= 14);
				answerIndex++;
			}
			delete [] currentBases;
		}
    }
    void testGeneNet(void){
		Thresholds T(1.15,0.75,0.5,1,1,3, 0.01, 0.025,9,false);
    	char * c = "../examples/IEA06/Ext_Guet/work/ext_guet_1_4_20_100_20000";
	    callGeneNet(c, T);
    }
    
};

