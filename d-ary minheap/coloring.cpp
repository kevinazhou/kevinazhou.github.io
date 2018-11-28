#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <queue>

using namespace std;

void FillAdjacency(char** map, bool** visited, int** adjMatrix, stack<pair<int, int> >& borders, int rows, int columns);
bool ValidColor(int** adjMatrix, int country, int colorMatrix, int color, int numOfCountries);
bool ColorTheMap(int countryToColor, int** adjMatrix, int* colorMatrix, int numOfCountries);

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

  	int numOfCountries, rows, columns;

  	stringstream configStream(configuration);

  	configStream>>numOfCountries;
  	configStream>>rows;
  	configStream>>columns;//config values now set

  	char** map = new char*[rows];
  	bool** visited = new bool*[rows];
	for(int i=0; i<rows; i++)//creating the 2D arrays
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

	int** adjMatrix = new int*[numOfCountries];//adjacency matrix
	for(int i=0; i<numOfCountries; i++)
	{
		adjMatrix[i] = new int[numOfCountries];
		for(int j=0; j<numOfCountries; j++)
		{
			adjMatrix[i][j] = 0;
		}
	}

	stack<pair<int, int> > borders;
	pair<int, int> firstRoot;
	firstRoot.first = 0;
	firstRoot.second = 0;
	borders.push(firstRoot);//start at (0,0)

	while(!borders.empty())//fill out the adjacency matrix
	{
		FillAdjacency(map, visited, adjMatrix, borders, rows, columns);
	}

	int* colorMatrix = new int[numOfCountries];
	for(int c=0; c<numOfCountries; c++)//initialize the colorMatrix with no colors
	{
		colorMatrix[c] = 0;
	}

	ColorTheMap(0, adjMatrix, colorMatrix, numOfCountries);//fill colorMatrix

	for(int JoJo=0; JoJo<numOfCountries; JoJo++)//cout
	{
		char Dio = JoJo + 'A';
		cout<<Dio<<" "<<colorMatrix[JoJo]<<endl;
	}


	for(int i=0; i<rows; i++)
	{
		delete[] map[i];
		delete[] visited[i];
	}
	delete[] map;
	delete[] visited;

	for(int i=0; i<numOfCountries; i++)
	{
		delete[] adjMatrix[i];
	}
	delete[] adjMatrix;
	delete[] colorMatrix;

  	return 0;
	
}

void FillAdjacency(char** map, bool** visited, int** adjMatrix, stack<pair<int, int> >& borders, int rows, int columns)
{
	int yStart = -1;
	int xStart = -1;
	while(!borders.empty())//keeps track of countries that still need to be visited
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

	if(yStart==-1 && xStart==-1)//if all countries visited
	{
		return;
	}

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
		}

		if(y!=0)//checking index above
		{
			if(currentLetter==map[y-1][x] && !visited[y-1][x])//the index above
			{
				pair<int, int> toAddToNext;
				toAddToNext.first = y-1;
				toAddToNext.second = x;
				next.push(toAddToNext);
			}
			if(currentLetter!=map[y-1][x])//borders
			{
				if(!visited[y-1][x])
				{
					pair<int, int> toAddToBorders;
					toAddToBorders.first = y-1;
					toAddToBorders.second = x;
					borders.push(toAddToBorders);	
				}

				char otherLetter = map[y-1][x];
				adjMatrix[currentLetter-'A'][otherLetter-'A'] = 1;
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
			if(currentLetter!=map[y][x-1])
			{
				if(!visited[y][x-1])
				{
					pair<int, int> toAddToBorders;
					toAddToBorders.first = y;
					toAddToBorders.second = x-1;
					borders.push(toAddToBorders);
				}


				char otherLetter = map[y][x-1];
				adjMatrix[currentLetter-'A'][otherLetter-'A'] = 1;
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
			if(currentLetter!=map[y+1][x])
			{
				if(!visited[y+1][x])
				{
					pair<int, int> toAddToBorders;
					toAddToBorders.first = y+1;
					toAddToBorders.second = x;
					borders.push(toAddToBorders);
				}
				char otherLetter = map[y+1][x];
				adjMatrix[currentLetter-'A'][otherLetter-'A'] = 1;
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
			if(currentLetter!=map[y][x+1])
			{
				if(!visited[y][x+1])//only add when necessary
				{
					pair<int, int> toAddToBorders;
					toAddToBorders.first = y;
					toAddToBorders.second = x+1;
					borders.push(toAddToBorders);
				}
				char otherLetter = map[y][x+1];
				adjMatrix[currentLetter-'A'][otherLetter-'A'] = 1;
			}
		}
	}
}

bool ValidColor(int** adjMatrix, int country, int* colorMatrix, int color, int numOfCountries)
{
	for(int i=0; i<numOfCountries; i++)//making sure adjacent countries aren't same color
	{
		if(adjMatrix[country][i] && colorMatrix[i]==color)
		{
			return false;
		}
	}
	return true;
}

bool ColorTheMap(int countryToColor, int** adjMatrix, int* colorMatrix, int numOfCountries)
{
	if(countryToColor == numOfCountries)
	{
		return true;//base case
	}

	for(int colorNum=1; colorNum<=4; colorNum++)//all maps can be colored by max 4 colors
	{
		if(ValidColor(adjMatrix, countryToColor, colorMatrix, colorNum, numOfCountries))
		{
			colorMatrix[countryToColor] = colorNum;

			if(ColorTheMap(countryToColor+1, adjMatrix, colorMatrix, numOfCountries)==true)
			{
				return true;//all countries colored
			}

			colorMatrix[countryToColor] = 0;//backtracing

		}
	}
	return false;
}