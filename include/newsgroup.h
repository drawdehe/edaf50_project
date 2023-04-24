#include "article.h"

#include <map>
#include <iostream>
#include <sstream>

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
		if (std::find_if(m.begin(), m.end(), [title](pair<int, Article> kv) { return kv.second.getTitle() == title; }) == m.end()) {
			return false;
		}

		int articleId = nextId++;
		m.insert(pair<int, Article>(articleId, Article(articleId, title, author, text)));
		return true;
	}

	bool deleteArticle(int id) {
		return m.erase(id);
	}

	string listArticles() const {
		std::ostringstream os;
		os <<  m.size() << " articles in " << name << ":" "\n";
		for (auto it = m.begin(); it != m.end(); ++it) {
			os << it->first << " " << it->second.getTitle() << "\n";
		}
		return os.str();
	}

	string getArticle(int id) const {
		std::ostringstream os;
		os << m.at(id);
		return os.str();
	}

private:
	const int id;
	string name;
	map<int, Article> m;
	int nextId{1};
};