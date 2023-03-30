#include "server.h"

Server::Server(int port) {

}

Server::~Server() {

}

bool Server::isReady() const {

}

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
        error("registerConnection: no client is trying to connect");
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