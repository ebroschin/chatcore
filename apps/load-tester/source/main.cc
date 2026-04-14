#include "application/load_tester_application.h"
#include "application/load_tester_arguments.h"

int main(int argc, char** argv) {
  const claw::chat::tester::LoadTesterArguments arguments{argc, argv};
  claw::chat::tester::LoadTesterApplication application{arguments};
  application.RunBlocking();

  return 0;
}