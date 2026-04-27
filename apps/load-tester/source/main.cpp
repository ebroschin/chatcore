#include "application/load_tester_application.hpp"
#include "application/load_tester_arguments.hpp"

int main(int argc, char** argv) {
  ebroschin::chatcore::tester::LoadTesterArguments arguments{argc, argv};
  ebroschin::chatcore::tester::LoadTesterApplication application{std::move(arguments)};
  application.RunBlocking();

  return 0;
}