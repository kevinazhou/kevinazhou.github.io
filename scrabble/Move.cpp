#include "Move.h"

#include <string>
#include <sstream>
#include <iostream>

Move * Move::parseMove(std::string moveString, Player &p)
{
	std::string moveType;
	std::istringstream toParse(moveString);

	toParse >> moveType;
	if(moveType=="pass")
	{
		PassMove* pass = new PassMove(&p);
		return pass;
	}
	else if(moveType=="exchange")
	{
		std::string tiles;
		toParse >> tiles;
		ExchangeMove* exchange = new ExchangeMove(tiles, &p);
		return exchange;
	}
	else if(moveType=="place")
	{
		std::string orientation;
		size_t row;
		size_t column;
		std::string word;

		toParse >> orientation >> row >> column >> word;


		if(orientation=="-")
		{
			PlaceMove* place = new PlaceMove(column, row, true, word, &p);
			return place;
		}
		else
		{
			PlaceMove* place = new PlaceMove(column, row, false, word, &p);
			return place;
		}
	}

	else
	{
		return nullptr;
	}
}
