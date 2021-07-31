#include "utility.h"
#include "Serial_command.h"
#include <unistd.h>

std::map<std::string, std::filesystem::path> find_ports()
{
    using namespace std::chrono;
    std::string tgt{"ttyUSB"};
    std::map<std::string, std::filesystem::path> res{};
    for(auto p: std::filesystem::directory_iterator("/dev"))
    {
        std::string dir{p.path().string()};
        if(search( std::begin(dir), std::end(dir), std::begin(tgt), std::end(tgt))!= dir.end() )
        {
            cub::Serial_connector sc{dir};
            sc.connect();
            int fd = sc.get_fd();
            std::promise<std::string> pro{};
            std::future<std::string> fut = pro.get_future();
            std::thread th{ [fd, p = std::move(pro)]()mutable{
                    std::string res{};
                    char ch{};
                    do
                    {
                        std::this_thread::sleep_for(5ms);
                        [[maybe_unused]] auto read_bytes = read(fd, &ch, 1);
                        if(ch != '\n')
                            res += ch;

                    }
                    while(ch != '\n');
                    p.set_value(res);
                    return;
                }
            };

            std::string msg{"query@"};
            [[maybe_unused]] auto written_bytes =  write(fd, msg.c_str(), msg.size());
            auto status = fut.wait_for(1s);
            if(status != std::future_status::ready)
            {
                throw std::runtime_error{"can't get serial query."};
            }
            std::string dev{fut.get()};
            th.join();
            res.emplace(dev.substr(0,dev.size()-1), p.path());
        }
    }
    return res;
}

