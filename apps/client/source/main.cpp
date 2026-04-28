#include "application/chat_client_application.hpp"

int main(int, char**) {
  ebroschin::chatcore::client::ChatClientApplication application;
  application.RunBlocking();

  return 0;
}