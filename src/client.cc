#include "client.h"
#include "messagehandler.h"
#include "connection.h"
#include "connectionclosedexception.h"

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
    cout << "1. List newsgroups." << endl;
    cout << "2. Create a newsgroup." << endl;
    cout << "3. Delete a newsgroup." << endl;
    cout << "4. List articles in a newsgroup." << endl;
    cout << "5. Create an article." << endl;
    cout << "6. Delete an article." << endl;
    cout << "7. Get an article." << endl;
    cout << "8. End the session." << endl;
}


void receive_answer(MessageHandler& m){
    while (m.receive_code() != Protocol::ANS_END) {
        // handle the response from the server
    }
}

void list_newsgroups(MessageHandler& m) {
    m.send_code(Protocol::COM_LIST_NG);
    //m.send_code(Protocol::COM_END);
    Protocol p = m.receive_code();
    cout << "Sending request with code " << static_cast<int>(p) << endl;
    if(p == Protocol::ANS_LIST_NG){
        int nbr_newsgroups = m.receive_int_parameter();
        cout << nbr_newsgroups << endl;
        for(int i = 0; i < nbr_newsgroups; i++) {
            cout << m.receive_int_parameter() << ' ' << m.receive_string_parameter() << endl;
        }
    }
    int c = static_cast<int>(m.receive_code());
    //cout << "ended with code " << c << endl;
}

void create_newsgroup(MessageHandler& m) {
    string name_param;
    cout << "Pick a name for your new newsgroup:" << endl;
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
        } else if (p2 == Protocol::ERR_NG_ALREADY_EXISTS) {
            cout << "Newsgroup " << name_param << " already exists." << endl;
        }
    }

    int c = static_cast<int>(m.receive_code());
    cout << "ended with code " << c << endl;
}

void delete_newsgroup(MessageHandler& m){
    int num_p;
    cout << "Enter the identification number of the newsgroup to delete:" << endl;
    cin >> num_p;

    m.send_code(Protocol::COM_DELETE_NG);
    m.send_int_parameter(num_p);
    m.send_code(Protocol::COM_END);

    Protocol p = m.receive_code();
    if (p == Protocol::ANS_DELETE_NG) {
        Protocol p2 = m.receive_code();
        if (p2 == Protocol::ANS_ACK) {
            cout << "Newsgroup " << num_p << " was deleted." << endl;
        } else if (p2 == Protocol::ERR_NG_DOES_NOT_EXIST) {
            cout << "Newsgroup " << num_p << " does not exist." << endl;
        }
    }

    int c = static_cast<int>(m.receive_code());
    cout << "ended with code " << c << endl;
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
                    create_newsgroup(m);
                    break;
                case 3:
                    delete_newsgroup(m);
                    break;
                case 4:
                    //list_articles_in_a_newsgroup(m);
                    break;
                case 5:
                    //create_an_article(m);
                    break;
                case 6:
                    //delete_an_article(m);
                    break;
                case 7:
                    //get_an_article();
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