#include "diskdatabase.h"

#include <memory>

using std::cout;
using std::endl;

int main() {
	system("rm -r database");
	std::unique_ptr<Database> db(new DiskDatabase());

	db->addGroup("Group 1");
	db->addGroup("Group 2");
	db->addGroup("Supergroup");
	db->addGroup("Supergroup");
	// db->addGroup("Spicy BTS Gossip");
	// db->addGroup("Bland BTS Gossip");
	// db->addGroup("Jakobs hemligheter");
	// db->addGroup("Local News");
	cout << db->listGroups();
	// db->deleteGroup(1);
	// cout << db->listGroups();

	db->addArticle(1, "Jakobs bravader", "Jakob", "Här finns det inte så mycket att säga.");
	// db->addArticle(2, "Return of Jakobs bravader", "Jakob", "Här finns det inte så mycket att säga.");
	db->addArticle(1, "Isaks bravader", "Isak", "Här finns det jättemycket att säga!");
	db->addArticle(1, "Edwards bravader", "Edward", "Här finns det jättemycket att säga!");

	cout << db->listArticles(1);

	cout << db->deleteArticle(1, 1) << endl;

	db->addArticle(1, "Return of Jakobs bravader", "Jakob", "Här finns det inte så mycket att säga.");

	cout << db->listArticles(1);


	// // cout << db->listGroups() << endl;
	// cout << db->listArticles(1);

	// db->deleteArticle(1, 1);

	db->getArticle(1, 2);
}