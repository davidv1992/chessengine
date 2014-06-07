#include <vector>
#include <algorithm>
#include <ctime>

#include "board.h"
#include "search.h"
#include "history.h"
#include "table.h"

#include <iostream>

using namespace std;

#define MATESCORE(d) (60000-((d)>30?30000:1000*(d)))
#define MATESCORE_MAX 60000
#define MATESCORE_MIN 30000
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
		if (a.first < b.first)
			return false;
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
	
	lookupResult res;
	bool haveHit = queryTable(b, res);
	
	int bestSoFar;
	if (hasPass)
	{
		alpha = max(b.eval(), alpha);
		if (alpha >= beta)
			return alpha;
		if (haveHit)
		{
			if (res.type == SCORE_LOWERBOUND)
			{
				if (res.evaluation >= beta)
					return -res.evaluation;
				else
					alpha = max(alpha, res.evaluation);
			}
			if (res.type == SCORE_UPPERBOUND)
			{
				if (res.evaluation <= alpha)
					return -alpha;
				else
					beta = min(beta, res.evaluation);
			}
			if (res.type == SCORE_EXACT)
				return -max(alpha, res.evaluation);
		}
	
		bestSoFar = alpha;
	}
	else
	{
		if (haveHit)
		{
			if (res.type == SCORE_LOWERBOUND)
			{
				if (res.evaluation >= beta)
					return -res.evaluation;
				else
					alpha = max(alpha, res.evaluation);
			}
			if (res.type == SCORE_UPPERBOUND)
			{
				if (res.evaluation <= alpha)
					return -res.evaluation;
				else
					beta = min(beta, res.evaluation);
			}
			if (res.type == SCORE_EXACT)
				return -res.evaluation;
		}
		b.flipToMove();
		bestSoFar = quiesence(b, -beta, -alpha, true);
		b.flipToMove();
	}
	
	vector<move> moves = b.genMoves();
	moveOrderer order(b);
	sort(moves.begin(), moves.end(), order);
	int besti = 0;
	for (int i=0; i<moves.size(); i++)
	{
		if (b.moveGains(moves[i]) < 300)
			break;
		board temp = b;
		temp.executeMove(moves[i]);
		int curScore = quiesence(temp, -beta, -max(alpha, bestSoFar), false);
		if (curScore > bestSoFar)
		{
			bestSoFar = curScore;
			besti = i;
		}
		if (bestSoFar >= beta)
			break;
	}
	
	putTable(b, 0, bestSoFar, moves[besti], alpha, beta);
	
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
			if (res.evaluation >= beta)
				return -res.evaluation;
			else
				alpha = max(alpha, res.evaluation);
		}
		if (res.type == SCORE_UPPERBOUND)
		{
			if (res.evaluation <= alpha)
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

bool calcMoveID(board b, move &bestMove, clock_t deadline, clock_t hardline)
{
	vector<move> moves = b.genMoves();
	vector<pair<int, move> > moveEvalPairs;
	movePairOrderer order(b);
	
	if (moves.size() == 0)
		return false;
	
	for (int i=0; i<moves.size(); i++)
		moveEvalPairs.push_back(make_pair(0, moves[i]));
	
	sort(moveEvalPairs.begin(), moveEvalPairs.end(), order);
	
	for (int depth = 0; deadline > clock() && moveEvalPairs.size() > 1; depth++)
	{
		cout << "info depth " << depth << endl;	
		int besti = -1; 
		int bestScore = -MATESCORE_MAX;
		vector<pair<int, move> > newPairs;
		int qref = 0;
		for (int i=0; i<moveEvalPairs.size(); i++)
		{
			if ((i)*DONE_FACTOR < moveEvalPairs.size()*TOTAL_FACTOR && deadline <= clock())
				goto retMove;
			if (hardline <= clock())
				goto retMove;
			board temp = b;
			temp.executeMove(moveEvalPairs[i].second);
			pushHistory(temp);
			int curScore = minimax(depth, temp, -bestScore, -bestScore+1, 0);
			if (curScore >= bestScore)
				curScore = minimax(depth, temp, -MATESCORE_MAX, -bestScore+1, 0);
			else
				qref++;
			popHistory();
			newPairs.push_back(make_pair(curScore, moveEvalPairs[i].second));
			if (curScore > bestScore)
			{
				besti = i;
				bestScore = curScore;
			}
		}
		
		cout << "info string quickrefute " << qref << "/" << moveEvalPairs.size() << endl;
		cout << "info string quickrefute " << (double(qref)/double(moveEvalPairs.size())) << endl;
		
		moveEvalPairs.swap(newPairs);
		
		sort(moveEvalPairs.begin(), moveEvalPairs.end(), order);
		if (moveEvalPairs[0].first >= MATESCORE_MIN || moveEvalPairs[0].first <= -MATESCORE_MIN)
			break;	// mate found, further calc not neccessary
		
		// eliminate known bad moves
		int nSize = 1;
		while (nSize < moveEvalPairs.size() && moveEvalPairs[nSize].first > -MATESCORE_MIN)
			nSize++;
		
		moveEvalPairs.resize(nSize);
	}

retMove:	
	int range = 1;
	while (range < moveEvalPairs.size() && moveEvalPairs[range].first == moveEvalPairs[0].first)
		range++;
	
	int index = rand()%range;
	
	bestMove = moveEvalPairs[index].second;
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

bool findMove(board b, move &bestMove, int wtime, int btime, int movestogo)
{
	if (queryBook(b, bestMove))
	{
		clock_t start = clock();
		while (clock() < start + CLOCKS_PER_SEC/10);
		return true;
	}
	
	clock_t deadline = 0;
	clock_t hardline = 0;
	if (b.getToMove())
	{
		if (btime != 0)
		{
			int target = btime/(movestogo+MOVE_LEAWAY) - OVERHEAD;
			deadline = (CLOCKS_PER_SEC/1000)*target + clock();
			hardline = (CLOCKS_PER_SEC/1000)*HARDLINE_FACTOR*target+clock();
		}
	}
	else
	{
		if (wtime != 0)
		{
			int target = wtime/(movestogo+MOVE_LEAWAY) - OVERHEAD;
			deadline = (CLOCKS_PER_SEC/1000)*target + clock();
			hardline = (CLOCKS_PER_SEC/1000)*HARDLINE_FACTOR*target+clock();
		}
	}
	
	if (deadline != 0)
		return calcMoveID(b, bestMove, deadline, hardline);
	else
		return calcMove(b, bestMove);
}
