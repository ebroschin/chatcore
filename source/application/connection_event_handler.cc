#include "connection_event_handler.h"

#include "../users/user_server_system.h"

namespace claw::chat::server {

ConnectionEventHandler::ConnectionEventHandler(UserServerSystem& user_system) noexcept:
  user_system_{user_system}
{}

void ConnectionEventHandler::OnDisconnected(network::ConnectionId connection_id) {
  user_system_.RemoveSession(connection_id);
}

}