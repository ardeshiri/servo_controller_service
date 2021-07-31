#include "Serial_connector.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>

cub::Serial_connector::Serial_connector():port{},fd{-1},SerialPortSettings{}
{}

cub::Serial_connector::Serial_connector(std::string prt):port{prt},fd{-1},SerialPortSettings{}
{}


cub::Serial_connector::Serial_connector(Serial_connector&& rv):port{rv.port},fd{rv.fd},SerialPortSettings{rv.SerialPortSettings}
{
    rv.port = std::string{};
    rv.fd = -1;
}

cub::Serial_connector& cub::Serial_connector::operator=(Serial_connector&& rv)
{
    if(fd != -1)
    {
        close(fd);
    }
    fd = rv.fd;
    SerialPortSettings = rv.SerialPortSettings;
    port = rv.port;
    return *this;
}


cub::Serial_connector::~Serial_connector()
{
    if(fd != -1)
        close(fd);
    connected = false;
}

int cub::Serial_connector::connect(int flgs, speed_t spd, std::string str)
{
    if(!str.empty())
    {
        port.assign(str);
    }
    if(port.empty())
    {
        throw Serial_connector_error("no port specified.");
    }
    if(access(port.c_str(), F_OK) != 0)
    {
        throw Serial_connector_error("file does not exist.");
    }
    fd = open(port.c_str(),flgs);
    if(fd == -1)
    {
        throw Serial_connector_error("error in opening the port.");
    }
    memset(&SerialPortSettings,0,sizeof(struct termios));
    cfsetispeed(&SerialPortSettings,spd);
    cfsetospeed(&SerialPortSettings,spd);
    SerialPortSettings.c_cflag &= ~PARENB;
    SerialPortSettings.c_cflag &= ~CSTOPB;
    SerialPortSettings.c_cflag &= ~CSIZE;
    SerialPortSettings.c_cflag |=  CS8;
    SerialPortSettings.c_cflag &= ~CRTSCTS;
    SerialPortSettings.c_cflag |= CREAD | CLOCAL;
    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    SerialPortSettings.c_oflag &= ~OPOST;
    SerialPortSettings.c_oflag &= ~ONLCR;
    SerialPortSettings.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
    SerialPortSettings.c_cc[VMIN] = 10;
    SerialPortSettings.c_cc[VTIME] = 0;
    if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0)
        throw Serial_connector_error{"set attribute error."};
    tcflush(fd, TCIFLUSH);
    connected = true;
    return fd;
}

void cub::Serial_connector::disconnect()
{
    if(fd != -1)
    {
        close(fd);
        fd = -1;
    }
    connected = false;
}

int cub::Serial_connector::get_fd() const
{
    return fd;
}

std::string cub::Serial_connector::get_port() const
{
    return port;
}

bool cub::Serial_connector::is_connected() const
{
    return connected;
}



