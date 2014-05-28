#include <iostream>
#include <fstream>

#include "../board.h"

using namespace std;

int main()
{
	int tc = 1;
	int fails = 0;
	ifstream FENfile("test/board_test.fen");
	if (!FENfile.is_open())
	{
		cerr << "Could not open test information." << endl;
		return 77;
	}
	
	string FEN;
	while (getline(FENfile, FEN))
	{
		board b(FEN);
		if (FEN != b.dumpFEN())
		{
			cerr << FEN << " " << b.dumpFEN() << endl;
			cerr << "Failure in testcase " << tc << "." << endl;
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
