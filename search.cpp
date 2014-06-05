#include <vector>
#include <algorithm>

#include "board.h"
#include "search.h"
#include "history.h"
#include "table.h"

using namespace std;

#define MATESCORE(d) (60000-((d)>30?30000:1000*(d)))
#define MATESCORE_MAX 60000
#define DRAWSCORE 0

class moveOrderer
{
private:
	board base;
public:
	moveOrderer(board b)
	{
		base = b;
	}
	bool operator()(move a, move b)
	{
		if (base.moveGains(a) > base.moveGains(b))
			return true;
		if (base.moveGains(a) < base.moveGains(b))
			return false;
		return b < a;
	}
};

class movePairOrderer
{
private:
	board base;
public:
	movePairOrderer(board b)
	{
		base = b;
	}
	bool operator()(pair<int, move> a, pair<int, move> b)
	{
		if (a.first > b.first)
			return true;
		if (base.moveGains(a.second) > base.moveGains(b.second))
			return true;
		if (base.moveGains(a.second) < base.moveGains(b.second))
			return false;
		return b.second < a.second;
	}
};

int minimax(int depth, board b, int alpha, int beta, int movesDone);

int quiesence(board b, int alpha, int beta, bool hasPass)
{
	if (b.inCheck().first || b.inCheck().second)
		return minimax(1, b, alpha, beta, 0);
	
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

int minimax(int depth, board b, int alpha, int beta, int movesDone)
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
			return MATESCORE(movesDone);
		if (b.inCheck().second && b.getToMove())
			return MATESCORE(movesDone);
		return 0;
	}
	if (b.getPlyClock() >= 100)
		return 0;
	
	int bestSoFar = -MATESCORE_MAX;
	if (haveHit)
	{
		board temp = b;
		temp.executeMove(res.bestMove);
		pushHistory(temp);
		bestSoFar = minimax(depth-1, temp, -beta, -alpha, movesDone+1);
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
		int curScore = minimax(depth-1, temp, -beta, -max(alpha, bestSoFar), movesDone+1);
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

bool calcMoveID(board b, move &bestMove)
{
	vector<move> moves = b.genMoves();
	vector<pair<int, move> > moveEvalPairs;
	movePairOrderer order(b);
	
	if (moves.size() == 0)
		return false;
	
	for (int i=0; i<moves.size(); i++)
		moveEvalPairs.push_back(make_pair(0, moves[i]));
	
	sort(moveEvalPairs.begin(), moveEvalPairs.end());
	
	for (int depth = 0; depth<=DEPTH; depth++)
	{	
		int besti = -1; 
		int bestScore = -MATESCORE_MAX;
		vector<pair<int, move> > newPairs;
		for (int i=0; i<moveEvalPairs.size(); i++)
		{
			board temp = b;
			temp.executeMove(moveEvalPairs[i].second);
			pushHistory(temp);
			int curScore = minimax(DEPTH, temp, -MATESCORE_MAX, -bestScore, 0);
			popHistory();
			newPairs.push_back(make_pair(curScore, moveEvalPairs[i].second));
			if (curScore > bestScore)
			{
				besti = i;
				bestScore = curScore;
			}
		}
		
		sort(moveEvalPairs.begin(), moveEvalPairs.end());
	}
	
	bestMove = moveEvalPairs[0].second;
	return true;
}

bool calcMove(board b, move &bestMove)
{
	vector<move> moves = b.genMoves();
	moveOrderer order(b);
	sort(moves.begin(), moves.end(), order);
	
	if (moves.size() == 0)
		return false;
	
	int besti = -1;
	int bestScore = -MATESCORE_MAX;
	for (int i=0; i<moves.size(); i++)
	{
		board temp = b;
		temp.executeMove(moves[i]);
		pushHistory(temp);
		int curScore = minimax(DEPTH, temp, -MATESCORE_MAX, -bestScore+50, 0)+rand()%50;
		popHistory();
		if (curScore > bestScore)
		{
			besti = i;
			bestScore = curScore;
		}
	}
	
	bestMove = moves[besti];
	return true;
}

bool findMove(board b, move &bestMove)
{
	if (queryBook(b, bestMove))
	{
		clock_t start = clock();
		while (clock() < start + CLOCKS_PER_SEC/10);
		return true;
	}
	
	return calcMoveID(b, bestMove);
}
