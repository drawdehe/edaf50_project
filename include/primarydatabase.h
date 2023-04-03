#include "database.h"
#include "newsgroup.h"

#include <map>
#include <iostream>

using std::string;
using std::map;
using std::pair;
using std::cout;

class PrimaryDatabase : public Database {

public:	
	bool addGroup(string name) {
		int id = nextId++;
		m.insert(pair<int, NewsGroup>(id, NewsGroup(id, name)));
		return true;
	}

	bool deleteGroup(int id) {
		return m.erase(id);
	}

	void listGroups() const {
		cout << m.size() << " newsgroups:" << "\n";
		for (const auto& kv : m) {
			cout << kv.first << " " << kv.second.getName() << "\n";
		}
	}

	bool addArticle(int groupId, string title, string author, string text) {
		m.at(groupId).addArticle(title, author, text);
		return true;
	}

	bool deleteArticle(int groupId, int articleId) {
		return m.at(groupId).deleteArticle(articleId);
	}

	void listArticles(int groupId) {
		m.at(groupId).list();
	}

	void printArticle(int groupId, int articleId) {
		m.at(groupId).printArticle(articleId);
	}

private:
	map<int, NewsGroup> m; 
	int nextId{1};
};