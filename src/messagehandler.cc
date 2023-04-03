#include "messagehandler.h"


MessageHandler::MessageHandler(Connection t_conn) : conn(std::move(t_conn)) {}

void MessageHandler::send_byte(int code) {
    try {
        conn.write(code);
    } catch (struct ConnectionCloseException()) {
        throw ConnectionClosedException();
    }
} 

void MessageHandler::send_code(Protocol code) {
    send_byte(static_cast<int>(code));
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
}

int MessageHandler::receive_byte() {
    int code = conn.read();
    if (code == Connection.CONNECTION_CLOSED) { // fix this
        throw ConnectionClosedException();
    }
    return code;
}

Protocol MessageHandler::receive_code() {
    int code = receive_byte();
    return code;
}

int MessageHandler::receive_int() {
    int b1 = receive_byte();
    int b2 = receive_byte();
    int b3 = receive_byte();
    int b4 = receive_byte();

    return b1 << 24 | b2 << 16 | b3 << 8 | b4;
}

int MessageHandler::receive_int_parameter() {
    int code = receive_code();
    if (code != Protocol::PAR_NUM) {
        throw ConnectionClosedException();
    }
    return receive_int();
}

std::string MessageHandler::receive_string_parameter() {
    int code = receive_code();
    if (code != Protocol::PAR_STRING) {
        throw ConnectionClosedException();
    }
    int n = receive_int();
    if (n < 0) {
        throw ConnectionClosedException();
    }
    std::string result;
    for (unsigned int i = 0; i <= n; i++) {
        unsigned char ch = conn.read();
        result.push_back(ch);
    }
    return result;
}