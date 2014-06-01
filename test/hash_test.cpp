#include <iostream>
#include <fstream>
#include <sstream>

#include "../board.h"

using namespace std;

int main()
{
	int tc = 1;
	int fails = 0;
	ifstream FENfile("test/hash_test.fen");
	if (!FENfile.is_open())
	{
		cerr << "Could not open test information." << endl;
		return 77;
	}
	
	string FEN;
	while (getline(FENfile, FEN))
	{
		board b(FEN);
		string result;
		getline(FENfile, result);
		istringstream input(result);
		unsigned long long expectedHash;
		input >> hex >> expectedHash;
		
		if (b.hash() != expectedHash)
		{
			cerr << hex << expectedHash << " " << hex << b.hash() << " " << hex << (expectedHash ^ b.hash()) << endl;
			cerr << "Mismatch in testcase " << tc << endl;
			fails++;
		}
		tc++;
	}
	
	cerr << "Finished board tests." << endl;
	cerr << "Done:   " << tc-1 << endl;
	cerr << "Errors: " << fails << endl;
	
	if (fails)
		return 1;
	
	return 0;
}
