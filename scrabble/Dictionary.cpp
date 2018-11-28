#include "Dictionary.h"
#include "Exceptions.h"
#include <string>

#include <fstream>
//#include <iterator>

Dictionary::Dictionary(std::string dictionary_file_name)
{

	std::ifstream dictInputStream(dictionary_file_name);

	if(!dictInputStream)
	{
		throw FileException("DICTIONARY");
	}

	//adding every word into the hashtable dictionary structure

	//make sure everything is in lowercase
	while(!dictInputStream.eof())
	{
		std::string word;

		dictInputStream >> word;

		std::pair<std::string, std::string> p(word, word);

		dictMap.insert(p);

	}
}

Dictionary::~Dictionary() {}

bool Dictionary::isLegalWord(std::string const & word) const
{
	if(dictMap.find(word) == dictMap.end())
	{
		return false;
	}
	
	return true;
}