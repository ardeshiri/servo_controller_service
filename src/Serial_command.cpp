#include "Serial_command.h"
#include <unistd.h>
#include <thread>

cub::Serial_command::Serial_command(cub::Serial_connector&& s): sc{std::move(s)}
{
    if(!sc.is_connected())
    {
        sc.connect();
    }
}


bool cub::Serial_command::run(std::string msg, std::chrono::microseconds ms)
{
    using namespace std::chrono;
    std::unique_lock<std::timed_mutex> ul{serial_mutex, std::defer_lock};
    if(ul.try_lock_for(3s))
    {
        [[maybe_unused]] auto written_bytes = write(sc.get_fd(), msg.c_str(), msg.size());
        std::this_thread::sleep_for(ms);
        return true;
    }
    return false;
}
