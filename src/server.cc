// ------------------------------------------------------------------
//
//                                 Client/Server communication package
//
//                                          Server implementation file
//
// Change log
// 950323  RH  Initial version
// 951212  RH  Modified to allow subclassing of class Connection
// 970127  RH  Added extra include to make the file compile under Linux
// 990125  PH  Changed function names: Read -> read, ...
// 000114  PH  int -> bool, virtual destructors, other minor changes
// 010129  PH  added void type to initConnection
// 011212  PH  changed char* arguments to const char*
//             changed connection closed handling to exception
//             unsigned char instead of char/int in write/read
// 020102  PH  split into separate file for each class
// 040421  PH  added namespace, new casts, cleaned up a lot
// 050113  PH  added deregisterConnection, new registration (vector),
//             added check for server shutdown, many more changes
// 130521  PH  removed namespace, shared pointers instead of raw
//             pointers, uses library algorithms
// 190211  SGR use Connection::no_socket instead of literal -1
//             added move ctor to Connection and Server
// ------------------------------------------------------------------

#include "server.h"
#include "connection.h"
#include "messagehandler.h"

#include <string>
#include <algorithm>
#include <arpa/inet.h> /* htons(), ntohs() */
#include <iostream>
#include <memory>
#include <netinet/in.h> /* sockaddr_in */
#include <sys/socket.h> /* socket(), bind(), getsockname(), listen() */
#include <sys/time.h>   /* select() */
#include <sys/types.h>  /* socket(), bind(), select() */
#include <unistd.h>     /* close(), select() */

using namespace std;

Server::Server(int port) {
        my_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (my_socket < 0) {
                my_socket = Connection::no_socket;
                return;
        }

        sockaddr_in server;
        server.sin_family      = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port        = htons(port);
        if (::bind(my_socket, reinterpret_cast<sockaddr*>(&server),
                   sizeof(server)) < 0) {
                my_socket = Connection::no_socket;
                return;
        }

        int length = sizeof(server);
        if (getsockname(my_socket, reinterpret_cast<sockaddr*>(&server),
                        reinterpret_cast<socklen_t*>(&length)) < 0) {
                my_socket = Connection::no_socket;
                return;
        }

        if (ntohs(server.sin_port) != port) {
                my_socket = Connection::no_socket;
                return;
        }

        listen(my_socket, 5);
}

Server::~Server() {
        if (my_socket != Connection::no_socket) {
                close(my_socket);
        }
        my_socket = Connection::no_socket;

}

bool Server::isReady() const { return my_socket != Connection::no_socket; }

std::shared_ptr<Connection> Server::waitForActivity() const {
        if (my_socket == Connection::no_socket) {
                error("waitForActivity: server not opened");
        }

        fd_set read_template;
        FD_ZERO(&read_template);
        FD_SET(my_socket, &read_template);
        for (const auto& conn : connections) {
                FD_SET(conn->getSocket(), &read_template);
        }

        std::shared_ptr<Connection> return_conn;
        select(FD_SETSIZE, &read_template, 0, 0, 0);
        if (FD_ISSET(my_socket, &read_template)) {
                int new_socket = accept(my_socket, 0, 0);
                if (new_socket == -1) {
                        error("waitForActivity: accept returned error");
                }
                if (pending_socket != Connection::no_socket) {
                        error("waitForActivity: a previous connection is waiting to be registered");
                }
                pending_socket = new_socket;
                return_conn    = nullptr;
        } else {
                auto it = find_if(connections.begin(), connections.end(),
                                  [&](const std::shared_ptr<Connection>& conn) {
                                          return FD_ISSET(conn->getSocket(),
                                                          &read_template);
                                  });
                if (it == connections.end()) {
                        error("waitForActivity: could not find registered connection");
                }
                return_conn = *it;
        }
        return return_conn;
}

void Server::registerConnection(const std::shared_ptr<Connection>& conn) {
        if (conn->getSocket() != Connection::no_socket) {
                error("registerConnection: connection is busy");
        }
        if (pending_socket == Connection::no_socket) {
                error("registerConnection: no client is trying tokm connect");
        }
        conn->initConnection(pending_socket);
        connections.push_back(conn);
        pending_socket = Connection::no_socket;
}

void Server::deregisterConnection(const std::shared_ptr<Connection>& conn) {
        connections.erase(
            std::remove(connections.begin(), connections.end(), conn),
            connections.end());
}

void Server::error(const char* msg) const {
        std::cerr << "Class Server::" << msg << std::endl;
        exit(1);
}

Server init(int argc, char* argv[]) {
        if (argc != 2) {
                cerr << "Usage: myserver port-number" << endl;
                exit(1);
        }

        int port = -1;
        try {
                port = std::stoi(argv[1]);
        } catch (std::exception& e) {
                cerr << "Wrong format for port number. " << e.what() << endl;
                exit(2);
        }

        Server server(port);
        if (!server.isReady()) {
                cerr << "Server initialization error." << endl;
                exit(3);
        }
        return server;
}
void Server::process_request(MessageHandler& m) {
        Protocol code = m.receive_code();

        string result = "";
        switch(code) {
                case Protocol::COM_LIST_NG: {
                        result = db->listGroups();
                        m.send_code(Protocol::ANS_LIST_NG);

                        int i = 0;
                        int nbr_newsgroups = 0;
                        while(result[i] - '0' > 0 && result[i] - '0' < 10 ) {
                                nbr_newsgroups = nbr_newsgroups*10 + (result[i] - '0');
                                i++;
                        }

                        m.send_int_parameter(nbr_newsgroups);
                        cout << "read the number " << nbr_newsgroups << endl;
                        i++;
                        int n = 0;
                        int id = 0;
                        string name = "";
                        while(n < nbr_newsgroups) {
                                while(result[i] - '0' >= 0 && result[i] - '0' < 10 ) {
                                        id = id*10 + (result[i] - '0');
                                        i++;
                                }

                                i++;
                                while(result[i] != '\n') {
                                        name.push_back(result[i]);
                                        i++;
                                }

                                m.send_int_parameter(id);
                                m.send_string_parameter(name);
                                
                                n++;
                                i++;
                                name = "";
                                id = 0;
                        }

                        m.send_code(Protocol::ANS_END);
                } break;

                case Protocol::COM_CREATE_NG: {

                        bool added = db->addGroup(m.receive_string_parameter());

                        m.send_code(Protocol::ANS_CREATE_NG);
                        //added ? m.send_code(Protocol::ANS_ACK) : m.send_code(Protocol::ERR_NG_ALREADY_EXISTS);
                        if (added) {
                                m.send_code(Protocol::ANS_ACK);
                        } else {
                                m.send_code(Protocol::ANS_NAK);
                                m.send_code(Protocol::ERR_NG_ALREADY_EXISTS);
                        }
                        m.send_code(Protocol::ANS_END);
                } break;

                case Protocol::COM_DELETE_NG: {
                        bool deleted = db->deleteGroup(m.receive_int_parameter());

                        m.send_code(Protocol::ANS_DELETE_NG);
                        //deleted ? m.send_code(Protocol::ANS_ACK) : m.send_code(Protocol::ERR_NG_DOES_NOT_EXIST);
                        if (deleted) {
                                m.send_code(Protocol::ANS_ACK);
                        } else {
                                m.send_code(Protocol::ANS_NAK);
                                m.send_code(Protocol::ERR_NG_DOES_NOT_EXIST);
                        }
                        m.send_code(Protocol::ANS_END);
                } break;
                case Protocol::COM_LIST_ART: {
                        int grp_id = m.receive_int_parameter();
                        string result = db->listArticles(grp_id);

                        m.send_code(Protocol::ANS_LIST_ART);
                        if(result == "invalid group id") {
                                m.send_code(Protocol::ANS_NAK);
                                m.send_code(Protocol::ERR_NG_DOES_NOT_EXIST);
                        } else {
                                m.send_code(Protocol::ANS_ACK);
                                
                                int i = 0;
                                int nbr_articles = 0;
                                while(result[i] - '0' >= 0 && result[i] - '0' < 10) {
                                        nbr_articles = 10*nbr_articles + (result[i] - '0');
                                        i++;
                                }

                                m.send_int_parameter(nbr_articles);
                                i++;
                                int n = 0;
                                int article_id = 0;
                                string title = "";
                                while(n < nbr_articles) {
                                        while(result[i] - '0' >= 0 && result[i] - '0' < 10) {
                                                article_id = 10*article_id + (result[i] - '0');
                                                i++;
                                        }

                                        i++;
                                        while(result[i] != '\n') {
                                                title.push_back(result[i]);
                                                i++;
                                        }

                                        m.send_int_parameter(article_id);
                                        m.send_string_parameter(title);

                                        article_id = 0;
                                        title = "";
                                        n++;
                                        i++;
                                }
                        }

                        m.send_code(Protocol::ANS_END);
                } break;
                case Protocol::COM_CREATE_ART: {
                        int group_id = m.receive_int_parameter();
                        string title = m.receive_string_parameter();
                        string author = m.receive_string_parameter();
                        string text = m.receive_string_parameter();
                        //cout << "group_id: " << group_id << endl;
                        //cout << "title: " << title << endl;
                        //cout << "author: " << author << endl;
                        //cout << "text: " << text << endl;

                        bool added = db->addArticle(group_id, title, author, text);

                        m.send_code(Protocol::ANS_CREATE_ART);
                        //added ? m.send_code(Protocol::ANS_ACK) : m.send_code(Protocol::ERR_NG_DOES_NOT_EXIST);
                        if (added) {
                                m.send_code(Protocol::ANS_ACK);
                        } else {
                                m.send_code(Protocol::ANS_NAK);
                                m.send_code(Protocol::ERR_NG_DOES_NOT_EXIST);
                        }
                        m.send_code(Protocol::ANS_END);
                } break;
                case Protocol::COM_DELETE_ART: {
                        int group_id = m.receive_int_parameter();
                        int article_id = m.receive_int_parameter();
                        //cout << "group_id: " << group_id << endl;
                        //cout << "article_id: " << article_id << endl;

                        int deleted = db->deleteArticle(group_id, article_id);
                        //cout << "deleted: " << deleted << endl;

                        m.send_code(Protocol::ANS_DELETE_ART);
                        //deleted ? m.send_code(Protocol::ANS_ACK) : m.send_code(Protocol::ANS_NAK);
                        if (deleted == 0) {
                                m.send_code(Protocol::ANS_ACK);
                        } else {
                                // if ANS_NAK, also send code depending on if ng didn't exist or if article didn't exist
                                m.send_code(Protocol::ANS_NAK);
                                if (deleted == 1) {
                                      m.send_code(Protocol::ERR_NG_DOES_NOT_EXIST);  
                                } else if (deleted == 2) {
                                      m.send_code(Protocol::ERR_ART_DOES_NOT_EXIST);
                                }
                        }
                        m.send_code(Protocol::ANS_END);
                } break;
                case Protocol::COM_GET_ART: {
                        int group_id = m.receive_int_parameter();
                        int article_id = m.receive_int_parameter();
                        //cout << "group_id: " << group_id << endl;
                        //cout << "article_id: " << article_id << endl;

                        string article = db->getArticle(group_id, article_id);
                        m.send_code(Protocol::ANS_GET_ART);

                        // if article exists, send it
                        // m.send_code(Protocol::ANS_ACK);
                        // m.send_string_parameter(title);
                        // m.send_string_parameter(author);
                        // m.send_string_parameter(text);

                        // if article didn't exist, send code depending on the reason
                        // m.send_code(Protocol::ANS_NAK);
                        // m.send_code(Protocol::ERR_NG_DOES_NOT_EXIST) or m.send_code(Protocol::ERR_ART_DOES_NOT_EXIST);

                        m.send_code(Protocol::ANS_END);
                } break;
                case Protocol::COM_END:{
                        cout << "end of message" << endl;
                } break;
                default: {
                        cout << "this should not be printed in the full version" << endl;
                } break;
        }
}



void serve_one(Server& server)
{
        auto conn = server.waitForActivity();
        if (conn != nullptr) {
                try {
                        MessageHandler m(*conn);   
                        server.process_request(m);
                } catch (ConnectionClosedException&) {
                        server.deregisterConnection(conn);
                        cout << "Client closed connection" << endl;
                }
        } else {
                conn = std::make_shared<Connection>();
                server.registerConnection(conn);
                cout << "New client connects" << endl;
        }
}



int main(int argc, char* argv[])
{
        auto server = init(argc, argv);

        while (true) {
            serve_one(server);
        }

        return 0;
}

