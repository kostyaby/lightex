#pragma once

#include <string>

#include <lightex/ast/ast.h>

#include <boost/variant/static_visitor.hpp>

namespace lightex {
namespace html_converter {

class HtmlVisitor : public boost::static_visitor<bool> {
 public:
  HtmlVisitor(std::string* output, std::string* error_message);

  bool operator()(const ast::Program& program);

  bool operator()(const std::string& plain_text);

  bool operator()(const ast::ArgumentRef& argument_ref);

  bool operator()(const ast::OuterArgumentRef& outer_argument_ref);

  bool operator()(const ast::InlinedMathText& inlined_math_text);

  bool operator()(const ast::MathText& math_text);

  bool operator()(const ast::CommandMacro& command_macro);

  bool operator()(const ast::EnvironmentMacro& environment_macro);

  bool operator()(const ast::Command& command);

  bool operator()(const ast::TabularEnvironment& tabular_environment);

  bool operator()(const ast::Environment& environment);

 private:
  void SetErrorMessage(const std::string& error_message);

  std::string* output_;         // Not owned;
  std::string* error_message_;  // Not owned;
};

}  // namespace html_converter
}  // namespace lightex
