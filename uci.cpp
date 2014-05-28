#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>

#include "version.h"
#include "board.h"
#include "search.h"

using namespace std;

// UCI command interface

int main()
{
	string inputline;
	board b;

	while(getline(cin, inputline))
	{
		istringstream input(inputline);
		
		string command;
		input >> command;
	
		if (command == "uci")
		{
			srand(time(NULL));
			cout << "id name " << ENGINE_NAME << endl;
			cout << "id version " << ENGINE_VERSION << endl;
			cout << "uciok" << endl;
		}
		else if (command == "isready")
		{
			cout << "readyok" << endl;
		}
		else if (command == "position")
		{
			string FEN_A, FEN_B, FEN_C, FEN_D, FEN_E, FEN_F;
			string move;
			input >> FEN_A;
			if (FEN_A == "startpos")
			{
				b = board();
			}
			else
			{
				input >> FEN_B >> FEN_C >> FEN_D >> FEN_E >> FEN_F;
				b = board(FEN_A + " " + FEN_B + " " + FEN_C + " " + FEN_D + " " + FEN_E + " " + FEN_F);
			}
			
			input >> FEN_A;
			
			while (input)
			{
				input >> move;
				if (!input)
					break;
				b.executeMove(b.parseMove(move));
			}
		}
		else if (command == "go")
		{
			// TODO: implement full go
			vector<move> moves = b.genMoves();
			if (moves.size() != 0)
			{
				int besti = -1;
				int bestScore = -60000;
				for (int i=0; i<moves.size(); i++)
				{
					board temp = b;
					temp.executeMove(moves[i]);
					int curScore = minimax(DEPTH, temp, -60000, -bestScore+50)+rand()%50;
					if (curScore > bestScore)
					{
						besti = i;
						bestScore = curScore;
					}
				}
				cout << "bestmove " << moves[besti].dump() << endl;
			}
			else
			{
				cout << "bestmove 0000" << endl;
			}
		}
		else if (command == "quit")
		{
			return 0;
		}
		else
		{
			cout << "info string unrecognized command " << command << endl;
		}
		
		cout.flush();
	}
}
