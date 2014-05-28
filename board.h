#ifndef BOARD_H
#define BOARD_H

#include <cstdio>
#include <vector>
#include <string>

#include "boardConst.h"

class move;

class board
{
private:
	unsigned char squares[BOARD_SQRS];
	unsigned char castleRights;
	unsigned char plyClock;
	unsigned char toMove;
	unsigned char enPassantColumn;
	void genSwipeMoves(std::vector<move> &result, int startpos, int dx, int dy);
	void init(std::string);
	bool checkFromDiag(int, int, int, unsigned char);
public:
	board();
	board(std::string);
	void dump(FILE *);
	std::string dumpFEN();
	std::vector<move> genMoves();
	void executeMove(move m);
	std::pair<bool,bool> inCheck();
	move parseMove(std::string);
	unsigned char getPlyClock()
	{
		return plyClock;
	}
	unsigned char getToMove()
	{
		return toMove;
	}
};

class move
{
friend class board;
private:
	int from, to;
	int promotePiece;
	move(int from, int to)
	{
		this->from = from;
		this->to = to;
		this->promotePiece = 0;
	}
	move(int from, int to, int promotePiece)
	{
		this->from = from;
		this->to = to;
		this->promotePiece = promotePiece;
	}
public:
	std::string dump();
	bool operator<(const move &m) const
	{
		if (from < m.from)
			return true;
		if (from > m.from)
			return false;
		if (to < m.to)
			return true;
		if (to > m.to)
			return false;
		return promotePiece < m.promotePiece;
	}
	
	bool operator!=(const move &m) const
	{
		return from != m.from || to != m.to || promotePiece != m.promotePiece;
	}
	bool operator==(const move &m) const
	{
		return from == m.from && to == m.to && promotePiece == m.promotePiece;
	}
};

#endif //BOARD_H
