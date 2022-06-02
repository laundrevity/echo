#include <boost/asio.hpp>
#include <iostream> 
#include "EchoServer.h"

int main() {

    boost::asio::io_context io;
    auto es = EchoServer(io);

    std::cout << "my man" << std::endl;
    
    io.run();
    
    return 0;
}