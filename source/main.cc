#include "application/chat_server_application.h"

int main(int, char**) {
  claw::chat::server::ChatServerApplication application{};
  application.Run();

  return 0;
}