#include <lightex/utils/comment_remover.h>

namespace lightex {
namespace utils {

std::string RemoveCommentsFromProgram(const std::string& program_with_comments) {
  std::string result;

  bool is_commented = false;
  for (char c : program_with_comments) {
    switch (c) {
      case '%': {
        is_commented = true;
        break;
      }
      case '\n': {
        is_commented = false;
        break;
      }
    }

    if (!is_commented) {
      result += c;
    }
  }

  return result;
}

}  // namespace utils
}  // namespace lightex
