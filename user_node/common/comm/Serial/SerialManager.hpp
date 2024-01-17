#ifndef SERIAL_MANAGER_HPP
#define SERIAL_MANAGER_HPP

#include "encoding.hpp"

#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>

class SerialManager
{
private:
    /* data */
    EncodingCobs encoding_;
    const char* portName;
    int serialPort;
    speed_t baudRate;
public:
    SerialManager(/* args */);
    SerialManager(const char* portName, speed_t baudRate);
    ~SerialManager();
    bool openPort();
    void closePort();
    void send(uint8_t *data, int data_size);
    void recv(uint8_t *data, int data_size);
    void setPortName(const char* port_name);
    void setBaudRate(speed_t baud_rate);
};


#endif