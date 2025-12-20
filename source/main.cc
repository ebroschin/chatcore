#include <iostream>
#include "boost/asio.hpp"
#include <ranges>

using boost::asio::ip::tcp;

//#define SERVER_CODE

#ifndef SERVER_CODE
int main(int argc, char** argv) {
  boost::asio::io_context io_context;
  tcp::resolver resolver(io_context);
  const auto endpoints = resolver.resolve("192.168.178.20", "1338");

  tcp::socket socket(io_context);
  boost::asio::connect(socket, endpoints);

  while (true) {
    std::array<char, 128> buffer;
    boost::system::error_code error;

    const size_t length = socket.read_some(boost::asio::buffer(buffer), error);
    if (error == boost::asio::error::eof)
      break;
    else if (error) {
      std::cerr << error << std::endl;
      return 1;
    }

    std::cout << "[message length] " << length << "\n" << "[message] " << buffer.data() << std::endl;
  }

  return 0;
}
#endif

#ifdef SERVER_CODE
int main(int argc, char** argv) {
  boost::asio::io_context io_context;
  tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1338));

  std::cout << "started server" << std::endl;
  while (true) {
    std::cout << "waiting for clients" << std::endl;
    tcp::socket socket(io_context);
    acceptor.accept(socket);
    std::cout << "client connected!" << std::endl;

    std::string message(argv[1]);
    std::vector<char> buffer;
    boost::asio::write(socket, boost::asio::buffer(message));
  }

  std::cout << "hello world" << std::endl;
  return 0;
}

#endif