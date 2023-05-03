#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <string>
#include <array>

#include "customexception.h"

using std::string;
using std::array;

class Database {
public:
	virtual ~Database() {}

	virtual string listGroups() const = 0;
	virtual bool addGroup(string groupName) = 0;
	virtual bool deleteGroup(int groupId) = 0;

	virtual string listArticles(int groupId) = 0;
	virtual bool addArticle(int groupId, string title, string author, string text) = 0;
	virtual int deleteArticle(int groupId, int articleId) = 0;
	virtual array<string, 3> getArticle(int groupId, int articleId) = 0;
	// virtual string getArticle(int groupId, int articleId) = 0;
};

#endif