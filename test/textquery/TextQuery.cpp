#include "TextQuery.h"
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
using namespace std;

TextQuery::TextQuery(const string &filename)
{
	readFile(filename);
}


void TextQuery::readFile(const string &filename)
{
	ifstream in;
	in.open(filename);
	if (!in) 
		throw runtime_error("open file fail");
	string line;
	while(getline(in, line)) {
	    lines_.push_back(line);
	}
	in.close();

	for (vector<string>::size_type ix = 0; 
		 ix != lines_.size(); 
		 ++ix)
	{
		istringstream iss(lines_[ix]);
		string word;
		while(iss >> word) {
		    wordMap_[word].insert(ix);
		}
	}
}

string TextQuery::runQuery(const string &word) const
{
	map<string, set<lineNo> >::const_iterator it = wordMap_.find(word);
	if (it == wordMap_.end())
		return printResult(word, set<lineNo>());
	else
		return printResult(word, it->second);
}

string TextQuery::printResult(const string &word, const std::set<lineNo> &results) const
{
	string ret = word + "occurs " + to_string(results.size()) + " times\n";

	for (set<lineNo>::iterator it = results.begin();
		 it != results.end();
		 ++it)
	{
		ret += (string("line") + to_string(*it + 1) + ") ");
		ret += lines_[*it] + "\n";
	}

	return ret;
}
