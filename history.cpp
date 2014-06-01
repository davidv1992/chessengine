#include <vector>

#include "history.h"

using namespace std;

vector<board> history;

void clearHistory()
{
	history.clear();
}

void pushHistory(board b)
{
	history.push_back(b);
}

void popHistory()
{
	history.pop_back();
}

bool inHistory(board b)
{
	for (int i=0; i<history.size()-1; i++)
	{
		if (b.semiEqual(history[i]))
		{
			return true;
		}
	}
	return false;
}
