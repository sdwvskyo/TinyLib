#ifndef TEXT_QUERY_H
#define TEXT_QUERY_H

#include <string>
#include <set>
#include <map>
#include <vector>

class TextQuery
{
public:
	typedef std::vector<std::string>::size_type lineNo;
	
	TextQuery(const std::string &filename);

	void readFile(const std::string &filename);
	std::string runQuery(const std::string &word) const;

private:
	std::string printResult(const std::string &, const std::set<lineNo> &) const;

	std::vector<std::string> lines_;
	std::map<std::string, std::set<lineNo>> wordMap_;
};

#endif