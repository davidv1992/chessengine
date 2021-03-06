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
#include "table.h"

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
			int wtime = 0;
			int btime = 0;
			int movestogo = 0;
			
			resetTableHits();
		
			// Parse rest of input
			while (input)
			{
				string subcommand;
				input >> subcommand;
				if (subcommand == "wtime")
					input >> wtime;
				else if (subcommand == "btime")
					input >> btime;
				else if (subcommand == "movestogo")
					input >> movestogo;
			}
		
			// Find move
			move bestMove;
			if (findMove(b, bestMove, wtime, btime, movestogo))
			{
				cout << "bestmove " << bestMove.dump() << endl;
			}
			else
			{
				cout << "bestmove 0000" << endl;
			}
			cout << "info string tablehits: " << queryTableHits() << "/" << queryTableTotal() << endl;
			cout << "info string table frac: " << (double(queryTableHits())/double(queryTableTotal())) << endl;
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
