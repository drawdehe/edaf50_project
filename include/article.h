#include <iostream>

using std::string;
using std::ostream;

class Article {
public:
	Article(int idNbr, string tit = "", string auth = "", string txt = ""): id{idNbr}, title{tit}, author{auth}, text{txt} {}

	string getTitle() const {
		return title;
	}

	friend ostream& operator<<(ostream& os, Article a);

private:
	const int id;
	string title;
	string author;
	string text;
};

ostream& operator<<(ostream& os, Article a) {
	os << '"' << a.title << '"' << " by " << a.author << ":" << "\n";
	os << "\n";
	os << a.text << "\n";
	return os;
}