#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <string>

#include "protocol.h"
#include "connection.h"
#include "connectionclosedexception.h"

class MessageHandler {
public:
    MessageHandler(Connection conn);
    void send_code(Protocol code);
    void send_int(int value);
    void send_int_parameter(int param);
    void send_string_parameter(std::string param);
    Protocol receive_code();
    int receive_int();
    int receive_int_parameter();
    std::string receive_string_parameter();
private:
    const Connection conn;
    void send_byte(int code);
    int receive_byte();
};

#endif