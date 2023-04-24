#include "article.h"

ostream& operator<<(ostream& os, Article a) {
	os << '"' << a.title << '"' << " by " << a.author << ":" << "\n";
	os << "\n";
	os << a.text << "\n";
	return os;
}