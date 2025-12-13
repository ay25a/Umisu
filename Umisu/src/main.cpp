#include "MikuAgent.hpp"
#include <cstdio>

int main() {
  MikuAgent agent;
  if (auto result = agent.Init(); !result) {
    printf("Error: %s\n", result.error().c_str());
    return -1;
  }

  agent.Run();

  return 0;
}