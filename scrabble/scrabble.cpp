#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <stdio.h> 

#include "Board.h"
#include "Player.h"
#include "Bag.h"
#include "Dictionary.h"
#include "Move.h"
#include "ConsolePrinter.h"
#include "Exceptions.h"

#define getName(var)  #var 
using namespace std;

string makeLower(string s);

int main(int argc, char* argv[])
{

	if(argc < 2)
  	{
  		cout << "Please provide configuration filename\n";
    	return 1;
  	}

  	size_t HANDSIZE;
  	uint32_t seed;
  	string bagSecured, dictionaryFile, boardFile;


  	ifstream file (argv[1]);

  	if(!file)
  	{
  		throw FileException("CONFIG FILE");
  	}

  	while(!file.eof())
  	{
  		string settings;
  		file >> settings;

  		if(settings == "HANDSIZE:")
  		{
  			string temp;
  			file >> temp;
  			stringstream ss(temp);
  			ss>>HANDSIZE;
  		}
  		if(settings == "TILES:")
  		{
  			file >> bagSecured;
  		}
  		if(settings == "DICTIONARY:")
  		{
  			file >> dictionaryFile;
  		}
  		if(settings == "BOARD:")
  		{
  			file >> boardFile;
  		}
  		if(settings =="SEED:")
  		{
  			string temp;
  			file >> temp;
  			stringstream ss(temp);
  			ss>>seed;
  		}
  	}

  	Bag baggy(bagSecured, seed);

  	Dictionary dictionary(dictionaryFile);

  	Board board(boardFile);

  	cout<<"How many players?"<<endl;

  	size_t numPlayers;
  	cin>>numPlayers;

  	vector<Player*> players;

  	for(size_t i=0; i<numPlayers; i++)
  	{
  		cout<<"Player "<<i+1<<"'s name: ";
  		string name;
  		cin>>name;
  		players.push_back(new Player(name, HANDSIZE));
  	}

  	for(size_t i=0; i<numPlayers; i++)
  	{
  		players[i]->addTiles(baggy.drawTiles(HANDSIZE));
  	}

  	size_t turn = 0;
  	size_t consecutivePasses=0;
  	bool playHasStarted = false;
  	while(1)
  	{
  		ConsolePrinter::printBoard(board);
  		size_t whichPlayer = (turn%numPlayers)+1;
  		turn++;
  		ConsolePrinter::printHand(*(players[whichPlayer-1]));
label:
  		cout<<"Player "<< whichPlayer <<"'s turn: "<<endl;
  		if(!playHasStarted)
  		{
  			cout<<"Play must start at Y index " << board.getYStart()<< " and X index "<< board.getXStart()<<endl;
  		}

  		string moveString;

      cin >> moveString;
  		moveString = makeLower(moveString);

      if(moveString=="pass")
      {
        consecutivePasses++;
      }

      else if(moveString=="place")
      {
        for(int yeet=0; yeet<4; yeet++)
        {
          moveString += ' ';
          string extra;
          cin>>extra;
          extra = makeLower(extra);
          moveString.append(extra);
        }
        consecutivePasses=0;
      }

      else if(moveString=="exchange")
      {
        moveString += ' ';
        string extra;
        cin>>extra;
        moveString.append(extra);
        extra = makeLower(extra);
        consecutivePasses=0;
      }

      else if(moveString!="pass")//maybe ask again here?
      {
        cout<<"Unrecognized input."<<endl;
        goto label;
      }


  		if(!playHasStarted)//checking starting position
  		{
  			stringstream m(moveString);
  			string firstWord;
  			m >> firstWord;
  			size_t xIn;
  			size_t yIn;
  			if(firstWord == "place")
  			{
  				m >> firstWord;
  				m >> yIn;
  				m >> xIn;
  				if(board.getXStart()!=xIn || board.getYStart()!=yIn)
  				{
  					cout<<"ERROR: INCORRECT STARTING POSITION\n";
            goto label;
  				}
  				else
  				{
  					playHasStarted = true;
  				}
  			}
  		}

  		try
      {
        Move* move = Move::parseMove(moveString, *(players[whichPlayer-1]));

        move->execute(board, baggy, dictionary);

        delete move;

      }
      catch(FileException ex)
      {
        if(ex.getMessage()=="EXCHANGE")
        {
          cout<<"ERROR: DON'T HAVE THOSE TILES\n";
          goto label;
        }
        if(ex.getMessage()=="SQUARE TAKEN")
        {
          cout<<"ERROR: STARTING POSITION TAKEN"<<endl;
          goto label;
        }
        if(ex.getMessage()=="TILES NOT IN HAND")
        {
          cout<<"ERROR: THOSE TILES NOT IN HAND"<<endl;
          goto label;
        }
        if(ex.getMessage()=="EXCEEDS BOUNDS")
        {
          cout<<"ERROR: WORD EXCEEDS BOARD BOUNDARIES"<<endl;
          goto label;
        }
        if(ex.getMessage()=="INVALID APP WORD")
        {
          cout<<"ERROR: INVALID MAIN WORD"<<endl;
          goto label;
        }
        if(ex.getMessage()=="MORE WORDS INVALID")
        {
          cout<<"ERROR: ONE OR MORE WORDS ARE INVALID"<<endl;
          goto label;
        }
        if(ex.getMessage()=="WORD NOT ATTACHED")
        {
          cout<<"ERROR: WORD MUST BE ATTACHED TO ANOTHER WORD"<<endl;
          goto label;
        }
      }

  		cout<<"Player "<< whichPlayer <<"'s score is "<<players[whichPlayer-1]->getScore()<<endl;

      ConsolePrinter::printHand(*(players[whichPlayer-1]));

  		if(consecutivePasses==numPlayers)
  		{
  			cout<<"Game has ended."<<endl;

  			size_t winner;

  			unsigned int winnerScore = 0;
  			for(size_t i=0; i<numPlayers; i++)
  			{
  				unsigned int score = players[i]->getScore();
  				cout<<"Player "<< i+1 <<"'s score is "<< score<<endl;
  				if(winnerScore<=score)
  				{
  					winner = i;
  				}
  			}

  			cout<<"Player "<< winner+1 <<" is the winner";
  			cout<<" with a score of "<<winnerScore;
        break;
  		}
  		if(baggy.tilesRemaining()==0 && players[whichPlayer-1]->getRemaining()==0)
  		{
  			cout<<"Game has ended."<<endl;

  			size_t winner;

  			unsigned int winnerScore = 0;
  			for(size_t i=0; i<numPlayers; i++)
  			{
  				unsigned int score = players[i]->getScore();
  				cout<<"Player "<< i+1 <<"'s score is "<< score<<endl;
  				if(winnerScore<=score)
  				{
  					winner = i;
  				}
  			}

  			cout<<"Player "<< winner+1 <<", "<< getName(players[i])<< " is the winner";
  			cout<<" with a score of "<<winnerScore;
        break;
  		}



  	}


  	for(size_t i=0; i<numPlayers; i++)
  	{
  		delete players[i];
  	}

}

string makeLower(string s)
{
	for(size_t i=0; i<s.size(); i++)
	{
		if('A'<=s[i] && s[i]<='Z')
		{
			char c = ((int)s[i])+32;

			s[i]=c;
		}
	}
	return s;
}