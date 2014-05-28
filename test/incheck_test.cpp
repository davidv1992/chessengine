#include <iostream>
#include <fstream>

#include "../board.h"

using namespace std;

int main()
{
	int tc = 1;
	int fails = 0;
	ifstream FENfile("test/incheck_test.fen");
	if (!FENfile.is_open())
	{
		cerr << "Could not open test information." << endl;
		return 77;
	}
	
	string FEN;
	while (getline(FENfile, FEN))
	{
		string res;
		getline(FENfile, res);
		pair<bool,bool> expected;
		expected.first = (res[0] == '1');
		expected.second = (res[1] == '1');
		
		board b(FEN);
		pair<bool, bool> inc = b.inCheck();
		
		if (inc.first != expected.first || inc.second != expected.second)
		{
			cerr << "(" << (expected.first?"true":"false") << "," << (expected.second?"true":"false") << ") (";
			cerr << (inc.first?"true":"false") << "," << (inc.second?"true":"false") << ")" << endl;
			cerr << "Wrong answer in test case " << tc << endl;
			fails++;
		}
		tc++;
	}
	
	cerr << "Finished incheck tests." << endl;
	cerr << "Done:   " << tc-1 << endl;
	cerr << "Errors: " << fails << endl;
	
	if (fails)
		return 1;
	
	return 0;
}
