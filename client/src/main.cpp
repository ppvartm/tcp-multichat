#include <iostream>
#include <string>
#include <boost/asio.hpp>


namespace net = boost::asio;
using net::ip::tcp;



int main(int argc, char** argv){

   if(argc != 4){
       std::cout << "Incorrect number of argements; \n" << argv[0] << "<server ip> <port> <your nickname>";
    return 1;
   }

   boost::system::error_code ec;

   auto ip = net::ip::make_address(argv[1], ec);
   if(ec){
       std::cout << "Incorrect ip\n";
       return 1;
   }

   int port = std::stoi(argv[2]);
   std::string name = argv[3];

   std::cout << port << "  " << ip << "\n";
   auto endpoint = tcp::endpoint(ip, port);

   net::io_context ioc;

   net::signal_set signals(ioc, SIGINT, SIGTERM);
   signals.async_wait([&ioc](const boost::system::error_code& ec, int signal_number) {
       if (!ec) {
           ioc.stop();
       }; }
   );

   tcp::socket socket{ioc};
   socket.connect(endpoint, ec);

   if(ec){
       std::cout << "Socket doesn't connect\n";
       return 1;
   }

   while(true){
      std::string input;
      std::cout << "Entry your message: ";
      std::getline(std::cin, input);
      socket.write_some(net::buffer(name + ": " + input + "\n"), ec);
      if(ec){
        std::cout << "Error sending data\n";
        return 1;
      }

      net::streambuf stream_buf;
      net::read_until(socket, stream_buf, '\n', ec);
      std::string server_data{ std::istreambuf_iterator<char>(&stream_buf),
                                std::istreambuf_iterator<char>() };
      if (ec) {
          std::cout << "Error reading data from server\n";
          return 1;
      }
      std::cout << "Server responded: " << server_data << std::endl;
   }



}
