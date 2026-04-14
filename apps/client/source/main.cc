#include "application/chat_client_application.h"

using namespace claw::chat::client;

int main(int, char**) {
  ChatClientApplication application;
  application.RunBlocking();

  return 0;
}