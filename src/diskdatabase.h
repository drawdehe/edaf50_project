#include "database.h"
#include "newsgroup.h"

#include <stdlib.h>
#include <dirent.h>
#include <fstream>

#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

using std::string;
using std::map;
using std::pair;
using std::cout;
using std::endl;
using std::to_string;
using std::ofstream;
using std::ifstream;
using std::ostringstream;

class DiskDatabase : public Database {

public:	
	DiskDatabase() {
	    DIR *dir = opendir(rootName.c_str());
	    if (dir == NULL) {
	        string cmd = "mkdir " + rootName;
			system(cmd.c_str());
			cmd = "touch " + rootName + "/1.txt";
			system(cmd.c_str());
	    }
	}

	string listGroups() const {
	    DIR *dir = opendir(rootName.c_str());

	    struct dirent* entry;
		std::ostringstream os;
		readdir(dir);
		readdir(dir); 
		entry = readdir(dir); // fixa

		int count = 0;
	    while(entry != NULL) {
	    	if (!isTextFile(entry->d_name)) {
	    		os << entry->d_name << endl;
	    		++count;
	    	}
			entry = readdir(dir);
	    }
		return to_string(count) + "\n" + os.str();
	}

	bool addGroup(string groupName) {
	    DIR *dir = opendir(rootName.c_str());
	    if (dir == NULL) {
	    }
	   	struct dirent* entry;
		entry = readdir(dir);

		bool txtFound = false;
		string textFileName;

	    while(entry != NULL) {
	    	if (entry->d_name == groupName) {
	    		return false;
	    	}

	    	if (!txtFound) {
	    		if (isTextFile(entry->d_name)) {
	    			textFileName = entry->d_name;
	    			txtFound = true;
	    		}
	    	}
			entry = readdir(dir);
	    }

        int nextId = std::stoi(textFileName.substr(0, textFileName.length() - 4)); 
        // int nextId = findNextId(dir);

        string groupPath = rootName + "/" + inQuotes(to_string(nextId) + " " + groupName);
        
        runCommand("mkdir " + groupPath);
        runCommand("touch " + groupPath + inQuotes("/nextID 1.txt"));

        runCommand("rm " + rootName + "/" + textFileName);
        runCommand("touch " + rootName + "/" + to_string(nextId + 1) + ".txt");

		return true;
	}

	bool deleteGroup(int groupId) {
		string id = to_string(groupId);

		DIR *dir = opendir(rootName.c_str());
	   	struct dirent* entry;
		entry = readdir(dir);

		while (entry != NULL) {
			string name = entry->d_name;
			if (!isTextFile(name) && (name).substr(0, id.length() + 1) == id + " ") {
				runCommand("rm -r " + rootName + "/" + inQuotes(name));
				return true;
			} 
			entry = readdir(dir);
		}
		return false;
	}

	string listArticles(int groupId) {
		string groupName = getGroupName(groupId);
		string path = rootName + "/" + groupName;

		DIR *dir = opendir(path.c_str());
	    struct dirent* entry;

		std::ostringstream os;
		int count = 0;
		entry = readdir(dir);
		while (entry != NULL) {
			string name = entry->d_name;
			if (name[0] != 'n' && name[0] != '.') {
				++count;
				os << noTxt(name) << endl;
			}
			entry = readdir(dir);
		}
		return to_string(count) + "\n" + os.str();
	}

	bool addArticle(int groupId, string title, string author, string text) { 
		// DIR *dir = opendir(rootName.c_str());
	   	// struct dirent* entry;

	   	// string groupNameStart = to_string(groupId) + " ";

	   	// entry = readdir(dir);
	   	// string fName = entry->d_name; 
	   	// while (fName.substr(0, groupNameStart.length()) != groupNameStart) {
	   	// 	entry = readdir(dir);
	   	// 	fName = entry->d_name;
	   	// }
	   	// string groupName = entry->d_name;
		// string gPath = rootName + "/" + groupName;

		string groupName = getGroupName(groupId);
		string groupPath = rootName + "/" + groupName;	

		DIR *dir = opendir(groupPath.c_str());
        int nextId = findNextId(dir);
        runCommand("rm " + rootName + "/" + inQuotes(groupName) + "/" + inQuotes("nextID " + to_string(nextId)) + ".txt");
        runCommand("touch " + rootName + "/" + inQuotes(groupName) + "/" + inQuotes("nextID " + to_string(nextId + 1)) + ".txt");

       	ofstream outfile(groupPath + "/" + to_string(nextId) + " " + title + ".txt");
		outfile << author << endl;
		outfile << text;
		outfile.close();
		return true;

		// string path = groupPath(groupId);

		// string infoPath = path + "info.txt";
	    // ifstream infile(infoPath);
	    // string groupName;
	    // int articleId;
	    // std::getline(infile, groupName);
	    // infile >> articleId;
	    // infile.close();

	    // ofstream outfile(path + to_string(articleId) + ".txt");
    	// // if (!outfile.is_open()) {
        // // 	return false;
	    // // }
	    // outfile << '"' << title << '"' << " by " << author << ":" << "\n";
		// outfile << "\n";
		// outfile << text << "\n";
		// outfile.close();
		// // closedir(dir);

	    // ofstream outfile_info(infoPath, std::ofstream::trunc);
	    // outfile_info << groupName << endl;
	    // outfile_info << to_string(++articleId) << endl;
        
		// return true;
	}

	int deleteArticle(int groupId, int articleId) {
		string groupName = getGroupName(groupId);
		string articleName = getArticleName(groupId, articleId);

		string cmd = "rm " + rootName + "/" + inQuotes(groupName) + "/" + inQuotes(articleName) + ".txt";
		return !system(cmd.c_str());
	}

	string getArticle(int groupId, int articleId) {
		string title;
		string author;
		string text;

		string groupName = getGroupName(groupId);
		string articleName = getArticleName(groupId, articleId);
		int idLength = to_string(articleId).length();
		title = articleName.substr(idLength, articleName.length() - idLength);
		ifstream infile(rootName + "/" + groupName + "/" + articleName + ".txt");
		getline(infile, author);
		getline(infile, text);

		ostringstream os;
		os << '"' << title << '"' << " by " << author << ":" << "\n";
		os << "\n";
		os << text << "\n";

		return os.str();
	}

private:
	string rootName{"database"};
	int nextGroupId{1};

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

	void runCommand(string cmd) {
		system(cmd.c_str());
	}

	string getName(DIR* dir, int id) {
		string groupNameStart = to_string(id) + " ";
	   	struct dirent* entry = readdir(dir);

	   	string fName = entry->d_name; 
	   	while (fName.substr(0, groupNameStart.length()) != groupNameStart) {
	   		entry = readdir(dir);
	   		fName = entry->d_name;
	   	}
	   	string groupName = entry->d_name;
		return groupName;
	}

	string getGroupName(int groupId) {
		DIR *dir = opendir(rootName.c_str());

		return getName(dir, groupId);
		// string groupNameStart = to_string(groupId) + " ";
		// DIR *dir = opendir(rootName.c_str());
	   	// struct dirent* entry = readdir(dir);

	   	// string fName = entry->d_name; 
	   	// while (fName.substr(0, groupNameStart.length()) != groupNameStart) {
	   	// 	entry = readdir(dir);
	   	// 	fName = entry->d_name;
	   	// }
	   	// string groupName = entry->d_name;
		// return groupName;
	}

	string getArticleName(int groupId, int articleId) {
		string groupName = getGroupName(groupId);
		string path = rootName + "/" + groupName;
		DIR* dir = opendir(path.c_str());
		string name = getName(dir, articleId);
		return name.substr(0, name.length() - 4);

		// struct dirent* entry = readdir(dir)

	   	// string fName = entry->d_name; 
	   	// while (fName.substr(0, groupNameStart.length()) != groupNameStart) {
	   	// 	entry = readdir(dir);
	   	// 	fName = entry->d_name;
	   	// }
	   	// string groupName = entry->d_name;
		// return groupName;

	}

	int findNextId(DIR* dir) {
	    struct dirent* entry = readdir(dir);
	    string fName = entry->d_name;
	    while (fName[0] != 'n') {
	    	entry = readdir(dir);
	    	fName = entry->d_name;
	    }
	    return std::stoi(fName.substr(7, fName.length() - 7)); // Name format: nextID x
	}
};