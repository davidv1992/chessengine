#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <endian.h>

#include "book.h"

struct polygonEntry 
{
	uint64_t key;
	uint16_t move;
	uint16_t weight;
	uint32_t learn;
} __attribute__((packed));

void endianConvertEntry(polygonEntry &ent)
{
	ent.key = be64toh(ent.key);
	ent.move = be16toh(ent.move);
	ent.weight = be16toh(ent.weight);
	ent.learn = be16toh(ent.learn);
}

using namespace std;

vector<pair<uint64_t, int> > index;

FILE *bookfile = NULL;
int bookfile_entries = 0;

void initBook()
{
	if (bookfile != NULL)
		fclose(bookfile);
	bookfile = fopen(OPENINGBOOK, "rb");
	if (bookfile == NULL)
		exit(1);
	
	// generate index
	fseek(bookfile, 0, SEEK_END);
	
	int bookfile_len = ftell(bookfile);
	bookfile_entries = bookfile_len/16;
	
	index.clear();
	index.resize((1 << CACHE_DEPTH)-1);
	
	int blockSize = bookfile_entries >> CACHE_DEPTH;
	for (int i=1; i<(1<<CACHE_DEPTH); i++)
	{
		fseek(bookfile, i*blockSize*sizeof(polygonEntry), SEEK_SET);
		polygonEntry ent;
		if (fread(&ent, sizeof(polygonEntry), 1, bookfile) != 1)
			exit(1);
		endianConvertEntry(ent);
		uint64_t ckey = ent.key;
		index[i-1] = make_pair(ckey, i*blockSize);
	}
}

bool queryBook(board b, move &m)
{
	if (bookfile == NULL)
		initBook();
	
	uint64_t ourKey = b.hash();
	
	// Search index
	int low = 0, high = (1 << CACHE_DEPTH);
	while (high-low > 1)
	{
		int mid = (high+low)/2;
		if (ourKey < index[mid-1].first)
			high = mid;
		else
			low = mid;
	}
	
	if (low != 0)
		low = index[low-1].second;
	if (high == (1 << CACHE_DEPTH))
		high = bookfile_entries;
	else
		high = index[high-1].second;
	
	// Search book itself
	while (high - low > 1)
	{
		int mid = (high + low)/2;
		polygonEntry ent;
		fseek(bookfile, mid*sizeof(polygonEntry), SEEK_SET);
		if (fread(&ent, sizeof(polygonEntry), 1, bookfile) != 1)
			return false;
		endianConvertEntry(ent);
		
		if (ourKey < ent.key)
			high = mid;
		else
			low = mid;
	}
	
	// go to low end of range for this key
	while (low != 0)
	{
		polygonEntry ent;
		fseek(bookfile, (low-1)*sizeof(polygonEntry), SEEK_SET);
		if (fread(&ent, sizeof(polygonEntry), 1, bookfile) != 1)
			return false;
		
		endianConvertEntry(ent);
		if (ent.key == ourKey)
			low--;
		else
			break;
	}
	
	// read all entries for this key
	vector<polygonEntry> ents;
	uint32_t weight_sum = 0;
	while (low != bookfile_entries)
	{
		polygonEntry ent;
		fseek(bookfile, low*sizeof(polygonEntry), SEEK_SET);
		if (fread(&ent, sizeof(polygonEntry), 1, bookfile)!= 1)
			return false;
		endianConvertEntry(ent);
		if (ent.key == ourKey)
		{
			weight_sum += ent.weight;
			ents.push_back(ent);
		}
		else
		{
			break;
		}
		low++;
	}
	
	if (ents.size() == 0 || weight_sum == 0)
		return false;
	
	uint32_t choice = rand() % weight_sum;
	uint32_t partWS = 0;
	for (int i=0; i<ents.size(); i++)
	{
		partWS += ents[i].weight;
		if (partWS > choice)
		{
			int from = 0, to = 0, piece = 0;
			to = ents[i].move & 0x003F;
			from = (ents[i].move & 0x0FC0) >> 6;
			piece = ents[i].move & 0xF000;
			switch(piece)
			{
			case 0x1000:
				piece = PIECE_KNIGHT;
				break;
			case 0x2000:
				piece = PIECE_BISHOP;
				break;
			case 0x3000:
				piece = PIECE_ROOK;
				break;
			case 0x4000:
				piece = PIECE_QUEEN;
				break;
			}
			
			if (from == 4 && to == 0 && b.squares[4] == (PIECE_KING | PIECE_WHITE))
				to = 2;
			if (from == 4 && to == 7 && b.squares[4] == (PIECE_KING | PIECE_WHITE))
				to = 6;
			if (from == 60 && to == 56 && b.squares[60] == (PIECE_KING | PIECE_BLACK))
				to = 58;
			if (from == 60 && to == 63 && b.squares[60] == (PIECE_KING | PIECE_BLACK))
				to = 62;
			
			m = move(from, to, piece);
			return true;
		}
	}
	
	return false;
}
