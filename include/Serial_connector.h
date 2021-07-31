#ifndef SERIAL_CONNECTOR_H
#define SERIAL_CONNECTOR_H

#include <iostream>
#include <termios.h>
#include <fcntl.h>

namespace cub
{

class Serial_connector
{
public:
    explicit Serial_connector();
    explicit Serial_connector(std::string prt="");
    Serial_connector(const Serial_connector&) = delete;
    Serial_connector& operator=(const Serial_connector&) = delete;
    Serial_connector(Serial_connector&&);
    Serial_connector& operator=(Serial_connector&&);

    ~Serial_connector();
    int connect(int flgs=O_RDWR|O_NOCTTY, speed_t=B57600, std::string str="");
    void disconnect();
    int get_fd() const;
    std::string get_port() const;
    bool is_connected() const;
protected:

private:
    std::string port;
    int fd;
    struct termios SerialPortSettings;
    bool connected{false};
};

class Serial_connector_error: public std::runtime_error
{
    using std::runtime_error::runtime_error;

};

}

#endif // SERIAL_CONNECTOR_H

