#include "Board.h"
#include "Exceptions.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

Board::Board(std::string board_file_namey)
{
	std::ifstream boardFileStream(board_file_namey);

	if(!boardFileStream)
	{
		throw FileException("BOARD");
	}

	std::string dimString;
	std::getline(boardFileStream, dimString);
	std::istringstream dimSS(dimString);

		if(dimSS >> width >> height)
		{
			std::cout << "Width: " << width << std::endl;
			std::cout << "Height: " << height << std::endl;
		}


	std::string startString;
	std::getline(boardFileStream, startString);
	std::istringstream startSS(startString);

		if(startSS >> xStart >> yStart)
		{
			std::cout << "xStart: " << xStart << std::endl;
			std::cout << "yStart: " << yStart << std::endl;
		}
	


	size_t currentRow = 0;
	while(currentRow<height)
	{
		std::string oneRow;
		boardFileStream >> oneRow;

		std::vector<Square*> row;//creating the individual rows

		for(size_t i=0; i<oneRow.size(); i++)
		{
			if(currentRow+1==yStart && i+1==xStart)//starting square
			{
				if(oneRow[i]=='2')
				{
					Square* temp = new Square(2, 1, true);
					row.push_back(temp);
				}
				else if(oneRow[i]=='3')
				{
					Square* temp = new Square(3, 1, true);
					row.push_back(temp);
				}
				else if(oneRow[i]=='d')
				{
					Square* temp = new Square(1, 2, true);
					row.push_back(temp);
				}
				else if(oneRow[i]=='t')
				{
					Square* temp = new Square(1, 3, true);
					row.push_back(temp);
				}
				else if(oneRow[i]=='.')
				{
					Square* temp = new Square(1, 1, true);
					row.push_back(temp);
				}
			}
			else
			{
				if(oneRow[i]=='2')
				{
					Square* temp = new Square(2, 1, true);
					row.push_back(temp);
				}
				else if(oneRow[i]=='3')
				{
					Square* temp = new Square(3, 1, true);
					row.push_back(temp);
				}
				else if(oneRow[i]=='d')
				{
					Square* temp = new Square(1, 2, true);
					row.push_back(temp);
				}
				else if(oneRow[i]=='t')
				{
					Square* temp = new Square(1, 3, true);
					row.push_back(temp);
				}
				else if(oneRow[i]=='.')
				{
					Square* temp = new Square(1, 1, true);
					row.push_back(temp);
				}
			}


		}

		theBoard.push_back(row);

		currentRow++;


	}
}

Board::~Board ()
{
	for(size_t i=0; i<height; i++)
	{
		for(size_t j=0; j<width; j++)
		{
			delete theBoard[i][j];
		}
	}
}

std::vector<std::pair<std::string, unsigned int> > Board::getPlaceMoveResults(const PlaceMove &m) const
{
	//look at squares that are not yet occupied and figure out if they form possible new words

	size_t xWordStart = m.getX()-1;//indexing starts at 0 for the board
	size_t yWordStart = m.getY()-1;//so use -1


	std::string wordToForm = m.getNonAppended();
	size_t wordLength = wordToForm.size();

	std::vector<std::pair<std::string, unsigned int> > ret;

	if(m.horiz())//look for new vertical words formed
	{
		size_t track = 0;
		for(size_t x = xWordStart; x < xWordStart+wordLength; x++)
		{
			if(!(theBoard[yWordStart][x]->isOccupied()))//if not already occupied
			{
				//look up and down
				std::string potentialWord;
				std::vector<unsigned int> values;
				size_t a=0;
				size_t b=0;
				unsigned int wordMult = 1;
				unsigned int letMult = 1;
				if(yWordStart-a >0)
				{
					while(theBoard[yWordStart-a-1][x]->isOccupied())//seeing how far up the potential word is
					{
						a++;
					}
				}

				if(yWordStart+b+1 <height)
				{
					while(theBoard[yWordStart+b+1][x]->isOccupied())//seeing how far down
					{
						b++;
					}					
				}

				if(a!=0 || b!=0)
				{

					std::string tee = m.getString();
					for(size_t i=yWordStart-a; i<=yWordStart+b; i++)
					{

						if(i==yWordStart)//figuring out the letter that is placed value
						{
							letMult = theBoard[i][x]->getLMult();
							wordMult = theBoard[i][x]->getWMult();
							potentialWord += tee[track];
							track++;
							//potentialWord += theBoard[i][x]->getLetter();
							unsigned int letVal = theBoard[i][x]->getScore();
							values.push_back(letVal * letMult);
						}
						else
						{
							potentialWord += theBoard[i][x]->getLetter();
							values.push_back(theBoard[i][x]->getScore());
						}

					}	
					
					unsigned int finalVal = 0;
					for(size_t q=0; q<values.size(); q++)
					{
						finalVal += values[q];
					}
					finalVal *= wordMult;
					std::pair <std::string, unsigned int> pear;
					pear = std::make_pair(potentialWord, finalVal);
					ret.push_back(pear);				
				}


			}
		}
	}

	else
	{
		size_t track = 0;
		for(size_t y = yWordStart; y < yWordStart+wordLength; y++)
		{
			if(!(theBoard[y][xWordStart]->isOccupied()))//if not already occupied
			{
				//look up and down
				std::string potentialWord;
				std::vector<unsigned int> values;
				size_t a=0;
				size_t b=0;
				unsigned int wordMult = 1;
				unsigned int letMult = 1;
				if(xWordStart-a >0)
				{
					while(theBoard[y][xWordStart-a-1]->isOccupied())//seeing how far up the potential word is
					{
						a++;
					}					
				}

				if(xWordStart+b+1 <height)
				{
					while(theBoard[y][xWordStart+b+1]->isOccupied())//seeing how far down
					{
						b++;
					}					
				}

				if(a!=0 || b!=0)
				{
					
					std::string tee = m.getString();
					for(size_t i=xWordStart-a; i<=xWordStart+b; i++)
					{
						if(i==xWordStart)//figuring out the letter that is placed value
						{
							letMult = theBoard[y][i]->getLMult();
							wordMult = theBoard[y][i]->getWMult();

							potentialWord += tee[track];
							track++;
							//potentialWord += theBoard[y][i]->getLetter();
							unsigned int letVal = theBoard[y][i]->getScore();
							values.push_back(letVal * letMult);
						}
						else
						{	
							potentialWord += theBoard[y][i]->getLetter();
							values.push_back(theBoard[y][i]->getScore());
						}

					}	
					
					unsigned int finalVal = 0;
					for(size_t q=0; q<values.size(); q++)
					{
						finalVal += values[q];
					}
					finalVal *= wordMult;
					std::pair <std::string, unsigned int> pear;
					pear = std::make_pair(potentialWord, finalVal);
					ret.push_back(pear);				
				}


			}
		}
	}


	return ret;
}

void Board::executePlaceMove (const PlaceMove & m)
{	//multipliers should already be noted down, this
	//function simple changes the board
	size_t xPlace, yPlace;
	xPlace = m.getX()-1;//minus one since indexing of board starts with 0
	yPlace = m.getY()-1;

	std::string sizer = m.getString();

	//word needs to contain the raw data that was put in by the user
	std::string word = m.getRawData();


	Player* currentPlayer;
	currentPlayer = m.getPlayer();//getting player pointer
	std::vector<Tile*> wordTiles = currentPlayer->takeTiles(word, true);
	//now wordTiles has the vector of tiles

	if(m.horiz())
	{
		size_t i=0;
		size_t vectorIndex=0;
		for(size_t dap=0; dap<sizer.size(); dap++)
		{
			//if the square is occupied, go to next square
			if(xPlace+i<width)
			{
				if(theBoard[yPlace][xPlace+i]->isOccupied())
				{
					i++;
					theBoard[yPlace][xPlace+i]->placeTile(wordTiles[vectorIndex]);
					vectorIndex++;
				}
				else
				{
					theBoard[yPlace][xPlace+i]->placeTile(wordTiles[vectorIndex]);
					vectorIndex++;
				}
			}

			i++;
		}
	}
	else
	{
		size_t i=0;
		size_t vectorIndex=0;
		for(size_t dap=0; dap<sizer.size(); dap++)
		{
			if(yPlace+i<height)
			{
				//if the square is occupied, go to next square
				if(theBoard[yPlace+i][xPlace]->isOccupied())
				{
					i++;
					theBoard[yPlace+i][xPlace]->placeTile(wordTiles[vectorIndex]);
					vectorIndex++;
				}
				else
				{
					theBoard[yPlace+i][xPlace]->placeTile(wordTiles[vectorIndex]);
					vectorIndex++;
				}
			}
			i++;
		}

	}
}


Square * Board::getSquare (size_t y, size_t x) const
{
	x--;
	y--;
	uintptr_t r = (uintptr_t)theBoard[y][x];
	Square* ret = (Square*) r;
	return ret;
}

size_t Board::getRows() const
{
	return height;
}

size_t Board::getColumns() const
{
	return width;
}
