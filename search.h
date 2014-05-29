#ifndef SEARCH_H
#define SEARCH_H

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

int minimax(int depth, board b, int alpha, int beta);

#endif //SEARCH_H
