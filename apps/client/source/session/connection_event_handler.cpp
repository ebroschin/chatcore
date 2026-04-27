#include "connection_event_handler.hpp"
#include "ebroschin/logging/log.hpp"
#include "session_system.hpp"

namespace ebroschin::chatcore::client {

ConnectionEventHandler::ConnectionEventHandler(SessionSystem& session_system) noexcept:
  session_system_{session_system}
{ }

void ConnectionEventHandler::OnConnected(network::ConnectionId connection_id) {
  session_system_.OnConnected(connection_id);
}

void ConnectionEventHandler::OnConnectionFailed(const network::modules::BoostTcpResolverParameters& parameters) {
  session_system_.OnConnectionFailed(parameters);
}

void ConnectionEventHandler::OnDisconnected(network::ConnectionId connection_id) {
  session_system_.OnDisconnected(connection_id);
}

}