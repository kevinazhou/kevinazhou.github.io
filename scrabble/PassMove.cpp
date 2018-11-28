#include "Move.h"
#include "Board.h"

PassMove::PassMove(Player * player) : Move(player)
{
	_player = player;
}

void PassMove::execute(Board & board, Bag & bag, Dictionary & dictionary) {}