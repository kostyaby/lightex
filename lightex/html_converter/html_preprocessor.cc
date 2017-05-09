#include <lightex/html_converter/html_preprocessor.h>

#include <regex>
#include <vector>

namespace lightex {
namespace html_converter {

std::vector<std::string> kRegexReplaceParams = {"~", " ", "---", "\\unescaped{&mdash;}"};

std::string PreprocessHtmlInput(const std::string& input) {
  std::string result = input;

  for (int i = 0; 2 * i + 1 < kRegexReplaceParams.size(); ++i) {
    std::regex expression(kRegexReplaceParams[2 * i]);
    result = std::regex_replace(result, expression, kRegexReplaceParams[2 * i + 1]);
  }

  return result;
}

}  // namespace html_converter
}  // namespace lightex
