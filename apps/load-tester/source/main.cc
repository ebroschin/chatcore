#include "application/load_tester_application.h"
#include "application/load_tester_arguments.h"

int main(int argc, char** argv) {
  const ebroschin::chatcore::tester::LoadTesterArguments arguments{argc, argv};
  ebroschin::chatcore::tester::LoadTesterApplication application{std::move(arguments)};
  application.RunBlocking();

  return 0;
}