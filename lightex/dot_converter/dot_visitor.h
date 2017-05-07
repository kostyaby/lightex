#pragma once

#include <string>

#include <lightex/ast/ast.h>

#include <boost/variant/static_visitor.hpp>

namespace lightex {
namespace dot_converter {

using NodeId = std::string;

// Returns node id.
class DotVisitor : public boost::static_visitor<NodeId> {
 public:
  DotVisitor(std::string* output);

  NodeId operator()(const ast::Program& program);
  NodeId operator()(const ast::PlainText& plain_text);
  NodeId operator()(const ast::Paragraph& paragraph);
  NodeId operator()(const ast::ParagraphBreaker& paragraph_breaker);
  NodeId operator()(const ast::Argument& argument);
  NodeId operator()(const ast::ArgumentRef& argument_ref);
  NodeId operator()(const ast::OuterArgumentRef& outer_argument_ref);
  NodeId operator()(const ast::InlinedMathText& inlined_math_text);
  NodeId operator()(const ast::MathText& math_text);
  NodeId operator()(const ast::CommandMacro& command_macro);
  NodeId operator()(const ast::EnvironmentMacro& environment_macro);
  NodeId operator()(const ast::Command& command);
  NodeId operator()(const ast::UnescapedCommand& unescaped_command);
  NodeId operator()(const ast::Environment& environment);

 private:
  NodeId GenerateNodeId();
  void AppendToOutput(const std::string& s);

  int next_node_id_;
  std::string* output_;  // Not owned;
};

}  // namespace dot_converter
}  // namespace lightex
