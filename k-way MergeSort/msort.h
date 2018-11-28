#ifndef MSORT_
#define MSORT_

#include <vector>
#include <queue>
#include <array>


#include "functor.h"


template <class T, class Comparator>
void mergeSort (std::vector<T>& myArray, int k, Comparator comp)
{
	size_t arraySize = myArray.size();

	if(arraySize == 1)//base case, arrays of size 1 are sorted
	{
		return;
	}

	std::vector<size_t> indices;
	size_t chunkSize = arraySize/k;

	if(chunkSize<1)//when the segments are less than k in length
	{
		for(size_t i = 0; i<arraySize; i++)
		{
			indices.push_back(i);
		}
	}
	else
	{	
		size_t remainder = arraySize - (chunkSize*k);

		//filling indices with correct indices
		for(int i = 0; i<k; i++)
		{
			if(i==0)
			{
				indices.push_back(0);
			}
			else if(remainder>0)
			{
				indices.push_back(indices[i-1] + chunkSize +1);
				remainder--;
			}
			else
			{
				indices.push_back(indices[i-1] + chunkSize); 
			}
		}
	}

	//now split myArray into different arrays
	std::vector<std::vector<T> > splitArrays;
	for(size_t ind = 0; ind<indices.size(); ind++)
	{
		if(ind == indices.size()-1)
		{
			std::vector<T> toAdd(myArray.begin()+indices[ind], myArray.end());
			splitArrays.push_back(toAdd);
		}
		else
		{
			std::vector<T> toAdd(myArray.begin()+indices[ind], myArray.begin()+indices[ind+1]);
			splitArrays.push_back(toAdd);
		}

	}

	//now use recursion
	for(size_t i = 0; i<splitArrays.size(); i++)
	{
		mergeSort(splitArrays[i], k, comp);
	}

	merge(myArray, splitArrays, comp);

}

template <class T, class Comparator>
void merge (std::vector<T>& myArray, std::vector<std::vector<T> >& arrays, Comparator comp)
{
	//every vector in arrays is sorted
	//myArray is unsorted

	size_t numArrays = arrays.size();

	std::priority_queue<T, std::vector<T>, Comparator > pQueue;

	size_t* indexTracker = new size_t[numArrays];

	//putting the first vals into the priority queue
	for(size_t i=0; i<numArrays; i++)
	{
		pQueue.emplace(arrays[i][arrays[i].size()-1]);
		indexTracker[i] = arrays[i].size();
	}

	size_t god = myArray.size() - 1;//to track the index we are at
	while(!pQueue.empty())
	{
		for(size_t i=0; i<numArrays; i++)
		{	//if top val is equal to one of the sorted arrays first val
			if(indexTracker[i]!=0)
			{
				if(pQueue.top()==arrays[i][indexTracker[i]-1])
				{
					indexTracker[i]--;
					myArray.at(god) = pQueue.top();
					god--;
					pQueue.pop();
					if(indexTracker[i]!=0)
					{
						pQueue.emplace(arrays[i][indexTracker[i]-1]);
					}

					break;
				}
			}
		}
	}

	//now myArray should be sorted
	delete[] indexTracker;


}



#endif