#include <lightex/html_converter/html_preprocessor.h>

#include <regex>
#include <vector>

namespace lightex {
namespace html_converter {

const std::vector<std::pair<std::string, std::string>> kRegexReplaceParams = {
  {"~", "\\unescaped{&nbsp;}"},
  {"---", "\\unescaped{&mdash;}"},
};

std::string PreprocessHtmlInput(const std::string& input) {
  std::string result = input;

  for (const auto& pair : kRegexReplaceParams) {
    std::regex expression(pair.first);
    result = std::regex_replace(result, expression, pair.second);
  }

  return result;
}

}  // namespace html_converter
}  // namespace lightex
