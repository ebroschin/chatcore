#include "application/chat_server_application.h"

int main(int argc, char** argv) {
  claw::chat::server::ChatServerApplication application{argc > 1? argv[1] : ""};
  application.Run();

  return 0;
}