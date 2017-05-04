#pragma once

#include <string>
#include <vector>

#include <lightex/ast/ast.h>

#include <boost/variant/static_visitor.hpp>

namespace lightex {
namespace html_converter {

struct Result {
  bool is_successful;
  std::string value;  // If successful, it contains the resulting HTML code. Otherwise it contains error message.

  static Result Failure(const std::string& error_message);
  static Result Success(const std::string& html_code);
};

class HtmlVisitor : public boost::static_visitor<Result> {
 public:
  HtmlVisitor() {}

  Result operator()(const ast::Program& program);

  Result operator()(const ast::ParagraphBreaker& paragraph_breaker);

  Result operator()(const std::string& plain_text);

  Result operator()(const ast::ArgumentRef& argument_ref);

  Result operator()(const ast::OuterArgumentRef& outer_argument_ref);

  Result operator()(const ast::InlinedMathText& inlined_math_text);

  Result operator()(const ast::MathText& math_text);

  Result operator()(const ast::CommandMacro& command_macro);

  Result operator()(const ast::EnvironmentMacro& environment_macro);

  Result operator()(const ast::Command& command);

  Result operator()(const ast::TabularEnvironment& tabular_environment);

  Result operator()(const ast::Environment& environment);

 private:
  std::vector<bool> is_paragraphical_program = {true};
  std::vector<bool> is_paragraph_open = {false};
};

}  // namespace html_converter
}  // namespace lightex
