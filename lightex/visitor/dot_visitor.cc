#include <lightex/visitor/dot_visitor.h>

namespace lightex {
namespace visitor {
namespace {

std::string EscapeForDot(const std::string& s) {
  std::string result;
  for (char c : s) {
    switch (c) {
      case '\\': {
        result += '\\';
        break;
      }
    }

    result += c;
  }

  return result;
}
}  // namespace

DotVisitor::DotVisitor(std::string* output) : output_(output) {}

NodeId DotVisitor::operator()(const ast::Program& program) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"PROGRAM\"];\n");

  for (const auto& program_node : program.nodes) {
    NodeId child_id = boost::apply_visitor(*this, program_node);
    AppendToOutput("  " + node_id + " -> " + child_id + ";\n");
  }

  return node_id;
}

NodeId DotVisitor::operator()(const ast::Command& command) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"COMMAND = <name=" + command.name + ">\"];\n");

  for (const auto& argument : command.arguments) {
    NodeId child_id = (*this)(argument);
    AppendToOutput("  " + node_id + " -> " + child_id + ";\n");
  }

  return node_id;
}

NodeId DotVisitor::operator()(const ast::PlainText& plain_text) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"PLAIN_TEXT = <" + EscapeForDot(plain_text.text) + ">\"];\n");

  return node_id;
}

NodeId DotVisitor::GenerateNodeId() {
  return "node_" + std::to_string(counter_.GetValueAndIncrease());
}

void DotVisitor::AppendToOutput(const std::string& s) {
  if (!output_) {
    return;
  }

  *output_ += s;
}

}  // namespace visitor
}  // namespace lightex
