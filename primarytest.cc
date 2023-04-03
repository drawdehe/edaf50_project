#include "primarydatabase.h"

#include <memory>

int main() {
	std::unique_ptr<Database> db(new PrimaryDatabase());
	db->addGroup("Group 1");
	db->listGroups();
	db->addArticle(1, "Jakobs bravader", "Jakob", "Här finns det inte så mycket att säga.");
	db->listArticles(1);
	db->printArticle(1, 1);
}