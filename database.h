#include <map>
#include <string>

using std::string;

class Database {
public:
	virtual ~Database() {}

	virtual bool addGroup(string name) = 0;
	virtual bool deleteGroup(int id) = 0;
	virtual void listGroups() const = 0;

	virtual bool addArticle(int groupId, string title, string author, string text) = 0;
	virtual bool deleteArticle(int groupId, int articleId) = 0;
	virtual void listArticles(int groupId) = 0;
	virtual void printArticle(int groupId, int articleId) = 0;
};