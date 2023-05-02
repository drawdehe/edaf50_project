#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include "article.h"

#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <array>

using std::string;
using std::map;
using std::pair;
using std::cout;
using std::array;

class NewsGroup {
public:
	NewsGroup(int groupId, string groupName): id{groupId}, name{groupName} {}

	string getName() const {
		return name;
	}

	bool addArticle(string title, string author, string text) {
		if (std::find_if(m.begin(), m.end(), [title](pair<int, Article> kv) { return kv.second.getTitle() == title; }) == m.end()) {
			int articleId = nextId++;
			m.insert(pair<int, Article>(articleId, Article(articleId, title, author, text)));
			return true;
		}
		return false;
	}

	void addArticleWithId(int articleId, string title, string author, string text) {
		m.insert(pair<int, Article>(articleId, Article(articleId, title, author, text)));
	}

	// bool addArticle(int string title, string author, string text) {
	// 	if (std::find_if(m.begin(), m.end(), [title](pair<int, Article> kv) { return kv.second.getTitle() == title; }) == m.end()) {
	// 		m.insert(pair<int, Article>(articleId, Article(articleId, title, author, text)));
	// 		return true;
	// 	}
	// 	return false;
	// }

	bool deleteArticle(int id) {
		return m.erase(id);
	}

	string listArticles() const {
		std::ostringstream os;
		os <<  m.size() << "\n";
		for (auto it = m.begin(); it != m.end(); ++it) {
			os << it->first << " " << it->second.getTitle() << "\n";
		}
		return os.str();
	}

	// string getArticle(int id) const {
	// 	std::ostringstream os;
	// 	os << m.at(id);
	// 	return os.str();
	// }

	array<string, 3> getArticle(int id) const {
		// try {
		// 	Article art = m.at(id);
		// 	array<string, 3> a = {art.getTitle(), art.getAuthor(), art.getText()};
		// 	return a;
		// } catch (const std::out_of_range& e) {
		// 	// string s = "article not found";
		// 	// throw s;

		// 	throw std::runtime_error("article not found");
		// }

		if (m.count(id) > 0) {
			Article art = m.at(id);
			array<string, 3> a = {art.getTitle(), art.getAuthor(), art.getText()};
			return a;
		} else {
			throw std::runtime_error("article not found");
		}
	}

	int getNextId() {
		return nextId;
	}

	void setNextId(int id) {
		nextId = id;
	}


private:
	const int id;
	string name;
	map<int, Article> m;
	int nextId{1};
};

#endif