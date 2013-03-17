#include <iostream>
#include <sstream>
#include <fstream>
#include "provided.h"
#include "MyMap.h"
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

struct idCount
{
	int id;
	int count;
};

class IndexerImpl
{
public:
    bool incorporate(string url, WordBag& wb);
    vector<UrlCount> getUrlCounts(string word);
    bool save(string filenameBase);
    bool load(string filenameBase);
private:
	MyMap<string, int> urlToID;
	MyMap<int, string> idToUrl;
	MyMap<string, vector<idCount> > wordToIDCounts;
};

bool IndexerImpl::incorporate(string url, WordBag& wb)
{
    int* s = urlToID.find(url);  //**********N words in index, O(logN)
	if(s != NULL)
		return false;	//url was already used
	
	int random = rand() % 200;         // random number in the range 0 to 199
	urlToID.associate(url, random);   //associate each URL to an ID number
	idToUrl.associate(random, url);    //associate ID number to URL

	string word;
	int count;

	if(!wb.getFirstWord(word, count))  //there is NOT a 1st word in the word bag
	{
				return true;
	}	
	for(int i=0;; i++)  //*********** W words in wordbag, O(W)
	{
		if(i != 0)  //do once for the first loop only
		{
			if(wb.getNextWord(word, count) != true)
				break;
		}
	
		//add word to index
		//first check if the word is in the index
		vector<idCount>* vecPtr = wordToIDCounts.find(word);       //********** N words in index, O(logN)
		if(vecPtr == NULL)  //word NOT found in index
		{	
			//create an empty urlCount vector
			vector<idCount> temp;
			wordToIDCounts.associate(word, temp);    

			//index (wordToIdCounts) contains the word now
		//add the idCount to wordToIdCounts 
		
		idCount idCounter;
		idCounter.id = random;  
		idCounter.count = count;

		//get a pointer to the vector of the word
		vecPtr = wordToIDCounts.find(word);                       //************ N words in index, O(logN)
		vecPtr->push_back(idCounter);

		}
		else  //word already in index
		{
			idCount idCounter;
			idCounter.id = random;  
			idCounter.count = count;

			vector<idCount> vec = *(wordToIDCounts.find(word)); 
			vec.push_back(idCounter);
			wordToIDCounts.associate(word, vec);  
		}

		
		 
	}
	return true;
}

//returns vector of UrlCounts for the specified word
vector<UrlCount> IndexerImpl::getUrlCounts(string word)
{
	strToLower(word);

	//the vector that contains word to ID counts
	vector<idCount>* urls = wordToIDCounts.find(word);           //*****************N words in index, O(logN)
	
	//if the word is not found, return an empty vector
	if(urls == NULL)
	{
		vector<UrlCount> empty;
		return empty;
	}
	else  //found the word in wordToIDCounts and now have a pointer to that vector
	{
		//make a UrlCount vec
		vector<UrlCount> urlCountVec;

		//go through the idCount vector
		vector<idCount>::iterator itr;
		for(itr = urls->begin(); itr != urls->end(); itr++)     //******************each word in associated with U URLS on average
		{														//******************each outer loop is O(U)
			int id = itr->id;
			//create a new UrlCount object
			UrlCount urlCounter;
			urlCounter.url = *(idToUrl.find(id));               //*****************N words, in index, O(logN)
			urlCounter.count = itr->count;
			urlCountVec.push_back(urlCounter);
		}

		return urlCountVec; 
	}                
}

//**************writeItem functions*************************
//write any int
void writeItem(ostream& stream, int m)
{
	stream << m << endl;
}

//write any string
void writeItem(ostream& stream, string m)
{
	stream << m << endl;
}

//write urlToId (MyMap of string to int)
void writeItem(ostream& stream, MyMap<string, int>& m)
{
	//save the number of associations in MyMap m to outfile
	writeItem(stream, m.size());
	
	string url;

	for (int* IDptr = m.getFirst(url); IDptr != NULL; IDptr = m.getNext(url))
	{
		writeItem(stream, url);   //save the key item
		writeItem(stream, *IDptr); //save the value item
	}
}

//write to idToUrl (MyMap of int to string)
void writeItem(ostream& stream, MyMap<int, string>& m)
{
	//save the number of associations in MyMap m to outfile
	writeItem(stream, m.size());

	int id;

	//urlptr points to the url value 
	//id is now the current id
	for (string* urlptr = m.getFirst(id); urlptr != NULL; urlptr = m.getNext(id))
	{
		writeItem(stream, id);      //save the key item (id)
		writeItem(stream, *urlptr); //save the value item (url)
	}
}

//write to idCount (needed for vector<idCount>)
void writeItem(ostream& stream, idCount& m)
{
	//save each of the two data members
	writeItem(stream, m.id);
	writeItem(stream, m.count);
}

//write to vector<idCount> (needed for MyMap<string, vector<idCount>)
void writeItem(ostream& stream, vector<idCount>& m)
{
	//save the number of items in vector m to outfile
	writeItem(stream, m.size());

	//iterate through the vector
	//save each item in the vector
	for(unsigned int i=0; i < m.size(); i++)
	{
		writeItem(stream, m[i]);
	}
}

//write to wordToIDCounts (MyMap of string to vector<idCount>
void writeItem(ostream& stream, MyMap<string, vector<idCount> >& m)
{
	//save the number of associations in MyMap m to outfile
	writeItem(stream, m.size());

	string word;
	//iterate through the map
	for (vector<idCount>* idVecPtr = m.getFirst(word); idVecPtr != NULL; idVecPtr = m.getNext(word))
	{ 
		//save each key and value
		writeItem(stream, word);           //save the key item (word)
		writeItem(stream, *idVecPtr);      //save the value item (vector of idCount)
	}
}

//save MyMap
template<typename KeyType, typename ValueType>
bool saveMyMap(string filename, MyMap<KeyType, ValueType>& m)
{
	ofstream stream(filename);   // outfile is a name of our choosing.
	if ( !stream )		   // Did the creation fail?
	{
	    return false; //unable to create the output file
	}
			
	//visit the associations in level-order traversal
	//write item depends on the type of MyMap m is 
	writeItem(stream, m);
	
	return true;
}

//***************end of writeItem functions***************************

bool IndexerImpl::save(string filenameBase)
{
    return saveMyMap(filenameBase + "u2i.txt", urlToID) &&
		   saveMyMap(filenameBase + "i2u.txt", idToUrl) &&
		   saveMyMap(filenameBase + "w2ic.txt", wordToIDCounts);
}

//read in an int
//this code is provided in spec
bool readItem(istream& stream, int& m)
{
	string line;
	if ( ! getline(stream, line)) // return false if getline fails
		return false;
	// create an input stringstream whose text will be a copy of line
	istringstream iss(line); // #include <sstream> for istringstream
	// Try to extract an int from the stream, returning true if it
	// succeeds, or false if it fails (because the line has no
	// integer). 
	return iss >> m;
}

//read in a string
bool readItem(istream& stream, string& s)
{
 // Call getline and return true for success, false for failure
 // (e.g., end of file: there is no next line in the file to get)
 return getline(stream, s);
}

//read in idCounter
bool readItem(istream& stream, idCount& idCounter)
{
	if(readItem(stream, idCounter.id) == false)
		return false;
	if(readItem(stream, idCounter.count) == false)
		return false;
	return true;
}

//read in vector<idCount>
//remember: MyMap<string, vector<idCount> > wordToIDCounts;
bool readItem(istream& stream, vector<idCount>& idVec)
{
	//get the size of the vector
	int size; 
	if(readItem(stream, size) == false)
		return false;

	//idCounter will hold the current idCount
	idCount idCounter;
	//read each item in the vector and place it into idVec
	for(int i=0; i<size; i++)
	{
		if(readItem(stream, idCounter) == false)
			return false;
		idVec.push_back(idCounter);
	}
	return true;
}

template<typename KeyType, typename ValueType>
bool loadMyMap(string filename, MyMap<KeyType, ValueType>& m)
{
	//clear m of an association it currently holds
	m.clear();

	ifstream stream(filename);   // infile is a name of our choosing.
	if ( !stream )		   // Did opening the file fail?
	{
	    return false; //unable to create the output file
	}
	
	//read the number of associations in m from stream
	int n;
	
	//the first line should be the number of associations in the map
	if(readItem(stream, n) == false)
		return false;

	for(int i=0; i<n; i++)
	{
		//read in the key item
		KeyType key;
		if(readItem(stream, key) == false)
			return false;
		
		//read in value
		ValueType value;
		if(readItem(stream, value) == false)
			return false;
		
		m.associate(key, value);
	}
	return true;
}

bool IndexerImpl::load(string filenameBase)
{
	return loadMyMap(filenameBase + "u2i.txt", urlToID) &&
		loadMyMap(filenameBase + "i2u.txt", idToUrl) &&
		loadMyMap(filenameBase + "w2ic.txt", wordToIDCounts);
}

//******************** Indexer functions *******************************

// These functions simply delegate to IndexerImpl's functions.
// You probably don't want to change any of this code.

Indexer::Indexer()
{
    m_impl = new IndexerImpl;
}

Indexer::~Indexer()
{
    delete m_impl;
}

bool Indexer::incorporate(string url, WordBag& wb)
{
    return m_impl->incorporate(url, wb);
}

vector<UrlCount> Indexer::getUrlCounts(string word)
{
    return m_impl->getUrlCounts(word);
}

bool Indexer::save(string filenameBase)
{
    return m_impl->save(filenameBase);
}

bool Indexer::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}
