#include "Move.h"
#include "Exceptions.h"
#include "Board.h"

ExchangeMove::ExchangeMove(std::string tileString, Player * p) : Move(p)
{
	_player = p;
	tilesToExchange = tileString;
}

void ExchangeMove::execute(Board & board, Bag & bag, Dictionary & dictionary)
{
	if(_player->hasTiles(tilesToExchange, false))
	{
		std::vector<Tile*> newTiles;
		std::vector<Tile*> oldTiles;
		oldTiles = _player->takeTiles(tilesToExchange, false);
		newTiles = bag.drawTiles(tilesToExchange.size());

		bag.addTiles(oldTiles);
		if(!newTiles.empty())
		{
			_player->addTiles(newTiles);
		}

	}
	else
	{
		throw FileException("EXCHANGE");
	}


}