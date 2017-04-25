#include <lightex/utils/counter.h>

namespace lightex {
namespace utils {

Counter::Counter(int initial_value) : value_(initial_value) {}

int Counter::GetValueAndIncrease() {
  return value_++;
}
}  // namespace utils
}  // namespace lightex
