#include "SerialManager.hpp"


SerialManager::SerialManager(const char* portName, speed_t baudRate)
:
portName(portName),
baudRate(baudRate)
{
    serialPort = -1;
}

SerialManager::SerialManager(/* args */) {
    serialPort = -1;
}

SerialManager::~SerialManager() {
    closePort();
}

void SerialManager::send(uint8_t *data, int data_size) {
    if (serialPort != -1) {
        // encode
        uint8_t encode_buffer[encoding_.getEncodedBufferSize(data_size)];
        encoding_.encode(data, data_size, encode_buffer);
        // send uart
        write(serialPort, encode_buffer, sizeof(encode_buffer));
    }
    else {
        std::cerr << "Error: Serial port is not opened" << std::endl;
    }
    // Serial.write(encode_buffer, sizeof(encode_buffer));
    // USBSerial.write(encode_buffer, sizeof(encode_buffer));
}

void SerialManager::recv(uint8_t *data, int data_size) {
    int encode_buffer_size = encoding_.getEncodedBufferSize(data_size);
    uint8_t encoded_buffer[encode_buffer_size];
    if (serialPort != -1) {
        read(serialPort, encoded_buffer, encode_buffer_size);
        encoding_.decode(encoded_buffer, encode_buffer_size, data);
    }
    else {
        std::cerr << "Error: Serial port is not opened" << std::endl;
    }
    /*
    if (Serial.available() == true) {
        Serial.read(encoded_buffer, encode_buffer_size);
        encoding_.decode(encoded_buffer, encode_buffer_size, data);
    }
    */
    /*
    if (USBSerial.available() == true) {
        USBSerial.read(encoded_buffer, encode_buffer_size);
        encoding_.decode(encoded_buffer, encode_buffer_size, data);
    }
    */
}

void SerialManager::setPortName(const char *port_name)
{
    portName = port_name;
}

void SerialManager::setBaudRate(speed_t baud_rate)
{
    baudRate = baud_rate;
}

bool SerialManager::openPort() {

    std::cout << "Opening serial port " << portName << " at " << baudRate << " baud" << std::endl;

    serialPort = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);
    if (serialPort == -1) {
        std::cerr << "Error: Unable to open serial port" << std::endl;
        return false;
    }

    struct termios tty;
    if (tcgetattr(serialPort, &tty) != 0) {
        std::cerr << "Error: Unable to get serial port attributes" << std::endl;
        closePort();
        return false;
    }

    cfsetospeed(&tty, baudRate);
    cfsetispeed(&tty, baudRate);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit characters
    tty.c_cflag &= ~PARENB;                         // No parity bit
    tty.c_cflag &= ~CSTOPB;                         // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                        // No hardware flow control

    if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
        std::cerr << "Error: Unable to set serial port attributes" << std::endl;
        closePort();
        return false;
    }

    return true;
}

void SerialManager::closePort() {
    if (serialPort != -1) {
        close(serialPort);
        serialPort = -1;
    }
}