#include "boost_tcp_server.h"

#include <ranges>

namespace claw::communication {

std::unique_ptr<chat::server::BoostTcpConnection> BoostTcpServer::AcceptClientConnection() {
  tcp::socket socket{io_context_};
  acceptor_.accept(socket);
  return std::make_unique<chat::server::BoostTcpConnection>(std::move(socket));
}

// void BoostTCPServer::Update() {
//
//   //wait for the single client to connect, create a TcpConnection
//   if (connection_ == nullptr) {
//
//   }
//
//   if (connection_->HasData()) {
//     const std::string message = connection_->ReadMessage();
//     ProcessMessage(message);
//     SendMessage("you wrote: " + message);
//   }
//
//   SendResponse();
// }

// void BoostTCPServer::Deinitialize() {
//   std::cout << "server shutdown" << std::endl;
// }
//
// void BoostTCPServer::SendMessage(const std::string &message) {
//   response_message_buffer_ += message + "\n";
// }
//
// void BoostTCPServer::SendResponse() {
//   if (response_message_buffer_.empty()) return;
//   if (connection_ == nullptr) return;
//
//   connection_->SendMessage(response_message_buffer_);
//
//   std::cout << "[SENT] " << response_message_buffer_ << std::endl;
//   response_message_buffer_.clear();
// }
//
// void BoostTCPServer::ProcessMessage(const std::string& message) {
//   std::cout << "[RECEIVED] " << message << std::endl;
//   HandleMessage(message_type, content);
// }

}