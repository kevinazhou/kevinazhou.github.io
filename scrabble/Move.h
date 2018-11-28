/*
 * Move.h
 *
 *  Created on: Sep 18, 2016
 *      Author: kempe
 */

/* The Move class family encodes a move made by one player, including its type
   (PASS, EXCHANGE, PLACE), the tiles used (and use for blanks),
   start square, and direction (if needed).
   Call Move::parseMove() to turn an entered string into its subclass of
   Move, then call apply() on that returned move to have it execute.*/

#ifndef MOVE_H_
#define MOVE_H_

#include <vector>
#include <string>
#include "Tile.h"
#include "Player.h"
#include "Bag.h"
#include "Dictionary.h"

// forward declaration to prevent circular includes
class Board;

class Move
{

public:


	/* Parses the given move m, and constructs the corresponding move subclass.
	   m is assumed to be in the format described on the assignment sheet for a move command.
	   The player passed in is used to verify that the proposed tiles are
	   in fact in the player's hand.
	   It can handle all three types of move (PASS, EXCHANGE, PLACE).
	   Coordinates start with 1.
	*/
	static Move * parseMove(std::string moveString, Player &p);


	/* Returns true iff the move is a PASS move */
	virtual bool isPass () const
	{
		return false;
	}

	/* Returns true iff the move is an EXCHANGE move */
	virtual bool isExchange () const
	{
		return false;
	}

	/* Returns true iff the move is a PLACE move */
	virtual bool isWord () const
	{
		return false;
	}

	/* Executes this move, whichever type it is.
	   This may throw exceptions; students: it's up to you to
	   decide (and document) what exactly it throws*/
	virtual void execute(Board & board, Bag & bag, Dictionary & dictionary) = 0;

	virtual ~Move() {}

	//Add more public/protected/private functions/variables here.
	virtual Player* getPlayer() const
	{
		return _player;
	}

protected:

	Player * _player;

	// constructor -- only accessible to subclasses
	Move(Player * player)
	{
		_player = player;
	}

};

// Represents a pass move, where the player takes no action
class PassMove : public Move
{

public:

	/* Constructs a pass move. */
	PassMove(Player * player);

	/* Returns true iff the move is a PASS move */
	bool isPass () const
	{
		return true;
	}

	/* Executes this move, whichever type it is.
	   This may throw exceptions; students: it's up to you to
	   decide (and document) what exactly it throws*/
	void execute(Board & board, Bag & bag, Dictionary & dictionary);

	//Add more public/protected/private functions/variables here.

};

// represents an exchange move, were a player replaces certain tiles
class ExchangeMove : public Move
{

public:
	/* Creates an EXCHANGE move, exchanging the tiles listed in the
	   string (formatted according to the EXCHANGE command description)
	   with new tiles from the bag.
	   */
	ExchangeMove(std::string tileString, Player * p);

	/* Returns true iff the move is an EXCHANGE move */
	bool isExchange () const
	{
		return true;
	}

	/* Executes this move, whichever type it is.
	   This may throw exceptions; students: it's up to you to
	   decide (and document) what exactly it throws*/
	void execute(Board & board, Bag & bag, Dictionary & dictionary);

	//Add more public/protected/private functions/variables here.
private:
	std::string tilesToExchange;

};

// represents a place move, where a player places one or more tiles onto the board.
class PlaceMove : public Move
{

public:
	/* Creates a PLACE move, starting at row y, column x, placing the tiles
	   described by the string tileString. If "horizontal" is true, then the tiles
	   are placed horizontally, otherwise vertically.
	   Coordinates start with 1.
	   The string m is in the format described in HW4; in particular, a '?'
	   must be followed by the letter it is to be used as.
	*/
	PlaceMove (size_t x, size_t y, bool horizontal, std::string tileString, Player * p);

	/* Returns true iff the move is a PLACE move */
	bool isWord () const
	{
		return true;
	}

	/* Returns the vector of tiles associated with a PLACE/EXCHANGE move.
	   Return value could be arbitrary for PASS moves. */
	std::vector<Tile*> const & tileVector () const;

	/* Executes this move, whichever type it is.
	   This may throw exceptions; students: it's up to you to
	   decide (and document) what exactly it throws*/
	void execute(Board & board, Bag & bag, Dictionary & dictionary);

	//Add more public/protected/private functions/variables here.
	size_t getX() const {return xIndex;}

	size_t getY() const {return yIndex;}

	bool horiz() const {return isHorizontal;}

	std::string getString() const {return tilesToPlace;}

	std::string getAppend() const {return appendString;}

	std::string getNonAppended() const {return wordWithoutAppend;}

	std::string getRawData() const {return rawWordData;}

	unsigned int getDirection() const {return appendDirection;}

private:
	size_t xIndex;
	size_t yIndex;
	bool isHorizontal;
	std::string rawWordData;
	std::string tilesToPlace;//after execute, will have letters being placed without the letter after ?
	std::string appendString;
	std::string wordWithoutAppend;//letters without the appends, board
	unsigned int appendDirection;//0 for none, 1 for letters in back, 2 for letters in front, 3 for both
	unsigned int totalPoints;
	std::vector<Tile*> tempTiles;
};



#endif /* MOVE_H_ */
