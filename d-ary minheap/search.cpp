#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <queue>

using namespace std;

void CountCountry(char** map, bool** visited, stack<pair<int, int> >& borders, int& max, int rows, int columns);

int main(int argc, char* argv[])
{
  	if(argc < 2)
  	{
  		std::cout << "Please provide filenames\n";
    	return 1;
  	}

  	ifstream file (argv[1]);

  	if(!file)
  	{
  		cout<<"Unable to open file"<<endl;
  		return 1;
  	}

  	string configuration;
  	getline(file, configuration);//configuration has the first line now

  	int numOfDifLetters, rows, columns;

  	stringstream configStream(configuration);

  	configStream>>numOfDifLetters;
  	configStream>>rows;
  	configStream>>columns;//config values now all set

  	char** map = new char*[rows];
  	bool** visited = new bool*[rows];
	for(int i=0; i<rows; i++)//creating arrays
	{
		map[i] = new char[columns];
		visited[i] = new bool[columns];
		string line;
		getline(file, line);

		for(int j=0; j<columns; j++)
		{
			map[i][j]=line[j];
			visited[i][j]=false;
		}
	}//now map contains the map of countries, visited is same size as map

	int max = 0;//the final value, keeps track of largest country
	stack<pair<int, int> > borders;
	pair<int, int> firstRoot;
	firstRoot.first = 0;
	firstRoot.second = 0;
	borders.push(firstRoot);//start at (0,0)

	while(!borders.empty())//keep counting countries until all countries counted
	{
		CountCountry(map, visited, borders, max, rows, columns);
	}

	cout<<max<<endl;


	for(int i=0; i<rows; i++)
	{
		delete[] map[i];
		delete[] visited[i];
	}
	delete[] map;
	delete[] visited;

  	return 0;
}

void CountCountry(char** map, bool** visited, stack<pair<int, int> >& borders, int& max, int rows, int columns)
{
	int yStart = -1;
	int xStart = -1;
	while(!borders.empty())//keeps track of what countries have not been visited yet
	{
		pair<int, int> root = borders.top();
		yStart = root.first;
		xStart = root.second;
		if(visited[yStart][xStart])
		{
			borders.pop();
		}
		else
		{
			break;
		}
	}//now we have the root of a country not yet counted

	if(yStart==-1 && xStart==-1)
	{
		return;
	}

	int countrySize = 0;

	queue<pair<int, int> > next;//never a visited one
	pair<int, int> start;
	start.first = yStart;
	start.second = xStart;
	next.push(start);

	while(!next.empty())
	{
		int y, x;
		char currentLetter;//letter at current index
		pair<int, int> current;//current index being analyzed

		do
		{
			current = next.front();
			next.pop();
			y = current.first;
			x = current.second;
		}while(visited[y][x] && !next.empty());

		currentLetter=map[y][x];

		if(!visited[y][x])
		{
			visited[y][x] = true;
			countrySize++;
		}

		if(y!=0)//checking the index above
		{
			if(currentLetter==map[y-1][x] && !visited[y-1][x])//the index above
			{
				pair<int, int> toAddToNext;
				toAddToNext.first = y-1;
				toAddToNext.second = x;
				next.push(toAddToNext);
			}
			if(currentLetter!=map[y-1][x] && !visited[y-1][x])//if border element
			{
				pair<int, int> toAddToBorders;
				toAddToBorders.first = y-1;
				toAddToBorders.second = x;
				borders.push(toAddToBorders);
			}
		}
		if(x!=0)//checking to the left
		{
			if(currentLetter==map[y][x-1] && !visited[y][x-1])//the index to the left
			{
				pair<int, int> toAddToNext;
				toAddToNext.first = y;
				toAddToNext.second = x-1;
				next.push(toAddToNext);
			}
			if(currentLetter!=map[y][x-1] && !visited[y][x-1])//border
			{
				pair<int, int> toAddToBorders;
				toAddToBorders.first = y;
				toAddToBorders.second = x-1;
				borders.push(toAddToBorders);
			}
		}
		if(y!=rows-1)//checking below
		{
			if(currentLetter==map[y+1][x] && !visited[y+1][x])//the index below
			{
				pair<int, int> toAddToNext;
				toAddToNext.first = y+1;
				toAddToNext.second = x;
				next.push(toAddToNext);
			}
			if(currentLetter!=map[y+1][x] && !visited[y+1][x])//border
			{
				pair<int, int> toAddToBorders;
				toAddToBorders.first = y+1;
				toAddToBorders.second = x;
				borders.push(toAddToBorders);
			}
		}
		if(x!=columns-1)//checking to the right
		{
			if(currentLetter==map[y][x+1] && !visited[y][x+1])//the index to the right
			{
				pair<int, int> toAddToNext;
				toAddToNext.first = y;
				toAddToNext.second = x+1;
				next.push(toAddToNext);
			}
			if(currentLetter!=map[y][x+1] && !visited[y][x+1])//border
			{
				pair<int, int> toAddToBorders;
				toAddToBorders.first = y;
				toAddToBorders.second = x+1;
				borders.push(toAddToBorders);
			}
		}
	}

	if(countrySize>max)
	{
		max = countrySize;
	}
}