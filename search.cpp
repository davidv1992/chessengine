#include <vector>
#include <algorithm>

#include "board.h"
#include "search.h"
#include "history.h"
#include "table.h"

using namespace std;

#define MATESCORE 30000
#define DRAWSCORE 0

int minimax(int depth, board b, int alpha, int beta);

int quiesence(board b, int alpha, int beta, bool hasPass)
{
	if (b.inCheck().first || b.inCheck().second)
		return minimax(1, b, alpha, beta);
	
	int bestSoFar;
	if (hasPass)
		bestSoFar = b.eval();
	else
	{
		b.flipToMove();
		bestSoFar = quiesence(b, -beta, -alpha, true);
		b.flipToMove();
	}
	
	vector<move> moves = b.genMoves();
	moveOrderer order(b);
	sort(moves.begin(), moves.end(), order);
	for (int i=0; i<moves.size(); i++)
	{
		if (b.moveGains(moves[i]) < 300)
			break;
		board temp = b;
		temp.executeMove(moves[i]);
		int curScore = quiesence(temp, -beta, -max(alpha, bestSoFar), false);
		if (curScore > bestSoFar)
			bestSoFar = curScore;
		if (bestSoFar >= beta)
			break;
	}
	
	return -bestSoFar;
}

int minimax(int depth, board b, int alpha, int beta)
{
	lookupResult res;
	bool haveHit = queryTable(b, res);

	if (haveHit && depth <= res.depth)
	{
		if (res.type == SCORE_LOWERBOUND)
		{
			if (res.evaluation > beta)
				return -res.evaluation;
			else
				alpha = max(alpha, res.evaluation);
		}
		if (res.type == SCORE_UPPERBOUND)
		{
			if (res.evaluation < alpha)
				return -res.evaluation;
			else
				beta = min(beta, res.evaluation);
		}
		if (res.type == SCORE_EXACT)
			return -res.evaluation;
	}

	//if (depth == 0)
	//	return quiesence(b, alpha, beta, false);
	if (depth == 0)
		return -b.eval();
	
	if (inHistory(b))
		return 0;
	
	moveOrderer order(b);
	vector<move> moves = b.genMoves();
	sort(moves.begin(), moves.end(), order);
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
	if (haveHit)
	{
		board temp = b;
		temp.executeMove(res.bestMove);
		pushHistory(temp);
		bestSoFar = minimax(depth-1, temp, -beta, -alpha);
		popHistory();
	}
	int besti = -1;
	for (int i=0; i<moves.size() && bestSoFar < beta; i++)
	{
		if (haveHit && moves[i] == res.bestMove)
			continue;
		board temp = b;
		temp.executeMove(moves[i]);
		pushHistory(temp);
		int curScore = minimax(depth-1, temp, -beta, -max(alpha, bestSoFar));
		popHistory();
		if (curScore > bestSoFar)
		{
			bestSoFar = curScore;
			besti = i;
		}
	}
	
	if (besti == -1 && haveHit)
		putTable(b, depth, bestSoFar, res.bestMove, alpha, beta);
	else
		putTable(b, depth, bestSoFar, moves[besti], alpha, beta);
	
	return -bestSoFar;
}
