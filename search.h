#ifndef SEARCH_H
#define SEARCH_H

#define DEPTH 7
#define MOVE_LEAWAY 2
#define OVERHEAD 20
#define DONE_FACTOR 2
#define TOTAL_FACTOR 1

bool findMove(board b, move &bestMove, int wtime, int btime, int movestogo);

#endif //SEARCH_H
