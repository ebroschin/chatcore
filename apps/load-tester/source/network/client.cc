#include "client.h"

#include <iostream>

#include "../application/application_system.h"

namespace claw::chat::tester {

Client::Client(ApplicationSystem& app_system,
  ClientTcpSystem& tcp_system,
  ClientRpcSystem& rpc_system,
  const std::string& name):
  app_system_(app_system),
  tcp_system_(tcp_system),
  rpc_system_{rpc_system},
  name_(name)
{}

void Client::Prepare() {
  const auto& arguments = app_system_.GetArguments();
  tcp_system_.Connect({arguments.GetIp(), arguments.GetPort()}, this);
}

void Client::OnConnected(network::ConnectionId connection_id) {
  connection_id_.emplace(connection_id);

  auto create_user_call = rpc_system_.Prepare<api::CreateUserRequestMessage>(*connection_id_, name_, "123");
  create_user_call.OnSuccess([this](const api::CreateUserResponseMessage& message) {
    OnUserCreated(message);
  });

  RegisterDefaultErrorHandler(create_user_call);
  RegisterDefaultTimeoutHandler(create_user_call, "Create user request timed out");
  create_user_call.Call();
}

void Client::OnUserCreated(const api::CreateUserResponseMessage& message) {
  std::cout << "create successful: " << message.user.name << " / " << message.user.id << std::endl;
  user_.emplace(message.user);

  auto authenticate_call = rpc_system_.Prepare<api::AuthenticateUserRequestMessage>(*connection_id_, message.user.name, "123");
  authenticate_call.OnSuccess([this](const api::AuthenticateUserResponseMessage& auth_message) {
    std::cout << "authentication successful: " << auth_message.user.name << std::endl;
    OnPrepared();
  });

  RegisterDefaultErrorHandler(authenticate_call);
  RegisterDefaultTimeoutHandler(authenticate_call, "Authenticate user request timed out");
  authenticate_call.Call();
}

}