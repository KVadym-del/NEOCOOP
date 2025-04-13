#include <boost/asio.hpp>
#include <cstdio>
#include <deque>
#include <fmt/base.h>
#include <fmt/format.h>
#include <set>
#include <string>

static inline void printlnerr(std::string str)
{
    fmt::print(stderr, "ERROR: {}\n", str);
}

class ChatParticipant
{
  public:
    virtual ~ChatParticipant()
    {
    }
    virtual void deliver(const std::string &message) = 0;
};

using ChatParticipantPtr = std::shared_ptr<ChatParticipant>;

class ChatRoom
{
    enum : int
    {
        max_recent_msgs = 100
    };

  public:
    void join(ChatParticipantPtr participant)
    {
        m_participants.insert(participant);
        for (auto &msg : m_recent_msgs)
            participant->deliver(msg);

        fmt::print("Participant joined the chat room\n");
    }

    void leave(ChatParticipantPtr participant) noexcept
    {
        m_participants.erase(participant);
        fmt::print("Participant left the chat room\n");
    }

    void deliver(const std::string &msg)
    {
        m_recent_msgs.push_back(msg);
        while (m_recent_msgs.size() > max_recent_msgs)
            m_recent_msgs.pop_front();

        for (auto const &r_msg : m_recent_msgs)
            fmt::print("DEBUG: {}", r_msg);

        for (auto participant : m_participants)
            participant->deliver(msg);
    }

  private:
    std::set<ChatParticipantPtr> m_participants;
    std::deque<std::string> m_recent_msgs;
};

class ChatSession : public ChatParticipant, public std::enable_shared_from_this<ChatSession>
{
  public:
    ChatSession(boost::asio::ip::tcp::socket socket, ChatRoom &room) : m_socket(std::move(socket)), m_room(room)
    {
    }

    void start()
    {
        m_room.join(shared_from_this());
        doRead();
    }

    void deliver(const std::string &message) override
    {
        bool write_in_progress = !m_write_msgs.empty();
        m_write_msgs.push_back(message);
        if (!write_in_progress)
            doWrite();
    }

  private:
    void doRead()
    {
        auto self(shared_from_this());
        m_socket.async_read_some(boost::asio::buffer(m_read_msg),
                                 [this, self](boost::system::error_code ec, std::size_t length) {
                                     if (!ec)
                                     {
                                         m_room.deliver(std::string(m_read_msg.data(), length));
                                         doRead();
                                     }
                                     else
                                     {
                                         m_room.leave(shared_from_this());
                                     }
                                 });
    }

    void doWrite()
    {
        auto self(shared_from_this());
        boost::asio::async_write(m_socket, boost::asio::buffer(m_write_msgs.front()),
                                 [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                     if (!ec)
                                     {
                                         m_write_msgs.pop_front();
                                         if (!m_write_msgs.empty())
                                             doWrite();
                                     }
                                     else
                                     {
                                         m_room.leave(shared_from_this());
                                     }
                                 });
    }

  private:
    boost::asio::ip::tcp::socket m_socket;
    ChatRoom &m_room;
    std::array<char, 1024> m_read_msg;
    std::deque<std::string> m_write_msgs;
};

class Server
{
  public:
    Server(boost::asio::io_context &io_context, const boost::asio::ip::tcp::endpoint &endpoint)
        : m_acceptor(io_context, endpoint)
    {
        if (!m_acceptor.is_open())
        {
            printlnerr(fmt::format("Could not open acceptor on port {}", endpoint.port()));
            return;
        }

        fmt::print("Chat server started on port {}\n", endpoint.port());
        startAccept();
    }

  private:
    void startAccept()
    {
        m_acceptor.async_accept([this](const boost::system::error_code &error, boost::asio::ip::tcp::socket socket) {
            if (!error)
            {
                fmt::print("Accepted connection from {}\n", socket.remote_endpoint().address().to_string());
                std::make_shared<ChatSession>(std::move(socket), m_chatRoom)->start();
            }
            else
            {
                printlnerr(fmt::format("Error accepting connection: {}", error.message()));
            }

            startAccept();
        });
    }

  private:
    boost::asio::ip::tcp::acceptor m_acceptor;
    ChatRoom m_chatRoom;
};
