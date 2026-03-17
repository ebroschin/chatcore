#include "application/chat_server_application.h"

int main(int argc, char** argv) {
  const claw::chat::server::ChatServerArguments arguments{argc, argv};
  claw::chat::server::ChatServerApplication application{arguments};
  application.RunBlocking();

  return 0;
}