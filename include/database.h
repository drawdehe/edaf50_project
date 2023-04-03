#include <map>
#include <string>

using std::string;

class Database {
public:
	virtual ~Database();

	virtual bool addGroup(string name);
	virtual bool deleteGroup(int id);
	virtual void listGroups() const;

	virtual bool addArticle(int groupName, string title, string author, string text);	
	virtual bool deleteArticle(int groupId, int articleId);
	virtual void listArticles(int groupId);
};