//
// Created by Linjian Li on 2018/11/30.
//

#ifndef BAYESIANNETWORK_SOMETEST_H
#define BAYESIANNETWORK_SOMETEST_H

#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

#include "Trainer.h"
#include "Network.h"

class SomeTest {
public:
	void test1() {
		std::cout << "doing some tests..." << std::endl;

		// do some test

		std::cout << "test finished" << std::endl;
	}

	void test2(Network* network) {
		// There exists a path:		0(root) -> 75 -> 74(leaf)

		int Z[2] = {0,75};
		int nz = 2;
		Node* Y = network->givenIndexToFindNodePointer(74);
		Combination E;
		Factor F;

		int eIndex[2]={0,75} , eValue[2]={1,0} , eNum=2;
		E = network->constructEvidence(eIndex,eValue,eNum);
		F = network->variableEliminationInferenceReturningPossibilities(Z,nz,E,Y);
		cout << "=======================================================================" << '\n'
			 << "Given \"0\"=1, \"75\"=0, " << endl;
		for (auto comb : F.combList) {
			cout << "P(\"" << (*comb.begin()).first << "\'=" << (*comb.begin()).second << ")=" << F.potentialsList[comb] << endl;
		}

		int eIndex2[1]={0} , eValue2[1]={1} , eNum2=1;
		E = network->constructEvidence(eIndex2,eValue2,eNum2);
		F = network->variableEliminationInferenceReturningPossibilities(Z,nz,E,Y);
		cout << "=======================================================================" << '\n'
			 << "Given \"0\"=1 " << endl;
		for (auto comb : F.combList) {
			cout << "P(\"" << (*comb.begin()).first << "\'=" << (*comb.begin()).second << ")=" << F.potentialsList[comb] << endl;
		}
	}
};


#endif //BAYESIANNETWORK_SOMETEST_H
