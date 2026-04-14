#include "connection_event_handler.h"

#include "../users/user_server_system.h"
#include <ebroschin/logging/log.hpp>

namespace claw::chat::server {

ConnectionEventHandler::ConnectionEventHandler(UserServerSystem& user_system) noexcept:
  user_system_{user_system}
{}

void ConnectionEventHandler::OnConnected(network::ConnectionId connection_id) {
  ebroschin::logging::Log::Verbose("Client with id " + std::to_string(connection_id) + " has connected");
}

void ConnectionEventHandler::OnDisconnected(network::ConnectionId connection_id) {
  ebroschin::logging::Log::Verbose("Client with id " + std::to_string(connection_id) + " has disconnected");
  user_system_.RemoveSession(connection_id);
}

}