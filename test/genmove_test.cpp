#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "../board.h"

using namespace std;

int main()
{
	int tc = 1;
	int fails = 0;
	ifstream FENfile("test/genmove_test.fen");
	if (!FENfile.is_open())
	{
		cerr << "Could not open test information." << endl;
		return 77;
	}
	
	string FEN;
	while (getline(FENfile, FEN))
	{
		board b(FEN);
		string res;
		getline(FENfile, res);
		
		
		istringstream cp(res);
		string smove;
		vector<move> refMove;
		do
		{
			cp >> smove;
			if (!cp)
				break;
			
			refMove.push_back(b.parseMove(smove));
		}
		while (cp);
		
		vector<move> result = b.genMoves();
		
		sort(refMove.begin(), refMove.end());
		sort(result.begin(), result.end());
		
		if (result.size() == refMove.size())
		{
			bool correct = true;
			for (int i=0; i<result.size(); i++)
			{
				if (result[i] != refMove[i])
				{
					cerr << refMove[i].dump() << " " << result[i].dump() << endl;
					correct = false;
				}
			}
			
			if (!correct)
			{
				cerr << "Wrong move in testcase " << tc << endl;
				fails++;
			}
		}
		else
		{
			for (int i=0; i<result.size(); i++)
			{
				cerr << result[i].dump() << endl;
			}
			cerr << "Wrong number of moves in testcase " << tc << endl;
			fails++;
		}
		tc++;
	}
	
	cerr << "Finished genmove tests." << endl;
	cerr << "Done:   " << tc-1 << endl;
	cerr << "Errors: " << fails << endl;
	
	if (fails)
		return 1;
	
	return 0;
}
