#pragma once
#include <signal.h>
#include <sstream>
#include <thread>

namespace chan {
namespace threads {
static inline int send_signal_thread(std::thread &t, int signal) {
  std::stringstream ss;
  ss << t.get_id();
  auto id = std::stoul(ss.str());
  return pthread_kill(id, signal);
}
};  // namespace threads
};  // namespace chan
