#include "primarydatabase.h"

#include <memory>
#include <array>
#include <string>
#include <algorithm>

using std::cout;
using std::endl;
using std::string;
using std::array;

int main() {
	std::unique_ptr<Database> db(new PrimaryDatabase());
	db->addGroup("Group 1");
	// db->listGroups();
	db->addArticle(1, "Jakobs bravader", "Jakob", "Här finns det inte så mycket att säga.");
	// db->listArticles(1);
	// array<string, 3> a = db->getArticle(1, 1);
	// std::for_each(a.begin(), a.end(), [](string s) {cout << s << endl;});

	// array<string, 3> a = db->getArticle(2, 1);
	try {
		// array<string, 3> a = db->getArticle(1, 2);
		array<string, 3> a = db->getArticle(2, 2);

	} catch (const std::runtime_error& e) {
		cout << e.what() << endl;
	}

}