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
  Result operator()(const ast::PlainText& plain_text);
  Result operator()(const ast::Paragraph& paragraph);
  Result operator()(const ast::ParagraphBreaker& paragraph_breaker);
  Result operator()(const ast::Argument& argument);
  Result operator()(const ast::ArgumentRef& argument_ref);
  Result operator()(const ast::OuterArgumentRef& outer_argument_ref);
  Result operator()(const ast::InlinedMathText& inlined_math_text);
  Result operator()(const ast::MathText& math_text);
  Result operator()(const ast::CommandMacro& command_macro);
  Result operator()(const ast::EnvironmentMacro& environment_macro);
  Result operator()(const ast::Command& command);
  Result operator()(const ast::UnescapedCommand& unescaped_command);
  Result operator()(const ast::Environment& environment);

 private:
  const ast::CommandMacro* GetDefinedCommandMacro(const std::string& name) const;
  const std::string* GetArgumentByReference(int index, bool is_outer) const;

  // Always std::move argument_vector to gain efficiency.
  void PushArgumentVector(std::vector<std::string> argument_vector);
  void PopArgumentVector();

  int active_macro_definitions_num_ = 0;
  int math_text_span_num_ = 0;
  int active_unescaped_num_ = 0;
  std::vector<std::vector<std::string>> argument_vector_stack_;
  std::list<ast::CommandMacro> defined_command_macros_;
};

}  // namespace html_converter
}  // namespace lightex
