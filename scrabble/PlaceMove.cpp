#include "Move.h"
#include "Exceptions.h"
#include "Board.h"

#include <iostream>

PlaceMove::PlaceMove(size_t x, size_t y, bool horizontal, std::string tileString, Player * p) : Move(p)
{
	xIndex = x;
	yIndex = y;
	isHorizontal = horizontal;
	rawWordData = tileString;
	tilesToPlace = tileString;
	appendDirection = 0;
	_player = p;
	totalPoints = 0;
}

void PlaceMove::execute(Board & board, Bag & bag, Dictionary & dictionary)
{
	if(board.getSquare(yIndex, xIndex)->isOccupied())
	{	
		throw FileException("SQUARE TAKEN");
	}
	if(!(_player->hasTiles(rawWordData, true)))
	{
		//if there are any ? in the player's hand, the values are set with this hasTiles call
		throw FileException("TILES NOT IN HAND");
	}

	tempTiles = _player->takeTiles(rawWordData, true);//to keep track of tile base values
	//needs to be used right after hasTiles since takeTiles will ignore the letter after ? if true is passed in
	//and the following for loop erases the letters after ? for tilesToPlace

	//erasing the values after the blanks and then filling appendstring with the true word that is being placed
	for(std::string::iterator tileIter = tilesToPlace.begin(); tileIter != tilesToPlace.end(); ++tileIter)
	{
		if(*tileIter=='?')
		{
			appendString += *(tileIter+1);
			tilesToPlace.erase(tileIter+1);
		}
		else
		{
			appendString += *tileIter;
		}
	}

	if(xIndex!=board.getXStart() || yIndex!=board.getYStart())
	{
		bool STARPLATINUM = false;
		for(size_t ZAWARUDO = 0; ZAWARUDO<=tilesToPlace.size(); ZAWARUDO++)
		{
			if(isHorizontal)
			{	
				Square* SQUIDWARD;
				SQUIDWARD = board.getSquare(yIndex, xIndex+ZAWARUDO);
				if(SQUIDWARD->isOccupied())
				{
					STARPLATINUM = true;
				}
			}
			else
			{
				Square* SQUIDWARD;
				SQUIDWARD = board.getSquare(yIndex+ZAWARUDO, xIndex);
				if(SQUIDWARD->isOccupied())
				{
					STARPLATINUM = true;
				}
			}
		}
		if(!STARPLATINUM)
		{
			throw FileException("WORD NOT ATTACHED");
		}
	}

	//figuring out multiplier values of the squares before placing tiles
	std::vector<unsigned int> tileValues;
	std::vector<unsigned int> lMultipliers;
	std::vector<unsigned int> wMultipliers;
	int isAppend = 0;//to tell if creating bigger word from words already on board
	//0 for none, 1 is horizontal append after, 2 is horiz append before, 3 is both horizontally
	//4 is vertical append after, 5 is vertical append before, 6 is both vertically
	size_t wordLength = tilesToPlace.size();
	size_t tileValTracker = 0;

	if(isHorizontal)
	{
		for(size_t i=0; i<wordLength; i++)
		{
			Square* sq;
			sq = board.getSquare(yIndex, xIndex+i);
			if(sq->isOccupied())
			{
				appendString.insert(appendString.begin()+i, sq->getLetter());
				wordLength++;
				tileValTracker++;
				tileValues.push_back(sq->getScore());
			}
			else
			{
				tileValues.push_back(tempTiles[i-tileValTracker]->getPoints());	
			}

			lMultipliers.push_back(sq->getLMult());
			wMultipliers.push_back(sq->getWMult());

			
			wordWithoutAppend += sq->getLetter();
			if(i==wordLength-1)//if there are letters after
			{
				if(xIndex+wordLength-1 > board.getColumns())//checking exceeding bounds
				{
					throw FileException("EXCEEDS BOUNDS");
				}
				sq = board.getSquare(yIndex, xIndex+i+1);
				if(sq->isOccupied())
				{
					if(appendDirection==2)//meaning there is already appending before
					{
						appendDirection=3;
						isAppend=3;
					}
					else
					{
						appendDirection=1;
						isAppend = 1;//1 indicates horizontal append after
					}
				}
			}
			if(i==0 && xIndex!=1)//if there are letters before
			{
				sq = board.getSquare(yIndex, xIndex-1);
				if(sq->isOccupied())
				{
					isAppend=2;//2 is horizontal append before
					appendDirection=2;
				}
			}
		}

	}

	else
	{
		for(size_t i=0; i<wordLength; i++)
		{
			Square* sq;
			sq = board.getSquare(yIndex+i, xIndex);
			if(sq->isOccupied())
			{
				appendString.insert(appendString.begin()+i, sq->getLetter());
				wordLength++;
				tileValTracker++;
				tileValues.push_back(sq->getScore());;
			}
			else
			{
				tileValues.push_back(tempTiles[i-tileValTracker]->getPoints());	
			}

			lMultipliers.push_back(sq->getLMult());
			wMultipliers.push_back(sq->getWMult());

			wordWithoutAppend += sq->getLetter();
			if(i==wordLength-1)
			{
				if(yIndex+wordLength-1 > board.getRows())//checking exceeding bounds
				{
					throw FileException("EXCEEDS BOUNDS");
				}
				sq = board.getSquare(yIndex+i+1, xIndex);
				if(sq->isOccupied())
				{
					if(appendDirection==2)//if already appending before
					{
						appendDirection=6;
						isAppend=3;
					}
					else
					{
						appendDirection=1;
						isAppend = 4;//3 is vertical append after
					}
				}
			}
			if(i==0 && yIndex!=1)//if there are letters before
			{
				sq = board.getSquare(yIndex-1, xIndex);
				if(sq->isOccupied())
				{
					isAppend=5;//5 is vertical append before
					appendDirection=2;
				}
			}
		}
	}

	_player->addTiles(tempTiles);


	//now the multipliers vectors have the lMult and wMult values
	//check now to make sure the word is viable
	//could do this in the main function but easier to account for appending words here
	size_t a = 0;

	if(isAppend==1)//adding to back horizontally
	{
		while(board.getSquare(yIndex, xIndex+wordLength+a)->isOccupied() && xIndex+wordLength+a < board.getColumns())
		{
			lMultipliers.push_back(board.getSquare(yIndex, xIndex+wordLength+a)->getLMult());
			wMultipliers.push_back(board.getSquare(yIndex, xIndex+wordLength+a)->getWMult());
			tileValues.push_back(board.getSquare(yIndex, xIndex+wordLength+a)->getScore());
			appendString += board.getSquare(yIndex, xIndex+wordLength+a)->getLetter();
			a++;
		}	
	}
	else if(isAppend==2)//adding to front horizontally
	{
		while(board.getSquare(yIndex, xIndex-1-a)->isOccupied() && xIndex-1-a > 0)
		{
			lMultipliers.insert(lMultipliers.begin(), board.getSquare(yIndex, xIndex-1-a)->getLMult());
			wMultipliers.insert(wMultipliers.begin(), board.getSquare(yIndex, xIndex-1-a)->getWMult());
			tileValues.insert(tileValues.begin(), board.getSquare(yIndex, xIndex-1-a)->getScore());
			appendString.insert(appendString.begin(), board.getSquare(yIndex, xIndex-1-a)->getLetter());
			a++;
		}
	}
	else if(isAppend==3)//horizontal append both ways
	{
		while(board.getSquare(yIndex, xIndex+wordLength+a)->isOccupied() && xIndex+wordLength+a < board.getColumns())
		{
			lMultipliers.push_back(board.getSquare(yIndex, xIndex+wordLength+a)->getLMult());
			wMultipliers.push_back(board.getSquare(yIndex, xIndex+wordLength+a)->getWMult());
			tileValues.push_back(board.getSquare(yIndex, xIndex+wordLength+a)->getScore());
			appendString += board.getSquare(yIndex, xIndex+wordLength+a)->getLetter();
			a++;
		}
		size_t b=0;
		while(board.getSquare(yIndex, xIndex-1-b)->isOccupied() && xIndex-1-b > 0)
		{
			lMultipliers.insert(lMultipliers.begin(), board.getSquare(yIndex, xIndex-1-a)->getLMult());
			wMultipliers.insert(wMultipliers.begin(), board.getSquare(yIndex, xIndex-1-a)->getWMult());
			tileValues.insert(tileValues.begin(), board.getSquare(yIndex, xIndex-1-a)->getScore());
			appendString.insert(appendString.begin(), board.getSquare(yIndex, xIndex-1-a)->getLetter());
			b++;
			a++;
		}
	}
	else if(isAppend==4)//adding to back vertically
	{
		while(board.getSquare(yIndex+wordLength+a, xIndex)->isOccupied() && yIndex+wordLength+a < board.getRows())
		{
			lMultipliers.push_back(board.getSquare(yIndex+wordLength+a, xIndex)->getLMult());
			wMultipliers.push_back(board.getSquare(yIndex+wordLength+a, xIndex)->getWMult());
			tileValues.push_back(board.getSquare(yIndex+wordLength+a, xIndex)->getScore());
			appendString += board.getSquare(yIndex+wordLength+a, xIndex)->getLetter();
			a++;
		}	
	}
	else if(isAppend==5)//adding to front vertically
	{
		while(board.getSquare(yIndex-1-a, xIndex)->isOccupied() && yIndex-1-a>0)
		{
			lMultipliers.insert(lMultipliers.begin(), board.getSquare(yIndex-1-a, xIndex)->getLMult());
			wMultipliers.insert(wMultipliers.begin(), board.getSquare(yIndex-1-a, xIndex)->getWMult());
			tileValues.insert(tileValues.begin(), board.getSquare(yIndex-1-a, xIndex)->getScore());
			appendString.insert(appendString.begin(), board.getSquare(yIndex-1-a, xIndex)->getLetter());
			a++;
		}
	}
	else if(isAppend==6)
	{
		while(board.getSquare(yIndex+wordLength+a, xIndex)->isOccupied() && yIndex+wordLength+a < board.getRows())
		{
			lMultipliers.push_back(board.getSquare(yIndex+wordLength+a, xIndex)->getLMult());
			wMultipliers.push_back(board.getSquare(yIndex+wordLength+a, xIndex)->getWMult());
			tileValues.push_back(board.getSquare(yIndex+wordLength+a, xIndex)->getScore());
			appendString += board.getSquare(yIndex+wordLength+a, xIndex)->getLetter();
			a++;
		}	
		size_t b=0;
		while(board.getSquare(yIndex-1-b, xIndex)->isOccupied() && yIndex-1-b>0)
		{
			lMultipliers.insert(lMultipliers.begin(), board.getSquare(yIndex-1-a, xIndex)->getLMult());
			wMultipliers.insert(wMultipliers.begin(), board.getSquare(yIndex-1-a, xIndex)->getWMult());
			tileValues.insert(tileValues.begin(), board.getSquare(yIndex-1-a, xIndex)->getScore());
			appendString.insert(appendString.begin(), board.getSquare(yIndex-1-a, xIndex)->getLetter());
			a++;
			b++;
		}
	}

	//extraTiles contains base values of extra tiles
	if(!dictionary.isLegalWord(appendString))
	{
		throw FileException("INVALID APP WORD");
	}

	//now figure out the value of the main word

	unsigned int totalMultVal=1;
	unsigned int runningLVal=0;
	for(size_t i=0; i<lMultipliers.size(); i++)
	{
		tileValues[i] = lMultipliers[i]*tileValues[i];
		totalMultVal*=wMultipliers[i];
		runningLVal = runningLVal + tileValues[i];
	}

	totalMultVal *= runningLVal;
	//totalMultVal now contains the points for the main word

	std::vector<std::pair<std::string, unsigned int> > listOfPotentialWords;
	listOfPotentialWords = board.getPlaceMoveResults(*this);

	for(size_t i=0; i<listOfPotentialWords.size(); i++)
	{
		std::string wordToCheck = listOfPotentialWords[i].first;
		if(dictionary.isLegalWord(wordToCheck))
		{
			totalMultVal += listOfPotentialWords[i].second;
		}
		else
		{
			throw FileException("MORE WORDS INVALID");
		}
	}

	board.executePlaceMove(*this);

	_player->addTiles(bag.drawTiles(tilesToPlace.size()));

	totalPoints = totalMultVal;

	if(tilesToPlace.size() == _player->getMaxTiles())
	{
		totalPoints += 50;
	}

	_player->addToScore(totalPoints);

}