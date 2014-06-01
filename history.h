#ifndef HISTORY_H
#define HISTORY_H

#include "board.h"

void clearHistory();
void pushHistory(board b);
void popHistory();

bool inHistory(board b);

#endif
