#include "database.h"
#include "newsgroup.h"

#include <stdlib.h>
#include <dirent.h>
#include <cctype>
#include <fstream>
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <array>

using std::string;
using std::map;
using std::pair;
using std::cout;
using std::endl;
using std::to_string;
using std::ofstream;
using std::ifstream;
using std::ostringstream;
using std::array;
using std::stoi;

class DiskDatabase : public Database {

public:	
	DiskDatabase() {
	    DIR *dir = opendir(rootName.c_str());	
	    if (dir == NULL) {
	        runCommand("mkdir " + rootName);
			// runCommand("touch " + rootName + "/" + inQuotes(".nextid 1"));
	        ofstream outfile(rootName + "/.nextid.txt");
       		outfile << 1 << endl;
			outfile.close();
	    }
	}

	string listGroups() const {
	    DIR *dir = opendir(rootName.c_str());
	    struct dirent* entry;

		map<int, string> m;

		int count = 0;
		entry = readdir(dir);
	    while (entry != NULL) {
	    	if (!periodFile(entry->d_name)) {
    			++count;
	    		int groupId = stoi(entry->d_name);
	    		string groupName = getContents(rootName + "/" + to_string(groupId) + "/.name.txt");
	    		m.insert(pair<int, string>(groupId, groupName));
	    	}
			entry = readdir(dir);
	    }

		std::ostringstream os;
	    std::for_each(m.begin(), m.end(), [&os](pair<int, string> kv) { 
	    	os << kv.first << " " << kv.second << "\n";
	    });

		return to_string(count) + "\n" + os.str();
	}

	bool addGroup(string groupName) {
	    DIR *dir = opendir(rootName.c_str());
        struct dirent* entry;

	   	entry = firstItemFile(dir);
        while (entry != NULL) {
        	string name = getContents(rootName + "/" + entry->d_name + "/.name.txt");
        	if (name == groupName) {
        		return false;
        	}	
	   		entry = readdir(dir);
        }

        string nextId = getContents(rootName + "/.nextid.txt");

        string groupPath = rootName + "/" + nextId;

        runCommand("mkdir " + groupPath);
        setContents(groupPath + "/.nextid.txt", to_string(1));
        setContents(groupPath + "/.name.txt", groupName);

        runCommand("rm " + rootName + "/.nextid.txt");
        setContents(rootName + "/.nextid.txt", to_string(std::stoi(nextId) + 1));
		return true;
	}

	bool deleteGroup(int groupId) {
		string path = rootName + "/" + to_string(groupId);

		DIR *dir = opendir(path.c_str());
		if (dir == NULL) {
			return false;
		}

		runCommand("rm -r " + path);
		return true;
	}

	string listArticles(int groupId) const {
		string groupPath = rootName + "/" + to_string(groupId);
		DIR *dir = opendir(groupPath.c_str());
		if (dir == NULL) {
			return "invalid group id";
		}
		struct dirent* entry;

		int count = 0;
		map<int, string> m;
		entry = readdir(dir);
		while (entry != NULL) {
			if (!periodFile(entry->d_name)) {
				++count;
				string fName = entry->d_name;
				int articleId = stoi(fName.substr(0, fName.length() - 4));
				ifstream infile(groupPath + "/" + fName);
				string title;
				getline(infile, title);
				m.insert(pair<int, string>(articleId, title));
			}
			entry = readdir(dir);
		}

		std::ostringstream os;
	    std::for_each(m.begin(), m.end(), [&os](pair<int, string> kv) { 
	    	os << kv.first << " " << kv.second << "\n";
	    });

		return to_string(count) + "\n" + os.str();
	}

	bool addArticle(int groupId, string title, string author, string text) { 
		string groupPath = rootName + "/" + to_string(groupId);
		DIR *dir = opendir(groupPath.c_str());
		if (dir == NULL) {
			return false;
		}

    	string nextId = getContents(groupPath + "/.nextid.txt");

       	ofstream outfile(groupPath + "/" + nextId + ".txt");
       	outfile << title << endl;
		outfile << author << endl;
		outfile << text;
		outfile.close();

        runCommand("rm " + groupPath + "/.nextid.txt");
        setContents(groupPath + "/.nextid.txt", to_string(stoi(nextId) + 1));

		return true;
	}

	int deleteArticle(int groupId, int articleId) {
		string groupPath = rootName + "/" + to_string(groupId);
		DIR *dir = opendir(groupPath.c_str());
		if (dir == NULL) {
			return 1;
		}

		string articlePath = groupPath + "/" + to_string(articleId) + ".txt";

		ifstream infile(articlePath);
		if (!infile) {
			return 2;
		}

		runCommand("rm " + articlePath);
		return 0;
	}

	array<string, 3> getArticle(int groupId, int articleId) const {
		string groupPath = rootName + "/" + to_string(groupId);
		DIR *dir = opendir(groupPath.c_str());
		if (dir == NULL) {
 			throw NewsgroupDoesNotExistException("Error: the specified newsgroup does not exist.");
		}

		string title;
		string author;
		string text;

		ifstream infile(rootName + "/" + to_string(groupId) + "/" + to_string(articleId) + ".txt");
		if (!infile) {
			throw ArticleDoesNotExistException("Error: no article with that ID exists in this newsgroup.");	
		}

		getline(infile, title);
		getline(infile, author);
		//getline(infile, text);
		string s;
		while (getline(infile, s)) {
			text += s;
			if (!infile.eof()) {
				text += '\n';
			}
		}
		array<string, 3> a = {title, author, text};
		return a;
	}

private:
	string rootName{"database"};

	// const string NEXTID_PREAMBLE = ".nextid ";
	// const string NAME_PREAMBLE = ".name ";
	// const int NEXTID_PREAMBLE_LENGTH = NAME_PREAMBLE.length();
	// const int NAME_PREAMBLE_LENGTH = NEXTID_PREAMBLE.length();

	const int NEXTID_PREAMBLE_LENGTH = 8;
	const int NAME_PREAMBLE_LENGTH = 6;

	string inQuotes(string s) {
		return "\"" + s + "\"";
	}

	int runCommand(string cmd) {
		return system(cmd.c_str());
	}

	int extractId(string fName) const {
	    return std::stoi(fName.substr(NEXTID_PREAMBLE_LENGTH, fName.length() - NEXTID_PREAMBLE_LENGTH));
	}

	string extractName(string fName) const {
	    return fName.substr(NAME_PREAMBLE_LENGTH, fName.length() - NAME_PREAMBLE_LENGTH);
	}

	void skipFiles(DIR* dir, int nbr) const {
		for (int i = 0; i < nbr; ++i) {
			readdir(dir);
		}
	}

	string getContents(string path) const {
		ifstream infile(path);
		string s;
		getline(infile, s);
		return s;
	}

	void setContents(string path, string contents) const {
		ofstream outfile(path);
		outfile << contents << endl;
		outfile.close();
	}

	bool periodFile(string name) const {
		return name[0] == '.';
	}

	struct dirent* firstItemFile(DIR* dir) const {
		struct dirent* entry;
		entry = readdir(dir);
		if (entry == NULL) {
			return NULL;
		}
		while (entry != NULL && !std::isdigit((entry->d_name)[0])) { // Kolla och returnera null
			entry = readdir(dir);
		}
		return entry;
	}

	struct dirent* nextIdFile(DIR* dir) const {
		struct dirent* entry;
		entry = readdir(dir);
		if (entry == NULL) {
			return NULL;
		}

		string name = entry->d_name;
		while (entry != NULL && name.substr(0, 3) != ".ne") {
			entry = readdir(dir);
			name = entry->d_name;
		}
		return entry;
	}

	struct dirent* nameFile(DIR* dir) const {
		struct dirent* entry;
		entry = readdir(dir);
		if (entry == NULL) {
			return NULL;
		}

		string name = entry->d_name;
		while (entry != NULL && name.substr(0, 3) != ".na") {
			entry = readdir(dir);
			name = entry->d_name;
		}
		return entry;
	}

	void print(string s) {
		cout << s << endl;
	}
};