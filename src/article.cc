#include "article.h"

string Article::getTitle() const {
	return title;
}

string Article::getAuthor() const {
	return author;
}

string Article::getText() const {
	return text;
}

ostream& operator<<(ostream& os, Article a) {
	os << '"' << a.title << '"' << " by " << a.author << ":" << "\n";
	os << "\n";
	os << a.text << "\n";
	return os;
}