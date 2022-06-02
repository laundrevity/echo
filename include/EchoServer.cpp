#include "EchoServer.h"
#include <iostream>

EchoServer::EchoServer(boost::asio::io_context& io_context) 
: io_context_(io_context), tcp_acceptor_(io_context.get_executor(), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::tcp::v4(), 1337))
{
    std::cout << "initialized :P" << std::endl;
    start_accept();
}


std::shared_ptr<TcpConnection> TcpConnection::create(boost::asio::io_context& io_context)
{
    return std::make_shared<TcpConnection>(io_context);
}

boost::asio::ip::tcp::tcp::socket& TcpConnection::socket() {
        return socket_;
}

void TcpConnection::start() {

        message_ = "GENERIC";
        std::cout << "calling async_write to TcpConnection::handle_write" << std::endl;

        boost::asio::async_write(
            socket_, 
            boost::asio::buffer(message_),
            [self = shared_from_this(), this] (const boost::system::error_code& error, size_t bytes_transferred) {
                this->handle_write(error, bytes_transferred);
            }
        );
}

void TcpConnection::write(const std::string& message) {
    std::cout << "TcpConnection::write(string)" << std::endl;
    if (send_buffer_.empty()) {
        send_buffer_.insert(send_buffer_.end(), message.begin(), message.end());
        write();
    } else {
        std::cout << "TcpConnection::write(string) adding data to be send later" << std::endl;
        waiting_to_be_sent_.insert(waiting_to_be_sent_.end(), message.begin(), message.end());        
    }
}


void TcpConnection::write() {
    std::cout << "TcpConnection::write" << std::endl;
    boost::asio::async_write(
    socket_, boost::asio::buffer(send_buffer_.data(), send_buffer_.size()),
    [self = shared_from_this(), this] (const boost::system::error_code& error, size_t bytes_transfrd){
        this->handle_write(error, bytes_transfrd);
    });
}

TcpConnection::TcpConnection(boost::asio::io_context& io_context) 
    : socket_(io_context) {}

void TcpConnection::handle_write(const boost::system::error_code& error, size_t bytes_transfrd) {
    std::cout << "UnixConnection::handle_write" << std::endl;
    std::cout << "bytes_transfrd=" << bytes_transfrd << std::endl;
    std::cout << "send_buffer_.size()=" << send_buffer_.size() << std::endl;
    std::cout << "message_=" << std::string_view(send_buffer_.data(), send_buffer_.size()) << std::endl;
    send_buffer_.clear();
    if (!waiting_to_be_sent_.empty()) {
        std::cout << __FUNCTION__ << " found more to write " << std::string_view(waiting_to_be_sent_.data(), waiting_to_be_sent_.size()) << std::endl;
        swap(send_buffer_, waiting_to_be_sent_);
        write();
    }
}

void EchoServer::start_accept() {
    std::cout << "EchoServer::start_accept" << std::endl;
    std::shared_ptr<TcpConnection> new_tcp_connection = TcpConnection::create(io_context_);
    //web_server_ = UnixConnection::create(io_context_);
    tcp_acceptor_.async_accept(
        new_tcp_connection->socket(),
        [this, new_tcp_connection] (const boost::system::error_code& error) {
            this->handle_accept(new_tcp_connection, error);
        }
    );
}

void EchoServer::handle_accept(const std::shared_ptr<TcpConnection>& new_connection,
const boost::system::error_code& error) {
    std::cout << "EchoServer::handle_accept" << std::endl;
    if (!error) {
        boost::asio::async_read(
            new_connection->socket(), 
            boost::asio::buffer(read_msg_),
            boost::asio::transfer_at_least(4),
            [this, new_connection] (const boost::system::error_code& e, size_t bytes_transferred) {
                this->handle_user_read(new_connection, e, bytes_transferred);
            }
        );
        new_connection->start();
    }
    start_accept();
}

void EchoServer::handle_user_read(
    std::shared_ptr<TcpConnection> new_connection, 
    const boost::system::error_code& e, 
    size_t n_bytes_transferred
) {
    std::cout << "EchoServer::handle_user_read" << std::endl;
    auto view = std::string_view(read_msg_, n_bytes_transferred);
    std::cout << view << std::endl;

    std::cout << "ECHO: " << view.data() << std::endl;

    std::stringstream ss;
    ss << "ECHO: " << view.data();
    
    std::cout << ss.str() << std::endl;

    new_connection->write(ss.str());

}

void EchoServer::handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transfrd*/) {
        std::cout << "EchoServer::handle_write" << std::endl;

}