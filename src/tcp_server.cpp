#include "tcp_server.h"

namespace tcp_server{
    void SessionBase::Run(){
        net::dispatch(socket_.get_executor(), std::bind_front(&SessionBase::Read, GetSharedThis()));
    }

    void SessionBase::Read(){

        net::async_read_until(socket_, stream_buf_, '\n',
        std::bind_front(&SessionBase::OnRead, GetSharedThis()));
    }

    void SessionBase::OnRead(sys::error_code ec, std::size_t bytes){
        if(ec){
            Close();
            return;
        }
        client_data_ = std::string{std::istreambuf_iterator<char>(&stream_buf_), std::istreambuf_iterator<char>()};
        Handle(client_data_);
    }

    void SessionBase::OnWrite(sys::error_code ec){
        if(ec){
            Close();
            return;
        }
        Read();
    }

    void SessionBase::Close(){
        sys::error_code ec;
        socket_.shutdown(tcp::socket::shutdown_send, ec);
        if (ec)
            return;
    }

}
