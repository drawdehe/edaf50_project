#ifndef PRIMARYDB_H
#define PRIMARYDB_H

#include "database.h"
#include "newsgroup.h"

#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>

using std::string;
using std::map;
using std::pair;
using std::cout;
using std::endl;

class PrimaryDatabase : public Database {

public:	
	PrimaryDatabase() {}

	string listGroups() const {
		std::ostringstream os;
		os << m.size() << "\n";
		for (const auto& kv : m) {
			os << kv.first << " " << kv.second.getName() << "\n";
		}
		return os.str();
	}

	bool addGroup(string groupName) {
		if (std::find_if(m.begin(), m.end(), [groupName](pair<int, NewsGroup> kv) { return kv.second.getName() == groupName; }) != m.end()) {
			return false;
		}

		int id = nextId++;
		m.insert(pair<int, NewsGroup>(id, NewsGroup(id, groupName)));
		return true;
	}

	bool deleteGroup(int groupId) {
		return m.erase(groupId);
	}

	string listArticles(int groupId) const {
		try {
			return m.at(groupId).listArticles();
		} catch (const std::out_of_range& e) {
			return "invalid group id";
		}
	}

	bool addArticle(int groupId, string title, string author, string text) { 
		auto it = std::find_if(m.begin(), m.end(), [groupId](pair<int, NewsGroup> kv) { return kv.first == groupId; });

		if (it == m.end()) {
			return false; 
		}

		try {
			(*it).second.addArticle(title, author, text);	
		} catch (const std::out_of_range& e) {
			return false; 
		}
		
		return true;
	}

	// return 0 if it was deleted
	// return 1 if the newsgroup didn't exist
	// return 2 if the article didn't exist
	int deleteArticle(int groupId, int articleId) {
    	auto it = m.find(groupId);
    	if (it == m.end()) {
        	return 1;
    	}
    	NewsGroup& ng = it->second;
    	if (!ng.deleteArticle(articleId)) {
        	return 2;
   		}
    	return 0;
	}

	array<string, 3> getArticle(int groupId, int articleId) const {
		if (m.count(groupId) > 0) {
			try {
				return m.at(groupId).getArticle(articleId);
			} catch (const std::runtime_error& e) {
				throw ArticleDoesNotExistException("Error: no article with that ID exists in this newsgroup.");
			}
 		} else {
 			throw NewsgroupDoesNotExistException("Error: the specified newsgroup does not exist.");
 		}
	}

private:
	map<int, NewsGroup> m; 
	int nextId{1};
};

#endif