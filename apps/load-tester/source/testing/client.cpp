#include "client.hpp"

#include "../application/load_tester_application.hpp"

namespace ebroschin::chatcore::tester {

Client::Client(LoadTesterApplication& app, core::SystemContext& ctx, std::string name) noexcept:
  app_{app},
  ctx_{ctx},
  tcp_system_{ctx.Require<ClientTcpSystem>()},
  rpc_system_{ctx.Require<ClientRpcSystem>()},
  name_{std::move(name)}
{}

void Client::Quit() const {
  app_.Quit();
}

void Client::Prepare() {
  const auto& arguments = app_.GetArguments();
  tcp_system_.Connect({arguments.GetIp(), arguments.GetPort()}, [this](network::tcp::ConnectionResult result) {
    if (!result.Ok()) {
      logging::Log::Error() << "[Client][" << name_ << "] " << result.error;
      Quit();
      return;
    }

    OnConnected(*result.connection_id);
  });
}

void Client::OnConnected(network::ConnectionId connection_id) {
  connection_id_.emplace(connection_id);

  auto create_user_call = rpc_system_.Prepare<api::CreateUserRequestMessage>(*connection_id_, name_, std::string{TestPassword});
  create_user_call.OnSuccess([this]
  (const api::CreateUserResponseMessage& message)
  {
    logging::Log::Verbose() << "user successfully created: " << message.user.name;
    OnUserEnsured();
  });

  create_user_call.OnError([this]
  (const api::ErrorResponseMessage&)
  {
    logging::Log::Verbose() << "user already exists: " << name_;
    OnUserEnsured();
  });

  RegisterDefaultTimeoutHandler(create_user_call, "Create user request timed out");
  create_user_call.Call();
}

void Client::OnUserEnsured() {
  auto authenticate_call = rpc_system_.Prepare<api::AuthenticateUserRequestMessage>(*connection_id_, name_, std::string{TestPassword});
  authenticate_call.OnSuccess([this]
  (const api::AuthenticateUserResponseMessage& auth_message)
  {
    user_.emplace(auth_message.user);
    logging::Log::Verbose() << "user successfully authenticated: " << auth_message.user.name;
    OnPrepared();
  });

  RegisterDefaultErrorHandler(authenticate_call);
  RegisterDefaultTimeoutHandler(authenticate_call, "Authenticate user request timed out");
  authenticate_call.Call();
}

}