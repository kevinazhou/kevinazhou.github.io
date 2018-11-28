#ifndef FUNCTOR_H_
#define FUNCTOR_H_

#include <string>
#include <cctype>

struct AlphaStrComp {
  bool operator() (const std::string& lhs, const std::string& rhs)
  { // Uses string's built in operator<
    // to do lexicographic (alphabetical) comparison
    return (lhs < rhs);
  }
};

struct LengthStrComp {
  bool operator()(const std::string& lhs, const std::string& rhs)
  { // Compares strings by size;
    // Breaks ties by lexicographic comparison
    return (   (lhs.size() < rhs.size())
            || (lhs.size() == rhs.size() && lhs < rhs));
  }
};

struct NumStrComp {
  bool operator() (const std::string& lhs, const std::string& rhs)
  { 
    int leftSum = 0;
    int rightSum = 0;
    leftSum = sumFunc(lhs);
    rightSum = sumFunc(rhs);

    if(leftSum == rightSum)
    {
      return (lhs < rhs);
    }
    else
    {
      return (leftSum < rightSum);
    }
  }

  int sumFunc(const std::string& word)
  {
    int sum = 0;
    for(size_t i = 0; i <word.size(); i++)
    {
      if(isdigit(word[i]))
      {
        int val = word[i] - '0';
        sum += val;
      }
      if(isalpha(word[i]))
      {
        int val = toupper(word[i]) -'A' + 10;
        sum += val;
      }
    }
    return sum;
  }
};


struct PairComp {
  bool operator() (const std::pair<int,int> pair1, const std::pair<int,int> pair2)
  {
    if(pair1.first < pair2.first && pair1.second>pair2.second)
    {
      throw 1;
    }
    if(pair1.second < pair2.second && pair1.first > pair2.first)
    {
      throw 1;
    }
    return (pair1.first < pair2.first);
  }
};

#endif