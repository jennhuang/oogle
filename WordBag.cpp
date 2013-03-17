#include "provided.h"
#include "MyMap.h"
#include <string>
#include <cstdlib>
using namespace std;

class WordBagImpl
{
public:
    WordBagImpl(const string& text);
    bool getFirstWord(string& word, int& count);
    bool getNextWord(string& word, int& count);

private:
	//string words, int number of each word
	MyMap<string, int> m_words;

};

WordBagImpl::WordBagImpl(const string& text)
{
	string w = text;
	strToLower(w);
	Tokenizer t(w);
	while(t.getNextToken(w))
	{
		//if m_words map has the word
		//countPtr will be a pointer to the count
		//otherwise it will be NULL
		int* countPtr = m_words.find(w);
		if(countPtr == NULL)
		{
			//add the word to the m_words map
			m_words.associate(w, 1);
		}
		else
		{
			//increment the count of the word
			(*countPtr)++;
		}
	}
}

bool WordBagImpl::getFirstWord(string& word, int& count)
{
	//count now holds the number of times the word appears
	//word holds the first word
	int* c = m_words.getFirst(word);
	if(c != NULL)
	{
		count = *c;
		return true;
	}
	else 
		//the map is empty
		return false;
}

bool WordBagImpl::getNextWord(string& word, int& count)
{
	int* c = m_words.getNext(word);
	if(c == NULL)
	{
		//no next word
		return false;
	}
	else
	{
		count = *c;
		return true;
	}
}

//******************** WordBag functions *******************************

// These functions simply delegate to WordBagImpl's functions.
// You probably don't want to change any of this code.

WordBag::WordBag(const std::string& text)
{
    m_impl = new WordBagImpl(text);
}

WordBag::~WordBag()
{
    delete m_impl;
}

bool WordBag::getFirstWord(string& word, int& count)
{
    return m_impl->getFirstWord(word, count);
}

bool WordBag::getNextWord(string& word, int& count)
{
    return m_impl->getNextWord(word, count);
}
