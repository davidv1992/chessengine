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

unsigned long long hits = 0;
unsigned long long total = 0;

bool queryTable(board b, lookupResult &res)
{
	int index = b.hash()%TABLE_SIZE;
	total++;
	if (table[index].first == b)
	{
		res = table[index].second;
		hits++;
		return true;
	}
	return false;
}

void resetTableHits()
{
	hits = 0;
	total = 0;
}

unsigned long long queryTableHits()
{
	return hits;
}

unsigned long long queryTableTotal()
{
	return total;
}
