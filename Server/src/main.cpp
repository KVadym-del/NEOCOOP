#include "Server.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fmt::print(stderr, "Usage: chat_server <port>\n");
        return 1;
    }

    try
    {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), std::atoi(argv[1]));

        Server server(io_context, endpoint);

        io_context.run();
    }
    catch (std::exception &e)
    {
        printlnerr(e.what());
    }

    return 0;
}
