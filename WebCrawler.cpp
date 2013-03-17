#include "provided.h"
#include "http.h"
#include <string>
#include <vector>
using namespace std;

class WebCrawlerImpl
{
public:
    void addUrl(string url);
    int getNumberOfUrls() const;
    void crawl(void (*callback)(string url, bool success));
    bool save(string filenameBase);
    bool load(string filenameBase);
private:
	Indexer m_index;
	vector<string> m_urlsNotCrawled;
};

void WebCrawlerImpl::addUrl(string url)
{
	m_urlsNotCrawled.push_back(url);
}

int WebCrawlerImpl::getNumberOfUrls() const
{
    return m_urlsNotCrawled.size();
}

void WebCrawlerImpl::crawl(void (*callback)(string url, bool success))
{
	string page;

	for(int i=0; i<getNumberOfUrls(); i++)
	{
		string url = m_urlsNotCrawled[i];
		//succesfully downloaded the webpage
		if(HTTP().get(url, page))
		{
			WordBag wb(page);
			m_index.incorporate(url, wb);
			callback(url, true);
		}
		else  //did not successfully download the apge
		{
			callback(url, false);
		}
	}
}

bool WebCrawlerImpl::save(string filenameBase)
{
    return m_index.save(filenameBase);
}

bool WebCrawlerImpl::load(string filenameBase)
{
    return m_index.load(filenameBase);  //loads a previously saved index
}

//******************** WebCrawler functions *******************************

// These functions simply delegate to WebCrawlerImpl's functions.
// You probably don't want to change any of this code.

WebCrawler::WebCrawler()
{
    m_impl = new WebCrawlerImpl;
}

WebCrawler::~WebCrawler()
{
    delete m_impl;
}

void WebCrawler::addUrl(string url)
{
    m_impl->addUrl(url);
}

int WebCrawler::getNumberOfUrls() const
{
    return m_impl->getNumberOfUrls();
}

void WebCrawler::crawl(void (*callback)(string url, bool success))
{
    m_impl->crawl(callback);
}

bool WebCrawler::save(string filenameBase)
{
    return m_impl->save(filenameBase);
}

bool WebCrawler::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}
