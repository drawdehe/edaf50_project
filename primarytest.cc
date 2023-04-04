#include "include/primarydatabase.h"

#include <memory>

using std::cout;

int main() {
	std::unique_ptr<Database> db(new PrimaryDatabase());
	cout << db->addGroup("Group 1") << "\n";
	cout << db->addGroup("Group 2") << "\n";
	cout << db->listGroups() << "\n";
	// cout << db->addGroup("Group 1") << "\n";
	// cout << db->listGroups() << "\n";

	cout << db->listArticles(3);


	// db->addArticle(1, "Jakobs bravader", "Jakob", "Här finns det inte så mycket att säga.");
	// db->listArticles(1);
	// db->printArticle(1, 1);
}