#include <algorithm>
#include <vector>
#include <iostream>
#include "functor.h"


bool FatalistHypothesis(std::vector<std::pair<int,int> > grades)
{
	PairComp comp;
	try{
		std::sort(grades.begin(), grades.end(), comp);
	}
	catch (int n)
	{
		if(n==1)
		{
			return false;
		}
	}
	return true;
}
