#pragma once
#include <memory>
#include <iostream>
#include <string>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>

namespace net = boost::asio;
using tcp = net::ip::tcp;

namespace sys = boost::system;

namespace tcp_server{

    class SessionBase{
    public:
        SessionBase(const SessionBase&) = delete;
        SessionBase& operator=(const SessionBase&) = delete;
        void Run();

    protected:
    SessionBase(tcp::socket&& socket):socket_{std::move(socket)}{}

    void Write(std::string server_data){
        auto safe_server_data = std::make_shared<std::string> (std::move(server_data));
        auto self = GetSharedThis();
        net::async_write(socket_, net::buffer(safe_server_data->c_str(), safe_server_data->size()),
        [safe_server_data, self](sys::error_code ec, std::size_t bytes){
            self->OnWrite(ec);
        });
    }

    private:
        void Read();
        void OnRead(sys::error_code ec, std::size_t bytes);
        void OnWrite(sys::error_code ec);
        void Close();

        net::streambuf stream_buf_;
        std::string client_data_;
        tcp::socket socket_;

        virtual std::shared_ptr<SessionBase> GetSharedThis() = 0;
        virtual void Handle(std::string client_data) = 0;
    };

    template <typename Handler>
    class Session: public SessionBase, public std::enable_shared_from_this<Session<Handler>> {
    public:
    template <typename Handl>
    Session(tcp::socket&& socket, Handl&& hanlder):
    SessionBase(std::move(socket)), handler_(std::forward<Handl>(handler_)){}

    private:
    Handler handler_;
    std::shared_ptr<SessionBase> GetSharedThis() override{
        return this->shared_from_this();
    }

    void Handle(std::string client_data) override{
        handler_(std::move(client_data), [self = this->shared_from_this()](std::string server_data){
        self->Write(std::move(server_data));
        });
    }

    };



    template <typename Handler>
    class Listener: public std::enable_shared_from_this<Listener<Handler>>{
    public:
        template<typename Handl>
        Listener(net::io_context& io_context, const tcp::endpoint& endpoint, Handl&& handler):
            io_context_ {io_context},
            acceptor_ {net::make_strand(io_context_)},
            handler_{std::forward<Handler>(handler)}
        {
            acceptor_.open(endpoint.protocol());
            acceptor_.set_option(net::socket_base::reuse_address(true));
            acceptor_.bind(endpoint);
            acceptor_.listen(net::socket_base::max_listen_connections);
        }
        void Run(){
            DoAccept();
        }

    private:
        void DoAccept(){
            acceptor_.async_accept(net::make_strand(io_context_),
            std::bind_front(&Listener::OnAccept, this->shared_from_this()));
        }
        void OnAccept(sys::error_code ec, tcp::socket socket){
            if(ec){
                return;
            }
            AsyncRunSession(std::move(socket));
            DoAccept();
        }
        void AsyncRunSession(tcp::socket&& socket){
            std::make_shared<Session<Handler>>(std::move(socket), handler_)->Run();
        }


    net::io_context& io_context_;
    tcp::acceptor acceptor_;
    Handler handler_;

    };

    template <typename Handler>
    void TcpServer(net::io_context& io_context, const tcp::endpoint& endpoint, Handler&& handler){
        using MyListener = Listener<std::decay_t<Handler>>;
        std::make_shared<MyListener>(io_context, endpoint, std::forward<Handler>(handler))->Run();
    }



}
