//
// Created by Linjian Li on 2018/12/5.
//

#include "gadget.h"


/*
 * A function for generateParentsCombinations() of Node.
 *
 * Input several sets,
 * output all possible combinations of elements of each set,
 * by picking one element in each set.
 *
 * For example:
 * 		A={1,3,5} , B={a,b,c}
 * 		setOfSets={A,B}
 * 		result=thisfunction(setOfSets)
 * 		then, result={1a,1b,1c,3a,3b,3c,5a,5b,5c}
 *
 * Use recursive function.
 *
 * The input is the first argument.
 * */
set<Combination> generate_all_possible_combinations_from_several_sets(set<Combination>* setOfSets) {

	// Error Case
	if (setOfSets->size()<1) {
		cout << "The size of setOfSets is less than 1." << endl;
		exit(1);
	}


	set<Combination>::iterator its=setOfSets->begin();
	Combination toBeAdded;
	toBeAdded =  *its;
	set<Combination> result;

	// Base Case
	if (setOfSets->size()==1) {
		for (Combination::iterator itTBA=toBeAdded.begin(); itTBA!=toBeAdded.end(); itTBA++){
			pair<string, int> p = (*itTBA);
			Combination c;
			c.insert(p);
			result.insert(c);
		}
		return result;
	}

	// Recursive Case (the size of setOfSets is greater than 1)
	setOfSets->erase(its);
	result = generate_all_possible_combinations_from_several_sets(setOfSets);
	for (Combination::iterator itTBA=toBeAdded.begin(); itTBA!=toBeAdded.end(); itTBA++){
		for (set<Combination>::iterator itResult=result.begin(); itResult!=result.end(); itResult++) {
			Combination c = (*itResult);
			pair<string, int> p = *itTBA;
			c.insert(p);
			result.insert(c);
		}
	}
	return result;
}


/*
 * full_first_is_compatible_with_partial_second(Combination* first, Combination* second)
 *
 * Case1: first={ <"a",1> , <"b",2> }, second={ <"a",1> , <"b",2> , <"c",3> , whatever more... }
 * return true.
 *
 * Case2: first={ <"a",0> , <"b",2> }, second={ <"a",1> , <"b",2> , <"c",3> , whatever more... }
 * return false.
 *
 * Case3: first={ <"a",1> , <"b",2> , <"c",3> }, second={ <"a",1> , <"b",2> }
 * return false.
 *
 * */
bool full_first_is_compatible_with_partial_second(Combination* first, Combination* second) {
	for (Combination::iterator it1=first->begin(); it1!=first->end(); it1++) {
		if (second->find(*it1)==second->end()) return false;
	}
	return true;
}


/*
 * partial_first_is_compatible_with_partial_second_on_common_variable(Combination* first, Combination* second)
 *
 * Case1: first={ <"a",1> , <"b",2> }, second={ <"a",1> , <"b",2> , <"c",3> , whatever more... }
 * return true.
 *
 * Case2: first={ <"a",0> , <"b",2> }, second={ <"a",1> , <"b",2> , <"c",3> , whatever more... }
 * return false.
 *
 * Case3: first={ <"a",1> , <"b",2> , <"c",3> }, second={ <"a",1> , <"b",2> }
 * return true.
 *
 * Case4: first={ <"a",1> , <"b",2> , <"c",3> }, second={ <"a",1> , <"b",2> , <"d",4> }
 * return true.
 *
 * Case5: first={ <"a",1> , <"b",2> , <"c",3> }, second={ <"a",1> , <"b",2> , <"c",4> }
 * return false.
 *
 * Case5: first={ <"a",1> , <"b",2> }, second={ <"c",3> , <"d",4> }
 * return true.
 * */
bool partial_first_is_compatible_with_partial_second_on_common_variable(Combination* first, Combination* second) {
	for (Combination::iterator it1=first->begin(); it1!=first->end(); it1++) {
		for (Combination::iterator it2=second->begin(); it2!=second->end(); it2++) {
			if ((*it1).first==(*it2).first && (*it1).second!=(*it2).second) return false;
		}
	}
	return true;
}


int* widthFirstTraversalWithAdjacencyMatirx(int** graph, int numOfNodes, int start) {
	int* result = new int[numOfNodes];
	int itResult = 0;
	queue<int> que;
	set<int> markSet;
	que.push(start);
	while (!que.empty()) {
		int pos = que.front();
		result[itResult++] = pos;
		markSet.insert(pos);
		que.pop();
		for (int i=0; i<numOfNodes; i++) {
			if (graph[pos][i]!=0 && markSet.find(i)==markSet.end()) {
				que.push(i);
			}
		}
	}
	return result;
}