#include "newsgroup.h"

int main() {
	NewsGroup n = NewsGroup(1, "Group 1");
	n.addArticle("Jakobs bravader", "Jakob", "Här finns det inte så mycket att säga.");
	n.addArticle("Sigges Story", "Sigmund", "lorem");
	n.list();

	n.deleteArticle(1);
	n.list();


}