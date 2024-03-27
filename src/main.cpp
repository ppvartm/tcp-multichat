#include <iostream>
#include "tcp_server.h"
#include "handler.h"
#include <thread>

#include <boost/asio/signal_set.hpp>

using namespace std::literals;

template <typename Fn>
void RunWorkers(unsigned num_of_threads, const Fn& fn) {
    num_of_threads = std::max(1u, num_of_threads);
    std::vector<std::jthread> workers;
    workers.reserve(num_of_threads - 1);
    while (--num_of_threads) {
        workers.emplace_back(fn);
    }
    fn();
}




int main(int argc, char** argv){
    const auto address = net::ip::make_address("127.0.0.1");
    constexpr net::ip::port_type port = 3333;

    const unsigned num_threads = std::thread::hardware_concurrency();
    net::io_context ioc(num_threads);

    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](const boost::system::error_code& ec, int signal_number) {
        if (!ec) {
            ioc.stop();
        }; }
    );

    net::strand strand = net::make_strand(ioc);

    tcp_handler::Handler handler;

    tcp_server::TcpServer(ioc, { address, port }, [&handler](auto&& req, auto&& send) {
    handler(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));});

    std::cout << "Server has started..."sv << std::endl;
    RunWorkers(std::max(1u, num_threads), [&ioc] {
        ioc.run();
    });
}



