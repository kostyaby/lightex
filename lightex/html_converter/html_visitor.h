#pragma once

#include <string>
#include <vector>

#include <lightex/ast/ast.h>

#include <boost/variant/static_visitor.hpp>

namespace lightex {
namespace html_converter {

struct Result {
  bool is_successful;

  std::string escaped;
  std::string unescaped;
  std::string error_message;

  static Result Failure(const std::string& error_message);
  static Result Success(const std::string& escaped, const std::string& unescaped);
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
  Result operator()(const ast::VerbatimEnvironment& verbatim_environment);

 private:
  template <typename Node>
  Result JoinNodeResults(const std::list<Node>& nodes, const std::string& separator);

  template <typename Macro, typename MacroDefinition>
  Result PrepareMacroArguments(const Macro& macro,
                               const MacroDefinition& macro_definition,
                               std::vector<Result>* output_args);

  const ast::CommandMacro* GetDefinedCommandMacro(const std::string& name) const;
  const ast::EnvironmentMacro* GetDefinedEnvironmentMacro(const std::string& name) const;
  const Result* GetArgumentByReference(int index, bool is_outer) const;

  int active_environment_definitions_num_ = 0;
  int math_text_span_num_ = 0;
  std::vector<std::vector<Result>> arguments_stack_;

  std::list<ast::CommandMacro> defined_command_macros_;
  std::list<ast::EnvironmentMacro> defined_environment_macros_;
};

}  // namespace html_converter
}  // namespace lightex
