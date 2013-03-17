#include "provided.h"
#include <string>
#include "MyMap.h"
#include <algorithm>
using namespace std;

class SearcherImpl
{
public:
    vector<string> search(string terms);
    bool load(string filenameBase);	

private:
	Indexer m_index;
	MyMap<string, int> m_terms;  //store the search terms
	MyMap<string, int> m_score;  //store the score for each url
};


//MyMap doesn't do what I want
//order the scores with a binary search tree FWAHAHAHA
struct Node
{
	Node(string a, int b)
	{
		url = a;
		score = b;
		left = NULL;
		right = NULL;
		more = NULL;
	}
	string url;
	int score;
	Node* left;
	Node* right;
	Node* more;
};

class binaryTree
{
public:
	binaryTree() { m_root = NULL; m_size = 0;}
	~binaryTree() { FreeTree(m_root); }
	
	void FreeTree(Node* cur)
	{
		if (cur == NULL)  	
         return;
		
		FreeTree(cur->left); 
		FreeTree (cur-> right);
		FreeTree(cur->more);
		delete cur;                   
	}

	void insert(string url, int score)
	{
		m_size++;
		if (m_root == NULL)       
		{   
			m_root = new Node(url, score);   
			return; 
		}

		Node *cur = m_root;
		for (;;)    
		{
			if (score == cur->score)   
			{
				if(cur->more == NULL)
				{
					cur->more = new Node(url, score);
					return;
				}
				else  //there is already a url with the same score, go to the end
				{
					Node* morePtr = cur;  
					{
						while(morePtr->more != NULL)
							morePtr = morePtr->more;
						morePtr->more = new Node(url, score);
						return;
					}
				}
			}
			else if(score == cur->score)
			{

			}

			else if (url < cur->url)          
			{
				if (cur->left != NULL)                   
					cur = cur->left;
				else
				{
					cur->left = new Node(url, score);
					return;
				}
			}
			else if (url > cur->url)
			{
				if (cur->right != NULL)                    
					cur = cur->right;
				else 
				{
					cur->right = new Node(url, score);
					return;
				}
			}
		}
	}

	void InOrder(Node *cur)
	{
		if (cur == NULL)  	  // if empty, return…
			return;

		InOrder(cur-> right);
		InOrder(cur->more);    
		final.push_back(cur->url);      // Process the current node.
		InOrder(cur->left);  
	}

	vector<string> result()
	{
		InOrder(m_root);
		return final;
	}
	
private:
	Node* m_root;
	int m_size;
	vector<string> final;
};

vector<string> SearcherImpl::search(string terms)
{
	//*************** getting N, the number of words in the terms integer *************
	m_terms.clear();
	string w = terms;
	strToLower(w);
	Tokenizer t(w);
	while(t.getNextToken(w))         //***********S words in the search query, O(S)
	{
		//(don't actually need the count, would've preferred a set (only has unique items)
		//but can't use sets so I'm use the map like a set

		//if m_terms map has the word
		//countPtr will be a pointer to the count
		//otherwise it will be NULL
		int* countPtr = m_terms.find(w);              //*********average S words in the map of terms, O(logS)
		if(countPtr == NULL)
		{
			//add the word to the m_words map
			m_terms.associate(w, 1);                 //********average S words in the map of terms, O(logS)
		}
	}
	//number of unique words in the term string
	int N = m_terms.size();
	//************** end of getting number of unique words in string *******************
	
	//word will contain the current word in m_terms
	string word;

	if(!m_terms.getFirst(word))  //there is NOT a 1st word in the m_terms (user gives no search terms, derp)
		return vector<string>();   //return an empty vector
	
	m_score.clear();
	//for each term (each word in m_terms)
	for(int i=0; i<N; i++)                    //********S words that are being looked up, O(S)
	{
		if(i != 0)  //don't called getNext for first loop only (because getFirst was called already)
		{		
			m_terms.getNext(word);
		}
		//look up the word in m_terms in the index
		//for the word, get a vector with urlCount
		//(a vector of urls which contain the word and their count)
		vector<UrlCount> urlCounts = m_index.getUrlCounts(word);  //Indexer holds N words and each word is associated with U URLs on average
		                                                          //**********then getUrlCOounts() is O(UlogN)

		//********* Update score for each url *************
		//for each url in the vector
		//update the score for that url
		
		//score calculation
		// score = add up the number of times each word appears in a url

		for(unsigned int k=0; k<urlCounts.size(); k++)      //*****U associations, O(U)
		{
			string url = urlCounts[k].url;
			//find the url in the m_score
			//the vector that contains word to ID counts
			int* scorePtr = m_score.find(url);             //******U associations, O(logU)
			
			//url is not yet in m_score map so add it
			if(scorePtr == NULL)
			{
				m_score.associate(url, 1);  //add to the map    //******U associations, O(logU)
			}
			else
			{
				//increase the score by the number of occurrences of the word in the url page
				(*scorePtr)+= urlCounts[k].count;

			}
		}
		//******** end of update score for each url ************************
	}	//end of calculating score

	//********************** sort the urls by their score *******************
	int T;
	if(int(0.7*N) > 1)
		T = int(0.7*N);
	else
		T = 1;
		
	string matchUrl;

	int* scorePtr = m_score.getFirst(matchUrl);

	if(!scorePtr)  //there is NOT a 1st word in the m_score 
		return vector<string>();   //return an empty vector

	binaryTree tree;

	for(int j=0; j< m_score.size(); j++)            //*******U associations, O(logU)
	{
		if(j != 0)  //don't called getNext for first loop only (because getFirst was called already)
		{		
			scorePtr = m_score.getNext(matchUrl);
		}
		if(*scorePtr >= T)
			tree.insert(matchUrl, *scorePtr);      //*******C terms that constitute as a match, O(logC)
	}

	//return the result vector! fwahahahahahahaha
	return tree.result();                            //******C terms that constitute as a match, O(C)
}

bool SearcherImpl::load(string filenameBase)
{
	return m_index.load(filenameBase);
}

//******************** Searcher functions *******************************

// These functions simply delegate to SearcherImpl's functions.
// You probably don't want to change any of this code.

Searcher::Searcher()
{
    m_impl = new SearcherImpl;
}

Searcher::~Searcher()
{
    delete m_impl;
}

vector<string> Searcher::search(string terms)
{
    return m_impl->search(terms);
}

bool Searcher::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}
