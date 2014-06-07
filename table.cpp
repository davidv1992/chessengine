#include <vector>

#include "table.h"

using namespace std;

vector<pair<board,lookupResult> > table(TABLE_SIZE);

void putTable(board b, int depth, int eval, move bestmove, int alpha, int beta)
{
	lookupResult res;
	res.evaluation = eval;
	res.depth = depth;
	if (res.evaluation >= beta)
		res.type = SCORE_LOWERBOUND;
	else if (res.evaluation <= alpha)
		res.type = SCORE_UPPERBOUND;
	else
		res.type = SCORE_EXACT;
	table[b.hash()%TABLE_SIZE] = make_pair(b, res);
}

bool queryTable(board b, lookupResult &res)
{
	int index = b.hash()%TABLE_SIZE;
	if (table[index].first == b)
	{
		res = table[index].second;
		return true;
	}
	return false;
}
