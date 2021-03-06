#ifndef BOARD_H
#define BOARD_H

#include <cstdio>
#include <vector>
#include <string>

#include "boardConst.h"

class move;

class board
{
friend bool queryBook(board b, move &m);
private:
	unsigned char squares[BOARD_SQRS];
	unsigned char castleRights;
	unsigned char plyClock;
	unsigned char toMove;
	unsigned char enPassantColumn;
	void genSwipeMoves(std::vector<move> &result, int startpos, int dx, int dy) const;
	void init(std::string);
	bool checkFromDiag(int, int, int, unsigned char) const;
public:
	bool operator==(const board b) const;
	int eval() const;
	board();
	board(std::string);
	unsigned long long hash() const;
	bool semiEqual(const board b) const;
	void dump(FILE *) const;
	std::string dumpFEN() const;
	std::vector<move> genMoves() const;
	void executeMove(move m);
	void flipToMove();
	std::pair<bool,bool> inCheck() const;
	move parseMove(std::string) const;
	int moveGains(move m) const;
	unsigned char getPlyClock() const
	{
		return plyClock;
	}
	unsigned char getToMove() const
	{
		return toMove;
	}
};

class move
{
friend class board;
private:
	unsigned char from, to;
	unsigned char promotePiece;
public:
	move(unsigned char from, unsigned char to)
	{
		this->from = from;
		this->to = to;
		this->promotePiece = 0;
	}
	move(unsigned char from, unsigned char to, unsigned char promotePiece)
	{
		this->from = from;
		this->to = to;
		this->promotePiece = promotePiece;
	}
	move()
	{
		from = to = promotePiece = 0;
	}
	std::string dump() const;
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
