#include "application/chat_server_application.h"

int main(int argc, char** argv) {
  const ebroschin::chatcore::server::ChatServerArguments arguments{argc, argv};
  ebroschin::chatcore::server::ChatServerApplication application{std::move(arguments)};
  application.RunBlocking();

  return 0;
}