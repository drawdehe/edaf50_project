#include "primarydatabase.h"

#include <memory>

using std::cout;

int main() {
	std::unique_ptr<Database> db(new PrimaryDatabase());
	cout << db->addGroup("Group 1") << "\n";
	// db->listGroups();
	// db->addArticle(1, "Jakobs bravader", "Jakob", "Här finns det inte så mycket att säga.");
	// db->listArticles(1);
	// db->printArticle(1, 1);
}