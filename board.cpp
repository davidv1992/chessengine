#include <cctype>

#include "board.h"

using namespace std;

static inline int opp(const int a)
{
	return (~a & PIECE_COLORMASK);
}

static inline int CBI(const int A, const int B)
{
	return A*BOARD_SIDE+B;
}

static inline int abs(const int a)
{
	if (a < 0)
		return -a;
	return a;
}

board::board()
{
	init("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

board::board(string FEN)
{
	init(FEN);
}

bool board::semiEqual(const board b) const
{
	if (toMove != b.toMove)
		return false;
	
	for (int i=0; i<BOARD_SQRS; i++)
		if (squares[i] != b.squares[i])
			return false;
	
	return true;
}

void board::init(string FEN)
{
	// empty init everything
	for (int i=0; i<BOARD_SQRS; i++)
		squares[i] = 0;
	
	toMove = 0;
	castleRights = 0;
	plyClock = 0;
	enPassantColumn = 0;
	
	// Parse FEN board position
	int pos = 0;
	for (int i=0; i<BOARD_SIDE; i++)
	{
		for (int j=0; j<BOARD_SIDE; j++)
		{
			if (islower(FEN[pos]))
			{
				squares[CBI(BOARD_SIDE-1-i,j)] |= PIECE_BLACK;
			}
			
			switch(FEN[pos])
			{
			case 'r':
			case 'R':
				squares[CBI(BOARD_SIDE-1-i,j)] |= PIECE_ROOK;
				break;
			case 'b':
			case 'B':
				squares[CBI(BOARD_SIDE-1-i,j)] |= PIECE_BISHOP;
				break;
			case 'q':
			case 'Q':
				squares[CBI(BOARD_SIDE-1-i,j)] |= PIECE_QUEEN;
				break;
			case 'k':
			case 'K':
				squares[CBI(BOARD_SIDE-1-i,j)] |= PIECE_KING;
				break;
			case 'n':
			case 'N':
				squares[CBI(BOARD_SIDE-1-i,j)] |= PIECE_KNIGHT;
				break;
			case 'p':
			case 'P':
				squares[CBI(BOARD_SIDE-1-i,j)] |= PIECE_PAWN;
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				j += FEN[pos]-'1';
				break;
			}
			
			pos++;
		}
		while (FEN[pos] != ' ' && FEN[pos] != '/') pos++;
		pos++;
	}
	
	// Parse FEN auxilary information
	while (FEN[pos] == ' ')	pos++;
	
	// side to move
	if (FEN[pos] == 'w')
		toMove = PIECE_WHITE;
	else
		toMove = PIECE_BLACK;
	pos++;
	
	while (FEN[pos] == ' ') pos++;
	
	// castling rights
	while (isalpha(FEN[pos]))
	{
		switch(FEN[pos])
		{
		case 'K':
			castleRights |= CASTLE_S << CASTLE_WHITE_SHIFT;
			break;
		case 'Q':
			castleRights |= CASTLE_L << CASTLE_WHITE_SHIFT;
			break;
		case 'k':
			castleRights |= CASTLE_S << CASTLE_BLACK_SHIFT;
			break;
		case 'q':
			castleRights |= CASTLE_L << CASTLE_BLACK_SHIFT;
			break;
		}
		pos++;
	}
	
	if (FEN[pos] == '-') pos++;
	
	while (FEN[pos] == ' ') pos++;
	
	// en-passant rights
	if (FEN[pos] >= 'a' && FEN[pos] <= 'h')
		enPassantColumn = FEN[pos] - 'a'+1;
		
	while (FEN[pos] != ' ') pos++;
	while (FEN[pos] == ' ') pos++;
	
	// ply clock
	while (isdigit(FEN[pos]))
	{
		plyClock *= 10;
		plyClock += FEN[pos] -'0';
		pos++;
	}
}

void board::flipToMove()
{
	toMove = ~toMove & PIECE_COLORMASK;
}

void board::dump(FILE *stream) const
{
	for (int i=0; i<8; i++)
	{
		for (int j=0; j<8; j++)
		{
			if (squares[CBI(BOARD_SIDE-1-i,j)] == 0)
				fprintf(stream, ".");
			else
			{
				char piecechar = ' ';
				char piecebits = squares[CBI(BOARD_SIDE-1-i,j)] & ~PIECE_COLORMASK;
				switch(piecebits)
				{
				case PIECE_PAWN:
					piecechar = 'p';
					break;
				case PIECE_KNIGHT:
					piecechar = 'n';
					break;
				case PIECE_BISHOP:
					piecechar = 'b';
					break;
				case PIECE_ROOK:
					piecechar = 'r';
					break;
				case PIECE_QUEEN:
					piecechar = 'q';
					break;
				case PIECE_KING:
					piecechar = 'k';
					break;
				}
				if ((squares[CBI(BOARD_SIDE-1-i,j)] & PIECE_COLORMASK) == PIECE_WHITE)
				{
					piecechar = toupper(piecechar);
				}
				fprintf(stream, "%c", piecechar);
			}
		}
		fprintf(stream, "\n");
	}
	
	fprintf(stream, "\n");
	
	//extra info
	if (castleRights & (CASTLE_S << CASTLE_WHITE_SHIFT))
		fprintf(stream, "K");
	if (castleRights & (CASTLE_L << CASTLE_WHITE_SHIFT))
		fprintf(stream, "Q");
	if (castleRights & (CASTLE_S << CASTLE_BLACK_SHIFT))
		fprintf(stream, "k");
	if (castleRights & (CASTLE_L << CASTLE_BLACK_SHIFT))
		fprintf(stream, "q");
	if (castleRights == 0)
		fprintf(stream, "-");
	fprintf(stream, " %c %c %d\n", (toMove == PIECE_WHITE?'w':'b'), (enPassantColumn == 0)?'-':(enPassantColumn-1+'a'), plyClock);
}

string board::dumpFEN() const
{
	string result = "";
	
	for (int i=0; i<BOARD_SIDE; i++)
	{
		int emptyCount = 0;
		for (int j=0; j<BOARD_SIDE; j++)
		{
			if (squares[CBI(BOARD_SIDE-1-i,j)] == 0)
			{
				emptyCount++;
				continue;
			}
			
			if (emptyCount != 0)
			{
				result += '0'+emptyCount;
				emptyCount = 0;
			}
			
			char piecechar = ' ';
			char piecebits = squares[CBI(BOARD_SIDE-1-i,j)] & ~PIECE_COLORMASK;
			switch(piecebits)
			{
			case PIECE_PAWN:
				piecechar = 'p';
				break;
			case PIECE_KNIGHT:
				piecechar = 'n';
				break;
			case PIECE_BISHOP:
				piecechar = 'b';
				break;
			case PIECE_ROOK:
				piecechar = 'r';
				break;
			case PIECE_QUEEN:
				piecechar = 'q';
				break;
			case PIECE_KING:
				piecechar = 'k';
				break;
			}
			if ((squares[CBI(BOARD_SIDE-1-i,j)] & PIECE_COLORMASK) == PIECE_WHITE)
			{
				piecechar = toupper(piecechar);
			}
			
			result += piecechar;
		}
		if (emptyCount != 0)
			result += '0'+emptyCount;
		if (i != BOARD_SIDE - 1)
			result += '/';
	}
	
	result += ' ';
	
	if (toMove == PIECE_WHITE)
		result += 'w';
	else
		result += 'b';
	
	result += ' ';
	
	if (castleRights & (CASTLE_S << CASTLE_WHITE_SHIFT))
		result += "K";
	if (castleRights & (CASTLE_L << CASTLE_WHITE_SHIFT))
		result += "Q";
	if (castleRights & (CASTLE_S << CASTLE_BLACK_SHIFT))
		result += "k";
	if (castleRights & (CASTLE_L << CASTLE_BLACK_SHIFT))
		result += "q";
	if (castleRights == 0)
		result += "-";
	result += " ";
	
	if (enPassantColumn == 0)
	{
		result += '-';
	}
	else
	{
		result += 'a'-1+enPassantColumn;
		result += (toMove == PIECE_WHITE)?'6':'3';
	}
	result += ' ';
	
	if (plyClock < 10)
	{
		result += '0' + plyClock;
	}
	else if (plyClock < 100)
	{
		result += '0' + (plyClock / 10);
		result += '0' + (plyClock % 10);
	}
	else
	{
		result += '0' + (plyClock / 100);
		result += '0' + ((plyClock / 10) % 10);
		result += '0' + (plyClock % 10);
	}
	
	result += ' ';
	result += '1';
	
	return result;
}

void board::genSwipeMoves(vector<move> &result, int startpos, int dx, int dy) const
{
	int i = startpos / BOARD_SIDE;
	int j = startpos % BOARD_SIDE;
	
	i += dx;
	j += dy;
	while (i >= 0 && i < BOARD_SIDE && j >= 0 && j < BOARD_SIDE)
	{
		if (squares[CBI(i,j)] != 0)
		{
			if (opp(squares[CBI(i,j)]) == toMove)
			{
				result.push_back(move(startpos, CBI(i,j)));
			}
			return;
		}
		result.push_back(move(startpos,CBI(i,j)));
		i += dx;
		j += dy;
	}
}

vector<move> board::genMoves() const
{
	vector<move> result;
	
	// generate special moves
	// TODO: check against castling through/out check
	if (toMove)
	{
		if (enPassantColumn != 0)
		{
			int j = enPassantColumn -1;
			int targetPlace = 3*BOARD_SIDE + j;
			
			if (j > 0 && squares[targetPlace-1] == (PIECE_PAWN | toMove))
			{
				result.push_back(move(targetPlace - 1, targetPlace - BOARD_SIDE));
			}
			if (j < BOARD_SIDE-1 && squares[targetPlace+1] == (PIECE_PAWN | toMove))
			{
				result.push_back(move(targetPlace + 1, targetPlace - BOARD_SIDE));
			}
		}
		if (!inCheck().second && castleRights & (CASTLE_L << CASTLE_BLACK_SHIFT))
		{
			if (squares[57] == 0 && squares[58] == 0 && squares[59] == 0)
			{
				board temp = *this;
				temp.executeMove(move(60,59));
				if (!temp.inCheck().second)
					result.push_back(move(60,58));
			}
		}
		if (!inCheck().second && castleRights & (CASTLE_S << CASTLE_BLACK_SHIFT))
		{
			if (squares[61] == 0 && squares[62] == 0)
			{
				board temp = *this;
				temp.executeMove(move(60,61));
				if (!temp.inCheck().second)
					result.push_back(move(60,62));
			}
		}
	}
	else
	{
		if (enPassantColumn != 0)
		{
			int j = enPassantColumn -1;
			int targetPlace = 4*BOARD_SIDE + j;
			
			if (j > 0 && squares[targetPlace-1] == (PIECE_PAWN | toMove))
			{
				result.push_back(move(targetPlace - 1, targetPlace + BOARD_SIDE));
			}
			if (j < BOARD_SIDE-1 && squares[targetPlace+1] == (PIECE_PAWN | toMove))
			{
				result.push_back(move(targetPlace + 1, targetPlace + BOARD_SIDE));
			}
		}
		if (!inCheck().first && castleRights & (CASTLE_L << CASTLE_WHITE_SHIFT))
		{
			if (squares[1] == 0 && squares[2] == 0 && squares[3] == 0)
			{
				board temp = *this;
				temp.executeMove(move(4,3));
				if (!temp.inCheck().first)
					result.push_back(move(4,2));
			}
		}
		if (!inCheck().first && castleRights & (CASTLE_S << CASTLE_WHITE_SHIFT))
		{
			if (squares[5] == 0 && squares[6] == 0)
			{
				board temp = *this;
				temp.executeMove(move(4,5));
				if (!temp.inCheck().first)
					result.push_back(move(4,6));
			}
		}
	}
	
	for (int i=0; i<8; i++)
	{
		for (int j=0; j<8; j++)
		{
			if (squares[CBI(i,j)] == 0)
				continue;
			if ((squares[CBI(i,j)] & PIECE_COLORMASK) != toMove)
				continue;
			
			int piecebits = squares[CBI(i,j)] & ~PIECE_COLORMASK;
			
			if (piecebits == PIECE_PAWN)
			{
				int di = toMove?-1:1;
				int startPos = toMove?6:1;
				
				if (squares[CBI(i+di,j)] == 0)
				{
					if (i+di == 0 || i + di == BOARD_SIDE-1)
					{
						result.push_back(move(CBI(i,j),CBI(i+di,j),PIECE_KNIGHT));
						result.push_back(move(CBI(i,j),CBI(i+di,j),PIECE_BISHOP));
						result.push_back(move(CBI(i,j),CBI(i+di,j),PIECE_ROOK));
						result.push_back(move(CBI(i,j),CBI(i+di,j),PIECE_QUEEN));
					}
					else
					{
						result.push_back(move(CBI(i,j),CBI(i+di,j)));
					}
					
					if (i == startPos)
					{
						if (squares[CBI(i+2*di,j)] == 0)
						{
							result.push_back(move(CBI(i,j),CBI(i+2*di,j)));
						}
					}
				}
				if (j > 0 && squares[CBI(i+di,j-1)] && opp(squares[CBI(i+di,j-1)]) == toMove)
				{
					if (i+di == 0 || i + di == BOARD_SIDE-1)
					{
						result.push_back(move(CBI(i,j),CBI(i+di,j-1),PIECE_KNIGHT));
						result.push_back(move(CBI(i,j),CBI(i+di,j-1),PIECE_BISHOP));
						result.push_back(move(CBI(i,j),CBI(i+di,j-1),PIECE_ROOK));
						result.push_back(move(CBI(i,j),CBI(i+di,j-1),PIECE_QUEEN));
					}
					else
					{
						result.push_back(move(CBI(i,j),CBI(i+di,j-1)));
					}
				}
				if (j < BOARD_SIDE-1 && squares[CBI(i+di,j+1)] && opp(squares[CBI(i+di,j+1)]) == toMove)
				{
					if (i+di == 0 || i + di == BOARD_SIDE-1)
					{
						result.push_back(move(CBI(i,j),CBI(i+di,j+1),PIECE_KNIGHT));
						result.push_back(move(CBI(i,j),CBI(i+di,j+1),PIECE_BISHOP));
						result.push_back(move(CBI(i,j),CBI(i+di,j+1),PIECE_ROOK));
						result.push_back(move(CBI(i,j),CBI(i+di,j+1),PIECE_QUEEN));
					}
					else
					{
						result.push_back(move(CBI(i,j),CBI(i+di,j+1)));
					}
				}
			}
			else if (piecebits == PIECE_KNIGHT)
			{
				if (i > 0 && j > 1 && (squares[CBI(i-1,j-2)] == 0 || opp(squares[CBI(i-1,j-2)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i-1,j-2)));
				if (i > 1 && j > 0 && (squares[CBI(i-2,j-1)] == 0 || opp(squares[CBI(i-2,j-1)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i-2,j-1)));
				if (i > 1 && j < BOARD_SIDE-1 && (squares[CBI(i-2,j+1)] == 0 || opp(squares[CBI(i-2,j+1)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i-2,j+1)));
				if (i > 0 && j < BOARD_SIDE-2 && (squares[CBI(i-1,j+2)] == 0 || opp(squares[CBI(i-1,j+2)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i-1,j+2)));
				if (i < BOARD_SIDE-1 && j < BOARD_SIDE-2 && (squares[CBI(i+1,j+2)] == 0 || opp(squares[CBI(i+1,j+2)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i+1,j+2)));
				if (i < BOARD_SIDE-2 && j < BOARD_SIDE-1 && (squares[CBI(i+2,j+1)] == 0 || opp(squares[CBI(i+2,j+1)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i+2,j+1)));
				if (i < BOARD_SIDE-2 && j > 0 && (squares[CBI(i+2,j-1)] == 0 || opp(squares[CBI(i+2,j-1)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i+2,j-1)));
				if (i < BOARD_SIDE-1 && j > 1 && (squares[CBI(i+1,j-2)] == 0 || opp(squares[CBI(i+1,j-2)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i+1,j-2)));
			}
			else if (piecebits == PIECE_KING)
			{
				if (i > 0 && (squares[CBI(i-1,j)] == 0 || opp(squares[CBI(i-1,j)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i-1,j)));
				if (i > 0 && j > 0 && (squares[CBI(i-1,j-1)] == 0 || opp(squares[CBI(i-1,j-1)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i-1,j-1)));
				if (j > 0 && (squares[CBI(i,j-1)] == 0 || opp(squares[CBI(i,j-1)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i,j-1)));
				if (i < BOARD_SIDE-1 && j > 0 && (squares[CBI(i+1,j-1)] == 0 || opp(squares[CBI(i+1,j-1)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i+1,j-1)));
				if (i < BOARD_SIDE-1 && (squares[CBI(i+1,j)] == 0 || opp(squares[CBI(i+1,j)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i+1,j)));
				if (i < BOARD_SIDE-1 && j < BOARD_SIDE-1 && (squares[CBI(i+1,j+1)] == 0 || opp(squares[CBI(i+1,j+1)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i+1,j+1)));
				if (j < BOARD_SIDE-1 && (squares[CBI(i,j+1)] == 0 || opp(squares[CBI(i,j+1)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i,j+1)));
				if (i > 0 && j < BOARD_SIDE-1 && (squares[CBI(i-1,j+1)] == 0 || opp(squares[CBI(i-1,j+1)]) == toMove))
					result.push_back(move(CBI(i,j),CBI(i-1,j+1)));
			}
			else
			{
				if (piecebits & PIECE_BISHOP)
				{
					genSwipeMoves(result, CBI(i,j),1,1);
					genSwipeMoves(result, CBI(i,j),1,-1);
					genSwipeMoves(result, CBI(i,j),-1,1);
					genSwipeMoves(result, CBI(i,j),-1,-1);
				}
				if (piecebits & PIECE_ROOK)
				{
					genSwipeMoves(result, CBI(i,j),1,0);
					genSwipeMoves(result, CBI(i,j),-1,0);
					genSwipeMoves(result, CBI(i,j),0,1);
					genSwipeMoves(result, CBI(i,j),0,-1);
				}
			}
		}
	}
	
	vector<move> fresult;
	for (int i=0; i<result.size(); i++)
	{
		board temp = *this;
		temp.executeMove(result[i]);
		if (toMove && !temp.inCheck().second)
			fresult.push_back(result[i]);
		if (!toMove && !temp.inCheck().first)
			fresult.push_back(result[i]);
	}
	
	return fresult;
}

void board::executeMove(move m)
{
	int fromPieceBits = squares[m.from] & ~PIECE_COLORMASK;
	
	// plyClock change
	if (fromPieceBits == PIECE_PAWN || squares[m.to] != 0)
		plyClock = 0;
	else
		plyClock++;
	
	// Castle right changes
	if (fromPieceBits == PIECE_KING)
	{
		if (toMove)
		{
			castleRights &= ~((CASTLE_S | CASTLE_L) << CASTLE_BLACK_SHIFT);
		}
		else
		{
			castleRights &= ~((CASTLE_S | CASTLE_L) << CASTLE_WHITE_SHIFT);
		}
	}
	if (fromPieceBits == PIECE_ROOK)
	{
		if (toMove)
		{
			if (m.from == 56)
			{
				castleRights &= ~(CASTLE_L << CASTLE_BLACK_SHIFT);
			}
			if (m.from == 63)
			{
				castleRights &= ~(CASTLE_S << CASTLE_BLACK_SHIFT);
			}
		}
		else
		{
			if (m.from == 0)
			{
				castleRights &= ~(CASTLE_L << CASTLE_WHITE_SHIFT);
			}
			if (m.from == 7)
			{
				castleRights &= ~(CASTLE_S << CASTLE_WHITE_SHIFT);
			}
		}
	}
	
	// En-passant rights
	if (fromPieceBits == PIECE_PAWN && abs(m.from - m.to) == 2*BOARD_SIDE)
	{
		enPassantColumn = (m.from % 8) + 1;
	}
	else
	{
		enPassantColumn = 0;
	}
	
	// toMove color change
	toMove ^= PIECE_COLORMASK;
	
	// Move execution
	if (fromPieceBits == PIECE_KING && abs(m.from - m.to) == 2)
	{
		// Castling move handling
		if (m.from > m.to)
		{
			// long castle
			squares[m.to] = squares[m.from];
			squares[m.to+1] = squares[m.from-4];
			squares[m.from] = 0;
			squares[m.from-4] = 0;
		}
		else
		{
			// short castle
			squares[m.to] = squares[m.from];
			squares[m.to-1] = squares[m.from+3];
			squares[m.from] = 0;
			squares[m.from+3] = 0;
		}
	}
	else if (fromPieceBits == PIECE_PAWN && abs(m.from-m.to) != BOARD_SIDE 
				&& abs(m.from-m.to) != 2*BOARD_SIDE && squares[m.to] == 0)
	{
		// en passant taking
		squares[m.to] = squares[m.from];
		squares[m.from] = 0;
		int diff = (m.to - m.from > 0)?(m.to-m.from-BOARD_SIDE):(m.to-m.from+BOARD_SIDE);
		squares[m.from+diff] = 0;
	}
	else
	{
		// Normal move handling
		squares[m.to] = squares[m.from];
		squares[m.from] = 0;
	}
	
	// promotion handling
	if (m.promotePiece != 0)
		squares[m.to] = (squares[m.to] & PIECE_COLORMASK) | m.promotePiece;
}

bool board::checkFromDiag(int kingPos, int di, int dj, unsigned char PIECE_MASK) const
{
	int i = kingPos/BOARD_SIDE;
	int j = kingPos%BOARD_SIDE;
	
	i += di;
	j += dj;
	
	while (i >= 0 && i < BOARD_SIDE && j >= 0 && j < BOARD_SIDE && squares[CBI(i,j)] == 0)
	{
		i += di;
		j += dj;
	}
	
	if (i >= 0 && i < BOARD_SIDE && j >= 0 && j < BOARD_SIDE && 
		(squares[CBI(i,j)] & PIECE_MASK) == PIECE_MASK &&
		(squares[CBI(i,j)] & PIECE_COLORMASK) == (PIECE_MASK & PIECE_COLORMASK))
		return true;
	return false;
}

int board::moveGains(move m) const
{
	int piecebits = squares[m.to] & ~PIECE_COLORMASK;
	
	if (piecebits == PIECE_QUEEN)
		return 900;
	if (piecebits == PIECE_ROOK)
		return 500;
	if (piecebits == PIECE_BISHOP || piecebits == PIECE_KNIGHT)
		return 300;
	if (piecebits == PIECE_PAWN)
		return 100;
	return 0;
}

move board::parseMove(string mov) const
{
	int from = 0, to = 0, prom = 0;
	if (mov[0] >= 'a' && mov[0] <= 'h' && mov[1] >= '1' && mov[1] <= '8')
	{
		from = (mov[1]-'1')*BOARD_SIDE + (mov[0]-'a');
	}
	
	if (mov[2] >= 'a' && mov[2] <= 'h' && mov[3] >= '1' && mov[3] <= '8')
	{
		to = (mov[3] - '1')*BOARD_SIDE + (mov[2] - 'a');
	}
	
	if (mov[4] == 'n')
		prom = PIECE_KNIGHT;
	if (mov[4] == 'b')
		prom = PIECE_BISHOP;
	if (mov[4] == 'r')
		prom = PIECE_ROOK;
	if (mov[4] == 'q')
		prom = PIECE_QUEEN;
		
	move m(from,to,prom);
	return m;
}

pair<bool,bool> board::inCheck() const
{
	pair<bool, bool> result(false,false);
	for (int i=0; i<8; i++)
	{
		for (int j=0; j<8; j++)
		{
			if ((squares[CBI(i,j)] & ~PIECE_COLORMASK) == PIECE_KING)
			{
				bool curRes = false;
				// any pawns attacking king?
				int di = (squares[CBI(i,j)] & PIECE_COLORMASK)?-1:1;
				if (i+di >= 0 && i+di < BOARD_SIDE && j > 0
					 && squares[CBI(i+di,j-1)] == (opp(squares[CBI(i,j)]) | PIECE_PAWN))
						curRes = true;
				if (i+di >= 0 && i+di < BOARD_SIDE && j < BOARD_SIDE-1
					 && squares[CBI(i+di,j+1)] == (opp(squares[CBI(i,j)]) | PIECE_PAWN))
					 	curRes = true;
				
				// any knights?
				if (i > 0 && j > 1 && squares[CBI(i-1,j-2)] == (opp(squares[CBI(i,j)]) | PIECE_KNIGHT))
					curRes = true;
				if (i > 1 && j > 0 && squares[CBI(i-2,j-1)] == (opp(squares[CBI(i,j)]) | PIECE_KNIGHT))
					curRes = true;
				if (i > 1 && j < BOARD_SIDE-1 && squares[CBI(i-2,j+1)] == (opp(squares[CBI(i,j)]) | PIECE_KNIGHT))
					curRes = true;
				if (i > 0 && j < BOARD_SIDE-2 && squares[CBI(i-1,j+2)] == (opp(squares[CBI(i,j)]) | PIECE_KNIGHT))
					curRes = true;
				if (i < BOARD_SIDE-1 && j < BOARD_SIDE-2 && squares[CBI(i+1,j+2)] == (opp(squares[CBI(i,j)]) | PIECE_KNIGHT))
					curRes = true;
				if (i < BOARD_SIDE-2 && j < BOARD_SIDE-1 && squares[CBI(i+2,j+1)] == (opp(squares[CBI(i,j)]) | PIECE_KNIGHT))
					curRes = true;
				if (i < BOARD_SIDE-2 && j > 0 && squares[CBI(i+2,j-1)] == (opp(squares[CBI(i,j)]) | PIECE_KNIGHT))
					curRes = true;
				if (i < BOARD_SIDE-1 && j > 1 && squares[CBI(i+1,j-2)] == (opp(squares[CBI(i,j)]) | PIECE_KNIGHT))
					curRes = true;
				
				// the other king?
				if (i > 0 && squares[CBI(i-1,j)] == (opp(squares[CBI(i,j)]) | PIECE_KING))
					curRes = true;
				if (i > 0 && j > 0 && squares[CBI(i-1,j-1)] == (opp(squares[CBI(i,j)]) | PIECE_KING))
					curRes = true;
				if (j > 0 && squares[CBI(i,j-1)] == (opp(squares[CBI(i,j)]) | PIECE_KING))
					curRes = true;
				if (i < BOARD_SIDE-1 && j > 0 && squares[CBI(i+1,j-1)] == (opp(squares[CBI(i,j)]) | PIECE_KING))
					curRes = true;
				if (i < BOARD_SIDE-1 && squares[CBI(i+1,j)] == (opp(squares[CBI(i,j)]) | PIECE_KING))
					curRes = true;
				if (i < BOARD_SIDE-1 && j < BOARD_SIDE-1 && squares[CBI(i+1,j+1)] == (opp(squares[CBI(i,j)]) | PIECE_KING))
					curRes = true;
				if (j < BOARD_SIDE-1 && squares[CBI(i,j+1)] == (opp(squares[CBI(i,j)]) | PIECE_KING))
					curRes = true;
				if (i > 0 && j < BOARD_SIDE-1 && squares[CBI(i-1,j+1)] == (opp(squares[CBI(i,j)]) | PIECE_KING))
					curRes = true;
				
				// Any sliding pieces?
				if (checkFromDiag(CBI(i,j),1,1,PIECE_BISHOP | opp(squares[CBI(i,j)])))
					curRes = true;
				if (checkFromDiag(CBI(i,j),1,-1,PIECE_BISHOP | opp(squares[CBI(i,j)])))
					curRes = true;
				if (checkFromDiag(CBI(i,j),-1,1,PIECE_BISHOP | opp(squares[CBI(i,j)])))
					curRes = true;
				if (checkFromDiag(CBI(i,j),-1,-1,PIECE_BISHOP | opp(squares[CBI(i,j)])))
					curRes = true;
				
				if (checkFromDiag(CBI(i,j),1,0,PIECE_ROOK | opp(squares[CBI(i,j)])))
					curRes = true;
				if (checkFromDiag(CBI(i,j),-1,0,PIECE_ROOK | opp(squares[CBI(i,j)])))
					curRes = true;
				if (checkFromDiag(CBI(i,j),0,1,PIECE_ROOK | opp(squares[CBI(i,j)])))
					curRes = true;
				if (checkFromDiag(CBI(i,j),0,-1,PIECE_ROOK | opp(squares[CBI(i,j)])))
					curRes = true;
				
				if (curRes)
				{
					if (squares[CBI(i,j)] & PIECE_COLORMASK)
					{
						result = make_pair(result.first, true);
					}
					else
					{
						result = make_pair(true,result.second);
					}
				}
			}
		}
	}
	
	return result;
}

int board::eval() const
{
	int score = 0;
	for (int i=0; i<BOARD_SIDE; i++)
	{
		for (int j=0; j<BOARD_SIDE; j++)
		{
			int pieceScore = 0;
			int piecebits = squares[CBI(i,j)] &~PIECE_COLORMASK;
			if (piecebits == PIECE_PAWN)
				pieceScore = 100;
			if (piecebits == PIECE_KNIGHT)
				pieceScore = 300;
			if (piecebits == PIECE_BISHOP)
				pieceScore = 300;
			if (piecebits == PIECE_ROOK)
				pieceScore = 500;
			if (piecebits == PIECE_QUEEN)
				pieceScore = 900;
			
			if ((squares[CBI(i,j)] & PIECE_COLORMASK) == toMove)
				score += pieceScore;
			else
				score -= pieceScore;
		}
	}
	
	return score;
}

string move::dump() const
{
	string result = "";
	result += 'a' + (from%BOARD_SIDE);
	result += '1' + (from/BOARD_SIDE);
	result += 'a' + (to%BOARD_SIDE);
	result += '1' + (to/BOARD_SIDE);
	if (promotePiece == PIECE_KNIGHT)
		result += 'n';
	if (promotePiece == PIECE_BISHOP)
		result += 'b';
	if (promotePiece == PIECE_ROOK)
		result += 'r';
	if (promotePiece == PIECE_QUEEN)
		result += 'q';
	return result;
}
