#include <iostream>
#include <fstream>
#include <ctime>
#include "SomeTest.h"
#include "Trainer.h"
#include "Network.h"

int main() {

	SomeTest st;

	st.test1();

	// Redirect std::cout.
	string log_file = "log_bayesian_network_program.txt";
	std::cout << "The std::cout will be redirect to " << log_file << endl;
	std::ofstream out(log_file);
	std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
	std::cout.rdbuf(out.rdbuf()); //redirect std::cout to log_bayesian_network.txt!

	time_t now = time(0);
	char* dt = ctime(&now);
	cout << "Log file last modified at " << dt << endl;

	Trainer* trainer = new Trainer();
	trainer->loadConfig("../a1a.config.txt");	// Clion looks for file from Debug folder.
	trainer->loadLibSVMData("../a1a.txt");

	Trainer* tester = new Trainer();
	tester->loadConfig("../a1a.config.test.txt");	// Clion looks for file from Debug folder.
	tester->loadLibSVMData("../a1a.test.txt");

	Network* network = new Network();
	network->structLearn_ChowLiu_CompData(trainer);
	network->trainNetwork_KnowStruct_CompData(trainer);
	network->testingNetworkReturnAccuracy(tester);

	st.test2(network);

	cout << "Log file last modified at " << dt << endl;

	std::cout.rdbuf(coutbuf); //reset to standard output again
	std::cout << "The std::cout has been reset to standard output again." << endl;

	std::cout << "Hello, World!" << std::endl;

	return 0;


}