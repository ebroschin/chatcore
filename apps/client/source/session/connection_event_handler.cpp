#include "connection_event_handler.hpp"

#include "session_system.hpp"

namespace ebroschin::chatcore::client {

ConnectionEventHandler::ConnectionEventHandler(SessionSystem& session_system) noexcept:
  session_system_{session_system}
{ }

void ConnectionEventHandler::OnConnected(network::ConnectionId connection_id) {
  session_system_.connection_id_.emplace(connection_id);
  logging::Log::Info() << "Successfully connected to chat server.";
}

void ConnectionEventHandler::OnConnectionFailed(const network::modules::BoostTcpResolverParameters& parameters) {
  logging::Log::Error() << "Connection to chat server failed " << parameters.ip + ":" + parameters.port;
}

void ConnectionEventHandler::OnDisconnected(network::ConnectionId) {
  session_system_.connection_id_.reset();
  session_system_.user_.reset();
  logging::Log::Info() << "Lost connection to chat server.";
  session_system_.model_system_.SetChannelName(std::nullopt);
}

}