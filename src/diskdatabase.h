#include "database.h"
#include "newsgroup.h"

#include <stdlib.h>
#include <dirent.h>
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

class DiskDatabase : public Database {

public:	
	DiskDatabase() {
	    DIR *dir = opendir(rootName.c_str());	
	    if (dir == NULL) {
	        runCommand("mkdir " + rootName);
			runCommand("mkdir " + rootName + "/" + inQuotes(".nextid 1"));
	    }
	}

	string listGroups() const {
	    DIR *dir = opendir(rootName.c_str());
	    struct dirent* entry;
		std::ostringstream os;
		skipFiles(dir, 3);

		int count = 0;
		entry = readdir(dir);
	    while (entry != NULL) {
    		++count;

	    	string groupId = entry->d_name;
	    	DIR *innerDir = opendir((rootName + "/" + groupId).c_str());

	    	struct dirent* innerEntry;
	    	skipFiles(innerDir, 2);
	    	innerEntry = readdir(innerDir);
	    	string groupName = extractName(innerEntry->d_name);
    		os << groupId << " " << groupName << endl;

			entry = readdir(dir);
	    }
		return to_string(count) + "\n" + os.str();
	}

	bool addGroup(string groupName) {
	    DIR *dir = opendir(rootName.c_str());
        struct dirent* entry;
	    skipFiles(dir, 2);
        entry = readdir(dir);
        int nextId = extractId(entry->d_name);

	   	entry = readdir(dir);
        while (entry != NULL) {
        	if (entry->d_name == groupName) {
        		return false;
        	}
	   		entry = readdir(dir);
        }

        string groupPath = rootName + "/" + to_string(nextId);
        
        runCommand("mkdir " + groupPath);
        runCommand("mkdir " + groupPath + "/" + inQuotes(".nextid 1"));
        runCommand("mkdir " + groupPath + "/" + inQuotes(".name " + groupName));

        runCommand("rm -r " + rootName + "/" + inQuotes(".nextid " + to_string(nextId)));
        runCommand("mkdir " + rootName + "/" + inQuotes(".nextid " + to_string(nextId + 1)));

		return true;
	}

	bool deleteGroup(int groupId) {
		string id = to_string(groupId);
		DIR *dir = opendir(rootName.c_str());
	   	struct dirent* entry;
		entry = readdir(dir);

		while (entry != NULL) {
			if (entry->d_name == id) {
				runCommand("rm -r " + rootName + "/" + id);
				return true;
			}
			entry = readdir(dir);
		}
		return false;
	}

	string listArticles(int groupId) {
		string groupPath = rootName + "/" + to_string(groupId);
		DIR *dir = opendir(groupPath.c_str());
	    struct dirent* entry;
	    skipFiles(dir, 4);

		int count = 0;
		std::ostringstream os;
		entry = readdir(dir);
		while (entry != NULL) {
			++count;
			string fName = entry->d_name;
			string articleId = fName.substr(0, fName.length() - 4);
			ifstream infile(groupPath + "/" + fName);
			string title;
			getline(infile, title);
			os << articleId << " " << title << endl;

			entry = readdir(dir);
		}
		return to_string(count) + "\n" + os.str();
	}

	bool addArticle(int groupId, string title, string author, string text) { 
		string groupPath = rootName + "/" + to_string(groupId);
		DIR *dir = opendir(groupPath.c_str());
		if (dir == NULL) {
			return false;
		}

		struct dirent* entry;
		skipFiles(dir, 3);
		entry = readdir(dir);
        int nextId = extractId(entry->d_name);

       	ofstream outfile(groupPath + "/" + to_string(nextId) + ".txt");
       	outfile << title << endl;
		outfile << author << endl;
		outfile << text;
		outfile.close();

        runCommand("rm -r " + groupPath + "/" + inQuotes(".nextId " + to_string(nextId)));
        runCommand("mkdir " + groupPath + "/" + inQuotes(".nextId " + to_string(nextId + 1)));

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

	array<string, 3> getArticle(int groupId, int articleId) {
		DIR *dir = opendir(groupPath.c_str());
		if (dir == NULL) {
			return 1;
		}

		string title;
		string author;
		string text;

		ifstream infile(rootName + "/" + to_string(groupId) + "/" + to_string(articleId) + ".txt");
		if (!infile) {
			throw;
		}

		getline(infile, title);
		getline(infile, author);
		getline(infile, text);
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

	string noTxt(string fileName) {
		return fileName.substr(0, fileName.length() - 4);
	}

	// string groupPath(int groupId) {
	// 	return rootName + "/" + to_string(groupId) + "/";
	// }

	bool isTextFile(string fileName) const {
		int nameLength = fileName.length(); 
		return (nameLength > 4 && fileName.substr(nameLength - 4, 4) == ".txt");
	}

	int runCommand(string cmd) {
		return system(cmd.c_str());
	}

	// string getName(DIR* dir, int id) {
	// 	string groupNameStart = to_string(id) + " ";
	//    	struct dirent* entry = readdir(dir);

	//    	string fName = entry->d_name; 
	//    	while (fName.substr(0, groupNameStart.length()) != groupNameStart) {
	//    		entry = readdir(dir);
	//    		fName = entry->d_name;
	//    	}
	//    	string groupName = entry->d_name;
	// 	return groupName;
	// }

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
};