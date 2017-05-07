#include <lightex/dot_converter/dot_visitor.h>

namespace lightex {
namespace dot_converter {
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

DotVisitor::DotVisitor(std::string* output) : output_(output), next_node_id_(0) {}

NodeId DotVisitor::operator()(const ast::Program& program) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"PROGRAM\"];\n");

  for (const auto& program_node : program.nodes) {
    NodeId child_id = boost::apply_visitor(*this, program_node);
    AppendToOutput("  " + node_id + " -> " + child_id + ";\n");
  }

  return node_id;
}

NodeId DotVisitor::operator()(const ast::PlainText& plain_text) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"PLAIN_TEXT = <" + EscapeForDot(plain_text.text) + ">\"];\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::Paragraph& paragraph) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"PARAGRAPH\"];\n");

  for (const auto& paragraph_node : paragraph.nodes) {
    NodeId child_id = boost::apply_visitor(*this, paragraph_node);
    AppendToOutput("  " + node_id + " -> " + child_id + ";\n");
  }

  return node_id;
}

NodeId DotVisitor::operator()(const ast::ParagraphBreaker& paragraph_breaker) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"PARAGRAPH_BREAKER\"];\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::Argument& argument) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"ARGUMENT\"];\n");

  for (const auto& argument_node : argument.nodes) {
    NodeId child_id = boost::apply_visitor(*this, argument_node);
    AppendToOutput("  " + node_id + " -> " + child_id + ";\n");
  }

  return node_id;
}

NodeId DotVisitor::operator()(const ast::ArgumentRef& argument_ref) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"ARGUMENT_REF = <argument_id=");
  AppendToOutput(std::to_string(argument_ref.argument_id));
  AppendToOutput(">\"];\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::OuterArgumentRef& outer_argument_ref) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"OUTER_ARGUMENT_REF = <argument_id=");
  AppendToOutput(std::to_string(outer_argument_ref.argument_id));
  AppendToOutput(">\"];\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::InlinedMathText& math_text) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"INLINED_MATH_TEXT = <" + EscapeForDot(math_text.text) + ">\"];\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::MathText& math_text) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"MATH_TEXT = <" + EscapeForDot(math_text.text) + ">\"];\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::CommandMacro& command_macro) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"COMMAND_MACRO = <name=" + command_macro.name);
  AppendToOutput(" argument=");
  AppendToOutput(std::to_string(command_macro.arguments_num.value_or(0)));
  AppendToOutput(">\"];\n");

  for (const auto& argument : command_macro.default_arguments) {
    NodeId child_id = (*this)(argument);
    AppendToOutput("  " + node_id + " -> " + child_id + " [style=dotted];\n");
  }

  NodeId child_id = (*this)(command_macro.body);
  AppendToOutput("  " + node_id + " -> " + child_id + ";\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::EnvironmentMacro& environment_macro) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"ENVIRONMENT_MACRO = <name=" + environment_macro.name);
  AppendToOutput(" argument=");
  AppendToOutput(std::to_string(environment_macro.arguments_num.value_or(0)));
  AppendToOutput(">\"];\n");

  for (const auto& argument : environment_macro.default_arguments) {
    NodeId child_id = (*this)(argument);
    AppendToOutput("  " + node_id + " -> " + child_id + " [style=dotted];\n");
  }

  NodeId pre_child_id = (*this)(environment_macro.pre_program);
  AppendToOutput("  " + node_id + " -> " + pre_child_id + " [color=red];\n");

  NodeId post_child_id = (*this)(environment_macro.post_program);
  AppendToOutput("  " + node_id + " -> " + post_child_id + " [color=blue];\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::Command& command) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"COMMAND = <name=" + command.name + ">\"];\n");

  for (const auto& argument : command.default_arguments) {
    NodeId child_id = (*this)(argument);
    AppendToOutput("  " + node_id + " -> " + child_id + " [style=dotted];\n");
  }

  for (const auto& argument : command.arguments) {
    NodeId child_id = (*this)(argument);
    AppendToOutput("  " + node_id + " -> " + child_id + ";\n");
  }

  return node_id;
}

NodeId DotVisitor::operator()(const ast::UnescapedCommand& unescaped_command) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"UNESCAPED_COMMAND\"];\n");

  NodeId child_id = (*this)(unescaped_command.body);
  AppendToOutput("  " + node_id + " -> " + child_id + ";\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::Environment& environment) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"ENVIRONMENT = <name=" + environment.name);
  AppendToOutput(" end_name=" + environment.end_name + ">\"];\n");

  for (const auto& argument : environment.default_arguments) {
    NodeId child_id = (*this)(argument);
    AppendToOutput("  " + node_id + " -> " + child_id + " [style=dotted];\n");
  }

  for (const auto& argument : environment.arguments) {
    NodeId child_id = (*this)(argument);
    AppendToOutput("  " + node_id + " -> " + child_id + " [style=dotted];\n");
  }

  NodeId body_child_id = (*this)(environment.program);
  AppendToOutput("  " + node_id + " -> " + body_child_id + ";\n");

  return node_id;
}

NodeId DotVisitor::GenerateNodeId() {
  return "node_" + std::to_string(next_node_id_++);
}

void DotVisitor::AppendToOutput(const std::string& s) {
  if (!output_) {
    return;
  }

  *output_ += s;
}

}  // namespace dot_converter
}  // namespace lightex
