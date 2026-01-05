#include "tcp_system.h"

namespace claw::communication {

template<>
TestMessage TestSerializer::Deserialize<TestMessage>(const std::string& raw) {
  TestMessage msg;
  msg.value = raw;
  return msg;
}

template<>
OtherMessage TestSerializer::Deserialize<OtherMessage>(const std::string& raw) {
  OtherMessage msg;
  msg.value = std::stoi(raw) + 111;
  return msg;
}

template<>
AnotherMessage TestSerializer::Deserialize<AnotherMessage>(const std::string& raw) {
  AnotherMessage msg;
  msg.value = std::stof(raw);
  msg.value2 = std::stof(raw) + 1.5f;
  msg.value3 = std::stof(raw) + 3.5f;
  return msg;
}

template<>
std::string TestSerializer::Serialize<OtherMessage>(const OtherMessage& raw) {
  return "other:" + std::to_string(raw.value);
}

template<>
std::string TestSerializer::Serialize<TestMessage>(const TestMessage& raw) {
  return "test:" + raw.value;
}

template<>
std::string TestSerializer::Serialize<AnotherMessage>(const AnotherMessage& raw) {
  return "another:" + std::to_string(raw.value) + " " + std::to_string(raw.value2) + " " + std::to_string(raw.value3) + " ";
}

}