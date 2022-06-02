#include <boost/asio.hpp>
#include <fstream>
#include <memory>
#include <string_view>


class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    explicit TcpConnection(boost::asio::io_context&);
    static std::shared_ptr<TcpConnection> create(boost::asio::io_context& io_context);
    boost::asio::ip::tcp::tcp::socket& socket();
    void start();
    void write(const std::string& message);

    // use the overload that takes a message to write stuff, this one writes out the send_buffer_
    // handling of calling write() is already dealt with in write(const std::string&) and handle_read
    void write();    

private:
    void handle_write(const boost::system::error_code&, size_t);
    boost::asio::ip::tcp::tcp::socket socket_;
    std::string message_;
    std::vector<char> send_buffer_;
    std::vector<char> waiting_to_be_sent_;

};

class EchoServer : public std::enable_shared_from_this<EchoServer> {
public:
    EchoServer(boost::asio::io_context& io_context);
private:
    void start_accept();
    void handle_accept(const std::shared_ptr<TcpConnection>& new_connection,
    const boost::system::error_code& error);

    void handle_user_read(
        std::shared_ptr<TcpConnection> new_connection, 
        const boost::system::error_code& e, 
        size_t n_bytes_transferred
    );

    void handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transfrd*/);

    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::tcp::acceptor tcp_acceptor_;
    bool already_started{false};
    char read_msg_[1024]{};
    std::map<int, std::shared_ptr<TcpConnection>> clients_;
    std::ofstream log_file_stream;

};