#include <iostream>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cstdio>

#include "version.h"
#include "board.h"
#include "search.h"
#include "history.h"
#include "book.h"

using namespace std;

// UCI command interface

extern vector<board> history;

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
			initBook();
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
			clearHistory();
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
			
			pushHistory(b);
			while (input)
			{
				input >> move;
				if (!input)
					break;
				b.executeMove(b.parseMove(move));
				pushHistory(b);
			}
		}
		else if (command == "go")
		{
			// query book
			move bookMove;
			if (queryBook(b, bookMove))
			{
				clock_t start = clock();
				while (clock() < start + CLOCKS_PER_SEC);
				cout << "info string found book move" << endl;
				cout << "bestmove " << bookMove.dump() << endl;
			}
			else
			{
				cout << "info string start calculating" << endl;
				// TODO: implement full go
				vector<move> moves = b.genMoves();
				moveOrderer order(b);
				sort(moves.begin(), moves.end(), order);
				if (moves.size() != 0)
				{
					int besti = -1;
					int bestScore = -60000;
					for (int i=0; i<moves.size(); i++)
					{
						board temp = b;
						temp.executeMove(moves[i]);
						pushHistory(temp);
						int curScore = minimax(DEPTH, temp, -60000, -bestScore+50)+rand()%50;
						popHistory();
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
