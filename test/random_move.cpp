#include "../board.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>

using namespace std;

int draws = 0, wwins = 0, bwins = 0;
int nmoves = 0;

void playGame()
{
	board b;
	
	while (true)
	{
		vector<move> moves = b.genMoves();
		nmoves++;
		if (moves.size() == 0)
		{
			if (b.getToMove() && b.inCheck().second)
			{
				//cout << "Win white.\n";
				wwins++;
				return;
			}
			else if (!b.getToMove() && b.inCheck().first)
			{
				//cout << "Win black.\n";
				bwins++;
				return;
			}
			else
			{
				//cout << "Draw.\n";
				draws++;
				return;
			}
		}
		if (b.getPlyClock() > 100)
		{
			//cout << "Draw.\n";
			draws++;
			return;
		}
		int mi = rand() % moves.size();
		b.executeMove(moves[mi]);
	}
}

int main()
{	
	srand(time(NULL));
	
	for (int i=0; i<10000; i++)
		playGame();
	
	cout << "Moves:      " << nmoves << endl;
	cout << "Draws:      " << draws << endl;
	cout << "White wins: " << wwins << endl;
	cout << "Black wins: " << bwins << endl;
	
	return 0;
}
