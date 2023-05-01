#include "article.h"

string Article::getTitle() const {
	return title;
}

ostream& operator<<(ostream& os, Article a) {
	os << '"' << a.title << '"' << " by " << a.author << ":" << "\n";
	os << "\n";
	os << a.text << "\n";
	return os;
}