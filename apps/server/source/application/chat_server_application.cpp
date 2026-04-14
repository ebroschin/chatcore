#include "chat_server_application.h"

#include "../chat/adapters/sqlite_chat_persistence_adapter.h"
#include "../chat/chat_server_system.h"
#include "../persistence/persistence_system.h"
#include "../sqlite_persistence/sqlite_persistence_store.h"
#include "../users/adapters/sqlite_user_persistence_adapter.h"
#include "../users/user_server_system.h"
#include "application_system.h"
#include "chat_persistence_system.h"
#include "chat_tcp_system.h"
#include <claw/scheduling/scheduling_system.h>

#include <boost/stacktrace.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

using namespace claw::persistence::sqlite;

namespace claw::chat::server {

ChatServerApplication::ChatServerApplication(ChatServerArguments arguments):
  arguments_(std::move(arguments))
{ }

void ChatServerApplication::Initialize() {
  ctx_.Register<scheduling::SchedulingSystem>();
  ctx_.Register<ChatServerTcpSystem>();

  auto* persistence_system = ctx_.Register<ChatPersistenceSystem>(arguments_.GetSqliteFilename());
  persistence_system->Register<ChatPersistenceAdapter, SqliteChatPersistenceAdapter>();
  persistence_system->Register<UserPersistenceAdapter, SqliteUserPersistenceAdapter>();

  ctx_.Register<ApplicationSystem>(*this);
  ctx_.Register<UserServerSystem>();
  ctx_.Register<ChatServerSystem>();

  //TODO logging abstraction
  spdlog::init_thread_pool(8192, 1);
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::trace);

  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      "logs/chat.log", 10 * 1024 * 1024, 5);
  file_sink->set_level(spdlog::level::trace);

  const unsigned short log_level = arguments_.GetLogLevel();
  const auto logger = std::make_shared<spdlog::async_logger>(
      "chat",
      spdlog::sinks_init_list{console_sink, file_sink},
      spdlog::thread_pool(),
      spdlog::async_overflow_policy::block
  );

  if (log_level == 0) logger->set_level(spdlog::level::info);
  else logger->set_level(spdlog::level::trace);

  logger->flush_on(spdlog::level::err);
  spdlog::flush_every(std::chrono::seconds(1));
  spdlog::set_default_logger(logger);
}

void ChatServerApplication::HandleTerminate() {
  using namespace std::string_literals;

  try {
    std::rethrow_exception(std::current_exception());
  } catch (const std::exception& e) {
    spdlog::critical("Unhandled exception: "s + e.what());
  } catch (...) {
    spdlog::critical("Unhandled unknown exception");
  }

  std::stringstream stream;
  stream << "Stacktrace:\n"s << stacktrace::stacktrace();
  spdlog::critical(stream.str());
}

}