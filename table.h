#ifndef TABLE_H
#define TABLE_H

#include "board.h"

#define TABLE_SIZE (1024*1024*32)

enum scoreType {SCORE_EXACT, SCORE_LOWERBOUND, SCORE_UPPERBOUND};

struct lookupResult
{
	move bestMove;
	int evaluation;
	int depth;
	scoreType type;
};

void putTable(board b, int depth, int eval, move bestMove, int alpha, int beta);
bool queryTable(board b, lookupResult &res);

void resetTableHits();
unsigned long long queryTableHits();
unsigned long long queryTableTotal();

#endif
