#pragma once

#include <string>

#include <lightex/ast/ast.h>
#include <lightex/utils/counter.h>

#include <boost/variant/static_visitor.hpp>

namespace lightex {
namespace visitor {

using NodeId = std::string;

// Returns node id.
class DotVisitor : public boost::static_visitor<NodeId> {
 public:
  DotVisitor(std::string* output);

  NodeId operator()(const ast::Program& program);

  NodeId operator()(const ast::Command& command);

  NodeId operator()(const ast::PlainText& plain_text);

  NodeId operator()(const ast::MathText& math_text);

  NodeId operator()(const ast::Environment& environment);

 private:
  NodeId GenerateNodeId();
  void AppendToOutput(const std::string& s);

  utils::Counter counter_;
  std::string* output_;  // Not owned;
};

}  // namespace visitor
}  // namespace lightex
