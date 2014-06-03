#ifndef BOOK_H
#define BOOK_H

#define OPENINGBOOK "books/performance.bin"
#define CACHE_DEPTH 4

#include "board.h"

void initBook();
bool queryBook(board b, move &m);

#endif
