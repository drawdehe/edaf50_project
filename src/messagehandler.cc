#include "messagehandler.h"

#include <iostream>
#include <memory>

using namespace std;

// MessageHandler::MessageHandler(const Connection& t_conn) : conn(std::move(const_cast<Connection&>(t_conn))) {}
MessageHandler::MessageHandler(const Connection& t_conn) : conn(t_conn) {}

void MessageHandler::send_byte(unsigned char bt) {
    try {
        conn.write(bt);
    } catch (struct ConnectionCloseException()) {
        throw ConnectionClosedException();
    }
} 

void MessageHandler::send_code(Protocol code) {
    send_int(static_cast<int>(code));
}

void MessageHandler::send_int(int value) {
    send_byte((value >> 24) & 0xFF);
    send_byte((value >> 16) & 0xFF);
    send_byte((value >> 8) & 0xFF);
    send_byte(value & 0xFF);
}

void MessageHandler::send_int_parameter(int param) {
    send_code(Protocol::PAR_NUM);
    send_int(param);
}

void MessageHandler::send_string_parameter(std::string param) {
    send_code(Protocol::PAR_STRING);
    send_int(param.length());
    for (unsigned int i = 0; i < param.length(); i++) {
        send_byte(param.at(i));
    }
    std::cout << "sent string parameter" << std::endl;
}

int MessageHandler::receive_byte() {
    if (!conn.isConnected()) {
        throw ConnectionClosedException();
    }
    int code = conn.read();
    std::cout << "read byte " << code << endl;
    return code;
}

Protocol MessageHandler::receive_code() {
    cout <<  "receiving code " <<endl;
    return static_cast<Protocol>(receive_int());
}

int MessageHandler::receive_int() {
    int b1 = receive_byte();
    int b2 = receive_byte();
    int b3 = receive_byte();
    int b4 = receive_byte();

    return b1 << 24 | b2 << 16 | b3 << 8 | b4;
}

int MessageHandler::receive_int_parameter() {
    Protocol code = receive_code();
    if (code != Protocol::PAR_NUM) {
        throw ConnectionClosedException();
    }

    return receive_int();
}

std::string MessageHandler::receive_string_parameter() {
    Protocol code = receive_code();
    if (code != Protocol::PAR_STRING) {
        throw ConnectionClosedException();
    }

    int n = receive_int();
    if (n < 0) {
        throw ConnectionClosedException();
    }

    std::string result;
    for (unsigned int i = 0; i < n; i++) { // <=
        unsigned char ch = conn.read();
        result.push_back(ch);
    }
    return result;
}