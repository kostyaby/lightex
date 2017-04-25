#pragma once

#include <memory>

namespace lightex {
namespace utils {

// Not thread-safe.
class Counter {
 public:
  Counter(int initial_value = 0);

  int GetValueAndIncrease();

 private:
  int value_;
};
}  // namespace utils
}  // namespace lightex
