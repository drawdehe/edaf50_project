
#include "messagehandler.h"
#include "connection.h"

#include <iostream>
#include <string>
#include<limits>
#include<ios>

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::cerr;

Connection init(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: myclient host-name port-number" << endl;
        exit(1);
    }
    int port = -1;
    try {
        port = std::stoi(argv[2]);
    } catch (std::exception& e) {
        cerr << "Wrong port number. " << e.what() << endl;
        exit(2);
    }
    Connection conn(argv[1], port);
    if (!conn.isConnected()) {
        cerr << "Connection attempt failed" << endl;
        exit(3);
    }
    return conn;
}

void list_commands() {
    cout << "\nWhat do you want to do?" << endl;
    cout << "1. List newsgroups." << endl;
    cout << "2. Create a newsgroup." << endl;
    cout << "3. Delete a newsgroup." << endl;
    cout << "4. List articles in a newsgroup." << endl;
    cout << "5. Create an article." << endl;
    cout << "6. Delete an article." << endl;
    cout << "7. Get an article." << endl;
    cout << "8. End the session.\n" << endl;
}

void list_newsgroups(MessageHandler& m) {
    cout << "\n1. LIST NEWSGROUPS" << endl;
    m.send_code(Protocol::COM_LIST_NG);
    m.send_code(Protocol::COM_END);
    Protocol p = m.receive_code();
    if(p == Protocol::ANS_LIST_NG){
        int nbr_newsgroups = m.receive_int_parameter();
        cout << "There are " << nbr_newsgroups << " newsgroup(s).\n" << endl;
        cout << "Group id\t| Name" << endl;
        cout << "--------------------------------------" << endl;
        for(int i = 0; i < nbr_newsgroups; i++) {
            cout << m.receive_int_parameter() << "\t\t| " << m.receive_string_parameter() << endl;
        }
    }
    m.receive_code();
}

void create_newsgroup(MessageHandler& m) {
    cout << "\n2. CREATE NEWSGROUP" << endl;

    cout << "Enter a name for the newsgroup:" << endl;
    string name_param;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear input buffer
    std::getline(cin, name_param);
    m.send_code(Protocol::COM_CREATE_NG);
    m.send_string_parameter(name_param);
    m.send_code(Protocol::COM_END);

    Protocol p = m.receive_code();
    if (p == Protocol::ANS_CREATE_NG) {
        Protocol p2 = m.receive_code();
        if (p2 == Protocol::ANS_ACK) {
            cout << "Newsgroup " << name_param << " was created." << endl;
        } else if (p2 == Protocol::ANS_NAK) {
            Protocol p3 = m.receive_code();
            if (p3 == Protocol::ERR_NG_ALREADY_EXISTS) {
                cout << "Newsgroup " << name_param << " already exists." << endl;
            }
        }
    }

    m.receive_code();
}

void delete_newsgroup(MessageHandler& m){
    cout << "\n3. DELETE NEWSGROUP" << endl;
    cout << "Enter the identification number of the newsgroup to delete:" << endl;
    int num_p;
    cin >> num_p;
    if(!cin) {
        cout << "Not a valid input format. Needs to be an integer." << endl;
        throw ConnectionClosedException();
    }

    m.send_code(Protocol::COM_DELETE_NG);
    m.send_int_parameter(num_p);
    m.send_code(Protocol::COM_END);

    Protocol p = m.receive_code();
    if (p == Protocol::ANS_DELETE_NG) {
        Protocol p2 = m.receive_code();
        if (p2 == Protocol::ANS_ACK) {
            cout << "Newsgroup " << num_p << " was deleted." << endl;
        } else if (p2 == Protocol::ANS_NAK) {
            Protocol p3 = m.receive_code();
            if (p3 == Protocol::ERR_NG_DOES_NOT_EXIST) {
                cout << "Newsgroup " << num_p << " does not exist." << endl;
            }
        }
    }

    m.receive_code();
}

void list_articles_in_newsgroup(MessageHandler& m) {
    int id;
    cout << "\n4. LIST ARTICLES IN A NEWSGROUP \nEnter the identification number of the newsgroup:" << endl;
    cin >> id;
    if(!cin) {
        cout << "Not a valid input format. Needs to be an integer." << endl;
        throw ConnectionClosedException();
    }

    m.send_code(Protocol::COM_LIST_ART);
    m.send_int_parameter(id);
    m.send_code(Protocol::COM_END);
    Protocol p1 = m.receive_code();
    if(p1 == Protocol::ANS_LIST_ART) {
        Protocol p2 = m.receive_code();
        if(p2 == Protocol::ANS_ACK) {
            int nbr_articles = m.receive_int_parameter();
            cout << "There are " << nbr_articles << " articles in newsgroup " << id << endl;
            cout << "Article id\t| Title" << endl;
            cout << "--------------------------------------" << endl;
            for(int i = 0; i < nbr_articles; i++) {
                cout << m.receive_int_parameter() << "\t\t| " << m.receive_string_parameter() << endl;
            }
        } else {
            m.receive_code();
            cout << "Newsgroup does not exist" << endl;
        }
    }

    m.receive_code();
}

void create_article(MessageHandler& m) {
    cout << "\n5. CREATE ARTICLE" << endl;
    int num_p;
    cout << "Enter the identification number of the article's newsgroup:" << endl;
    cin >> num_p;
    if(!cin) {
        cout << "Not a valid input format. Needs to be an integer." << endl;
        throw ConnectionClosedException();
    }

    string article_title;
    cout << "Enter the title of the article:" << endl;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear input buffer
    std::getline(cin, article_title);

    string author_name;
    cout << "Enter the author of the article:" << endl;
    std::getline(cin, author_name);

    string text;
    cout << "Enter the text of the article:" << endl;
    std::getline(cin, text);

    m.send_code(Protocol::COM_CREATE_ART);
    m.send_int_parameter(num_p);
    m.send_string_parameter(article_title);
    m.send_string_parameter(author_name);
    m.send_string_parameter(text);
    m.send_code(Protocol::COM_END);

    Protocol p = m.receive_code();
    if (p == Protocol::ANS_CREATE_ART) {
        Protocol p2 = m.receive_code();
        if (p2 == Protocol::ANS_ACK) {
            cout << "Article \"" << article_title << "\" was created." << endl;
        } else if (p2 == Protocol::ANS_NAK) {
            Protocol p3 = m.receive_code();
            if (p3 == Protocol::ERR_NG_DOES_NOT_EXIST) {
                cout << "Article \"" << article_title << "\" could not be created since the newsgroup does not exist." << endl;
            }
        }
    }

    m.receive_code();
}

void delete_article(MessageHandler& m) {
    cout << "\n6. DELETE ARTICLE" << endl;
    int group_id;
    cout << "Enter the identification number of the article's newsgroup:" << endl;
    cin >> group_id;

    if(!cin) {
        cout << "Not a valid input format. Needs to be an integer." << endl;
        throw ConnectionClosedException();
    }

    int article_id;
    cout << "Enter the identification number of the article:" << endl;
    cin >> article_id;

    if(!cin) {
        cout << "Not a valid input format. Needs to be an integer." << endl;
        throw ConnectionClosedException();
    }

    m.send_code(Protocol::COM_DELETE_ART);
    m.send_int_parameter(group_id);
    m.send_int_parameter(article_id);
    m.send_code(Protocol::COM_END);

    Protocol p = m.receive_code();
    if (p == Protocol::ANS_DELETE_ART) {
        Protocol p2 = m.receive_code();
        if (p2 == Protocol::ANS_ACK) {
            cout << "Article " << article_id << " was deleted." << endl;
        } else if (p2 == Protocol::ANS_NAK) {
            Protocol p3 = m.receive_code();
            if (p3 == Protocol::ERR_NG_DOES_NOT_EXIST) {
                cout << "Newsgroup " << group_id << " does not exist." << endl;
            } else if (p3 == Protocol::ERR_ART_DOES_NOT_EXIST) {
                cout << "Article " << article_id << " does not exist." << endl;
            }
        }
    }

    m.receive_code();
}

void get_article(MessageHandler& m) {
    int group_id;
    cout << "\n7. GET ARTICLE" << endl;
    cout << "Enter the identification number of the article's newsgroup:" << endl;
    cin >> group_id;
    if(!cin) {
        cout << "Not a valid input format. Needs to be an integer." << endl;
        throw ConnectionClosedException();
    }

    int article_id;
    cout << "Enter the identification number of the article:" << endl;
    cin >> article_id;
    if(!cin) {
        cout << "Not a valid input format. Needs to be an integer." << endl;
        throw ConnectionClosedException();
    }

    m.send_code(Protocol::COM_GET_ART);
    m.send_int_parameter(group_id);
    m.send_int_parameter(article_id);
    m.send_code(Protocol::COM_END);

    Protocol p = m.receive_code();
    if (p == Protocol::ANS_GET_ART) {
        Protocol p2 = m.receive_code();
        if (p2 == Protocol::ANS_ACK) {
            cout << "\nArticle no. " << article_id << " in newsgroup no. " << group_id << endl;
            cout << "Title: " <<  m.receive_string_parameter() << endl;
            cout << "Author: " << m.receive_string_parameter() << endl;
            cout << "---\n" << m.receive_string_parameter() << endl;
        } else if (p2 == Protocol::ANS_NAK) {
            Protocol p3 = m.receive_code();
            if(p3 == Protocol::ERR_NG_DOES_NOT_EXIST) {
                cout << "Error: Newsgroup " << group_id << " does not exist." << endl;
            } else if (p3 == Protocol::ERR_ART_DOES_NOT_EXIST) {
                cout << "Error: Article " << article_id << " does not exist." << endl;
            }
        }
    }

    m.receive_code();
}

int app(const Connection& conn) {
    int nbr;
    MessageHandler m(conn);

    list_commands();

    while (cin >> nbr) {
        try {
            switch (nbr) {
                case 1:
                    list_newsgroups(m);
                    break;
                case 2:
                    create_newsgroup(m);
                    break;
                case 3:
                    delete_newsgroup(m);
                    break;
                case 4:
                    list_articles_in_newsgroup(m);
                    break;
                case 5:
                    create_article(m);
                    break;
                case 6:
                    delete_article(m);
                    break;
                case 7:
                    get_article(m);
                    break;
                case 8:
                    return 1;
                    break;
            }
            list_commands();
            //cout << "Choose a command: ";
        } catch (ConnectionClosedException&) {
            cout << "Invalid command. Disconnecting." << endl;
            // disconnect
            return 1;
        }
    }
    return 1;
}

int main(int argc, char* argv[]) {
        Connection conn = init(argc, argv);
        return app(conn);
}