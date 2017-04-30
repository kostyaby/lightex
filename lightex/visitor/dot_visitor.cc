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

NodeId DotVisitor::operator()(const ast::CommandDefinition& command_definition) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"COMMAND_DEF = <name=" + command_definition.name);
  AppendToOutput(" argument=");
  AppendToOutput(std::to_string(command_definition.arguments_num.value_or(0)));
  AppendToOutput(">\"];\n");

  NodeId child_id = (*this)(command_definition.program);
  AppendToOutput("  " + node_id + " -> " + child_id + ";\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::EnvironmentDefinition& environment_definition) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"ENVIRONMENT_DEF = <name=" + environment_definition.name);
  AppendToOutput(" argument=");
  AppendToOutput(std::to_string(environment_definition.arguments_num.value_or(0)));
  AppendToOutput(">\"];\n");

  NodeId pre_child_id = (*this)(environment_definition.pre_program);
  AppendToOutput("  " + node_id + " -> " + pre_child_id + " [color=red];\n");

  NodeId post_child_id = (*this)(environment_definition.post_program);
  AppendToOutput("  " + node_id + " -> " + post_child_id + " [color=blue];\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::ArgumentReference& argument_reference) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"ARGUMENT_REF = <argument_id=");
  AppendToOutput(std::to_string(argument_reference.argument_id));
  AppendToOutput(">\"];\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::PlainText& plain_text) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"PLAIN_TEXT = <" + EscapeForDot(plain_text.text) + ">\"];\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::MathText& math_text) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"MATH_TEXT = <" + EscapeForDot(math_text.text) + ">\"];\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::Environment& environment) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"ENVIRONMENT = <begin_name=" + environment.begin_name);
  AppendToOutput(" end_name=" + environment.end_name + ">\"];\n");

  for (const auto& argument : environment.arguments) {
    NodeId child_id = (*this)(argument);
    AppendToOutput("  " + node_id + " -> " + child_id + " [style=dotted];\n");
  }

  NodeId body_child_id = (*this)(environment.body);
  AppendToOutput("  " + node_id + " -> " + body_child_id + ";\n");

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
