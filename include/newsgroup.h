#include "article.h"

#include <map>
#include <iostream>

using std::string;
using std::map;
using std::pair;
using std::cout;

class NewsGroup {
public:
	NewsGroup(int groupId, string groupName): id{groupId}, name{groupName} {}

	string getName() const {
		return name;
	}

	bool addArticle(string title, string author, string text) {
		int articleId = nextId++;
		m.insert(pair<int, Article>(articleId, Article(articleId, title, author, text)));
		return true;
	}

	bool deleteArticle(int id) {
		return m.erase(id);
	}

	void list() const {
		cout <<  m.size() << " articles in " << name << ":" "\n";
		for (auto it = m.begin(); it != m.end(); ++it) {
			cout << it->first << " " << it->second.getTitle() << "\n";
		}
	}

	void printArticle(int id) const {
		cout << m.at(id);
	}

private:
	const int id;
	string name;
	map<int, Article> m;
	int nextId{1};
};