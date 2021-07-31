#ifndef SERIAL_COMMAND_H
#define SERIAL_COMMAND_H

#include <iostream>
#include "Serial_connector.h"
#include <mutex>
#include <chrono>
namespace cub
{
class Serial_command
{
public:
    Serial_command(cub::Serial_connector&&);
    bool run(std::string msg, std::chrono::microseconds);

protected:

private:
    std::timed_mutex serial_mutex{};
    cub::Serial_connector sc;
};
}
#endif // SERIAL_COMMAND_H
