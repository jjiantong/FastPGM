//
// Created by Linjian Li on 2018/11/29.
//

#include "Factor.h"

Factor::Factor() {}

void Factor::constructFactor(Node* node) {

	relatedVariables.insert(node->nodeName);

	set<pair<string,int>> tempPairSet;
	for (int i=0; i<node->numOfPotentialValues; i++) {
		Combination tempComb;
		pair<string, int> tempPair;
		tempPair.first = node->nodeName;
		tempPair.second = node->potentialValues[i];
		tempPairSet.insert(tempPair);
	}

	if (node->parentsPointers.empty()) {	// If this node has no parents.
		for (set<pair<string, int>>::iterator it=tempPairSet.begin(); it!=tempPairSet.end(); it++) {
			Combination c;
			c.insert(*it);
			combList.insert(c);
			potentialsList[c] = node->margProbTable[(*it).second];
		}
		return;
	}

	// If this node has parents
	for (set<pair<string, int>>::iterator it=tempPairSet.begin(); it!=tempPairSet.end(); it++) {
		for (auto p : node->parentsPointers) {
			relatedVariables.insert(p->nodeName);
		}
		for (set<pair<string, int>>::iterator it=tempPairSet.begin(); it!=tempPairSet.end(); it++) {
			for (set<Combination>::iterator itP=node->parentsCombinations.begin(); itP!=node->parentsCombinations.end(); itP++) {
				Combination cP = (*itP);
				pair<string, int> p = (*it);
				cP.insert(p);
				combList.insert(cP);
				potentialsList[cP] = node->condProbTable[p.second][*itP];
			}
		}
	}
	return;
}


Factor Factor::multiplyWithFactor(Factor secondFactor) {
	Factor newFactor;
	newFactor.relatedVariables.insert(this->relatedVariables.begin(),this->relatedVariables.end());
	newFactor.relatedVariables.insert(secondFactor.relatedVariables.begin(),secondFactor.relatedVariables.end());
	set<string> commonRelatedVariables;
	set_intersection(this->relatedVariables.begin(),this->relatedVariables.end(),
					  secondFactor.relatedVariables.begin(),secondFactor.relatedVariables.end(),
					  std::inserter(commonRelatedVariables,commonRelatedVariables.begin()));
	for (set<Combination>::iterator it=this->combList.begin(); it!=this->combList.end(); it++) {
		for (set<Combination>::iterator itSF=secondFactor.combList.begin(); itSF!=secondFactor.combList.end(); itSF++) {
			Combination first, second;
			first = *it;
			second = *itSF;
			if (!partial_first_is_compatible_with_partial_second_on_common_variable(&first,&second)) continue;	// solve the bug about common variables
			Combination nComb;
			nComb.insert((*it).begin(),(*it).end());
			nComb.insert((*itSF).begin(),(*itSF).end());
			newFactor.combList.insert(nComb);
			newFactor.potentialsList[nComb] = this->potentialsList[*it] * secondFactor.potentialsList[*itSF];
		}
	}
	return newFactor;
}

Factor Factor::sumProductOverVariable(Node* node) {
	Factor newFactor;
	this->relatedVariables.erase(node->nodeName);
	newFactor.relatedVariables = this->relatedVariables;
	for (set<Combination>::iterator it=this->combList.begin(); it!=this->combList.end(); it++) {
		pair<string, int> pairToBeErased;
		for (Combination::iterator itC=(*it).begin(); itC!=(*it).end(); itC++) {
			if ((*itC).first==node->nodeName) {
				pairToBeErased = (*itC);
				break;
			}
		}
		Combination comb = *it;
		double temp = this->potentialsList[comb];
		comb.erase(pairToBeErased);
		if (newFactor.combList.find(comb)!=newFactor.combList.end()) {
			newFactor.potentialsList[comb] += temp;
		} else {
			newFactor.combList.insert(comb);
			newFactor.potentialsList[comb] = temp;
		}
	}
	return newFactor;
}

void Factor::normalize() {
	double denominator = 0;
	for (auto comb : combList) {
		denominator += potentialsList[comb];
	}
	for (auto comb : combList) {
		potentialsList[comb] /= denominator;
	}
}