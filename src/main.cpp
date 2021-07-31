#include <iostream>
#include "Serial_connector.h"
#include <unistd.h>
#include <thread>
#include <chrono>
#include <mutex>
#include "Serial_command.h"

#include <map>
#include <algorithm>
#include <future>
#include "CPPAMQP.h"
#include "utility.h"

using namespace std;
using namespace std::chrono;

const auto run_delay = 120ms;
const int max_sending_errors = 4;
const auto port_check_delay = 3s;

int main(int argc, char** argv)
try{

    if(argc != 5)
    {
        throw std::runtime_error("wrong number of inputs.");
    }

    auto ports = find_ports();
    if(!ports.contains("servo_controller"))
    {
        throw std::runtime_error("servo_controller not connected.");
    }

    std::string rmq_address{argv[1]};
    int rmq_port{std::stoi(argv[2])};
    std::string rmq_username{argv[3]};
    std::string rmq_pass{argv[4]};

    arax::CPPAMQP rmq{rmq_address,
                      rmq_port,
                      500000};
    rmq.connect_normal("/",
                       rmq_username,
                       rmq_pass);

    rmq.listen_to_queue(1,"servo_ctrl");
    cub::Serial_command cm{cub::Serial_connector{ports["servo_controller"]}};

    std::jthread serial_access_th{ [&ports]()
    {
        for(;;)
        {
            if(access(ports["servo_controller"].c_str(), R_OK|W_OK) != 0)
            {
                throw runtime_error("serial disconnected.");
            }
            this_thread::sleep_for(port_check_delay);
        }
    }};

    int unable_to_send{};
    for(;;)
    {
        auto str_vec = rmq.receive_string(1, nullptr);
        for(const auto&[e,r,q,m]: str_vec)
        {
            if(!cm.run(m,run_delay))
            {
                unable_to_send++;
            }
            else
            {
                unable_to_send = 0;
            }
            if(unable_to_send >= max_sending_errors)
            {
                throw std::runtime_error("error sending commands to serial.");
            }
        }
    }
    return 0;
}
catch(std::runtime_error& re)
{
    std::cerr<<"exception: "<<re.what()<<std::endl;
}
catch(...)
{
    std::cerr<<"exception: "<<"unknown."<<std::endl;
}
