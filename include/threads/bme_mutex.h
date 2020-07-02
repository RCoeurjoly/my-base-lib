#pragma once

namespace chan {
namespace threads {

// like lock_guard but only blocks if condition is true
template <typename Mutex>
class conditional_lock_guard {
 public:
  explicit conditional_lock_guard(Mutex& m, bool condition) : _mutex(m), _condition(condition) {
    if (_condition) {
      _mutex.lock();
    }
  }

  ~conditional_lock_guard() {
    if (_condition) {
      _mutex.unlock();
    }
  }

  conditional_lock_guard(const conditional_lock_guard&) = delete;
  conditional_lock_guard& operator=(const conditional_lock_guard&) = delete;

 private:
  Mutex&     _mutex;
  const bool _condition;
};

}  // namespace threads
}  // namespace chan
