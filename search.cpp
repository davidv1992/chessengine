#include <vector>

#include "board.h"

using namespace std;

#define MATESCORE 30000
#define DRAWSCORE 0

int minimax(int depth, board b)
{
	if (depth == 0)
		return -b.eval();
	
	vector<move> moves = b.genMoves();
	if (moves.size() == 0)
	{
		if (b.inCheck().first && !b.getToMove())
			return MATESCORE;
		if (b.inCheck().second && b.getToMove())
			return MATESCORE;
		return 0;
	}
	if (b.getPlyClock() >= 100)
		return 0;
	
	int bestSoFar = -MATESCORE;
	for (int i=0; i<moves.size(); i++)
	{
		board temp = b;
		temp.executeMove(moves[i]);
		int curScore = minimax(depth-1, temp);
		if (curScore > bestSoFar)
			bestSoFar = curScore;
	}
	
	return -bestSoFar;
}
