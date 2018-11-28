#include "Player.h"
#include <iostream>
Player::Player (std::string const & name, size_t maxTiles)
{
	playerName = name;
	maximumTiles = maxTiles;
	score = 0;
}

Player::~Player()
{
	for(std::vector<Tile*>::iterator tileIter = playerTiles.begin(); tileIter != playerTiles.end(); ++tileIter)
	{
		delete *tileIter;
	}
}

std::set<Tile*> Player::getHandTiles() const
{
	//this function does not return the tiles in the same order they are stored
	//sets insert in sorted order
	std::set<Tile*> hand;

	for(size_t dap=0; dap<playerTiles.size(); dap++)
	{
		hand.insert(playerTiles[dap]);
	}

	return hand;
}

bool Player::hasTiles(std::string const & move, bool resolveBlanks) const
{
	//assuming that it is already confirmed that it is a exchange or place move
	std::vector<Tile*> hand;

	for(size_t plz=0; plz<playerTiles.size(); plz++)
	{
		hand.push_back(playerTiles[plz]);
	}

	if(!resolveBlanks)
	{
		for(size_t i=0; i<move.size(); i++)
		{
			size_t handSize = hand.size();
			size_t iterator = 0;
			while(iterator<handSize)
			{
				if(move[i]==hand[iterator]->getLetter())
				{
					hand.erase(hand.begin() + iterator);
					break;
				}
				iterator++;
			}
		}

		if(hand.size() == playerTiles.size()-move.size())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		std::string moveCopy=move;
		std::vector<char> blankVals;
		//finding all the blanks and the vals
		for(size_t i=0; i<moveCopy.size(); i++)
		{
			if(moveCopy[i]=='?')
			{
				blankVals.push_back(moveCopy[i+1]);
			}
		}
		//removing the vals
		for(std::string::iterator moveIter = moveCopy.begin(); moveIter != moveCopy.end(); ++moveIter)
		{
			if(*moveIter=='?')
			{
				moveCopy.erase(moveIter+1);
			}
		}

		//comparing the move and the hand
		size_t diff = 0;
		for(size_t i=0; i<moveCopy.size(); i++)
		{
			size_t handSize = hand.size();
			size_t iterator = 0;
			while(iterator<handSize)
			{
				if(moveCopy[i]==hand[iterator]->getLetter())
				{
					if(moveCopy[i]=='?')//setting value for ? in the hand
					{
						playerTiles[iterator]->useAs(blankVals[0]);
						blankVals.erase(blankVals.begin());
					}
					//hand.erase(hand.begin() + iterator);
					diff++;
					break;
				}
				iterator++;
			}
		}

		//each tile in the move has to have a corresponding tile in the had
		if(diff == moveCopy.size())
		{
			return true;
		}
		else
		{
			return false;
		}

	}

}

//can only be used if hasTiles returned true, and has to be used after the player's
//tile values have been set correctly with a prior call to hasTiles
std::vector<Tile*> Player::takeTiles (std::string const & move, bool resolveBlanks)
{
	std::vector<Tile*> ret;

	std::string moveCopy=move;
	std::vector<char> blankVals;
	//finding all the blanks and the vals
	for(size_t i=0; i<moveCopy.size(); i++)
	{
		if(moveCopy[i]=='?')
		{
			blankVals.push_back(moveCopy[i+1]);
		}
	}
	//removing the vals
	for(std::string::iterator moveIter = moveCopy.begin(); moveIter != moveCopy.end(); ++moveIter)
	{
		if(*moveIter=='?')
		{
			moveCopy.erase(moveIter+1);
		}
	}

	if(resolveBlanks==true)//place move
	{
		for(size_t i=0; i<moveCopy.size(); i++)
		{
			size_t handSize = playerTiles.size();
			while(handSize>0)
			{
				if(moveCopy[i] == playerTiles[handSize-1]->getLetter())
				{
					if(moveCopy[i]=='?' && blankVals[0]==playerTiles[handSize-1]->getUse())
					{
						ret.push_back(playerTiles[handSize-1]);
						playerTiles.erase(playerTiles.begin() + handSize-1);
						blankVals.erase(blankVals.begin());
						break;
					}
					else if(moveCopy[i]=='?' && blankVals[0]!=playerTiles[handSize-1]->getUse())
					{
						handSize--;
						continue;
					}
					else
					{
						ret.push_back(playerTiles[handSize-1]);
						playerTiles.erase(playerTiles.begin() + handSize-1);
						break;
					}
				}
				handSize--;
			}
		}
		//ret should have the tiles to be used after the for loop
		return ret;
	}
	else
	{
		for(size_t i=0; i<move.size(); i++)
		{
			size_t handSize = playerTiles.size();
			while(handSize>0)
			{
				if(move[i] == playerTiles[handSize-1]->getLetter())
				{
					ret.push_back(playerTiles[handSize-1]);
					playerTiles.erase(playerTiles.begin() + handSize-1);
					break;
				}
				handSize--;
			}
		}
		return ret;
	}
}

void Player::addTiles (std::vector<Tile*> const & tilesToAdd)
{
	for(size_t dap=0; dap<tilesToAdd.size(); dap++)
	{
		playerTiles.push_back(tilesToAdd[dap]);
	}
}