#include "connection.h"
#include "connectionclosedexception.h"
#include "messagehandler.h"

#include <iostream>
#include <string>

using std::string;
using std::cin;
using std::cout;
using std::endl;

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
    cout << "1. List newsgroups." << endl;
    cout << "2. Create a newsgroup." << endl;
    cout << "3. Delete a newsgroup." << endl;
    cout << "4. List articles in a newsgroup." << endl;
    cout << "5. Create an article." << endl;
    cout << "6. Delete an article." << endl;
    cout << "7. Get an article." << endl;
    cout << "8. End the session." << endl;
}

void list_newsgroups(MessageHandler m) {
    m.send_code(Protocol::COM_LIST_NG);
    m.send_code(Protocol::COM_END);
    while (m.receive_code != Protocol::ANS_END) {
        // handle the response from the server
    }
}

int app(const Connection& conn) {
    cout << "Choose a command: " << endl;
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
                    create_a_newsgroup(m);
                    break;
                case 3:
                    delete_a_newsgroup(m);
                    break;
                case 4:
                    list_articles_in_a_newsgroup(m);
                    break;
                case 5:
                    create_an_article(m);
                    break;
                case 6:
                    delete_an_article(m);
                    break;
                case 7:
                    get_an_article();
                    break;
                case 8:
                    return 1;
                    break;
            }
            cout << "Choose a command: " << endl;
            list_commands();
        } catch (ConnectionClosedException&) {
            cout << "Invalid command. Disconnecting." << endl;
            // disconnect
            return 1;
        }
    }
}

int main(int argc, char* argv[]) {
        Connection conn = init(argc, argv);
        return app(conn);
}