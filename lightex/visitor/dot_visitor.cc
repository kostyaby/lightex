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

NodeId DotVisitor::operator()(const ast::CommandMacro& command_macro) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"COMMAND_macro = <name=" + command_macro.name);
  AppendToOutput(" argument=");
  AppendToOutput(std::to_string(command_macro.arguments_num.value_or(0)));
  AppendToOutput(">\"];\n");

  for (const auto& argument : command_macro.default_arguments) {
    NodeId child_id = (*this)(argument);
    AppendToOutput("  " + node_id + " -> " + child_id + " [style=dotted];\n");
  }

  NodeId child_id = (*this)(command_macro.program);
  AppendToOutput("  " + node_id + " -> " + child_id + ";\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::EnvironmentMacro& environment_macro) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"ENVIRONMENT_macro = <name=" + environment_macro.name);
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

NodeId DotVisitor::operator()(const ast::OuterArgumentRef& outer_argument_ref) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"OUTER_ARGUMENT_REF = <argument_id=");
  AppendToOutput(std::to_string(outer_argument_ref.argument_id));
  AppendToOutput(">\"];\n");

  return node_id;
}

NodeId DotVisitor::operator()(const ast::ArgumentRef& argument_ref) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"ARGUMENT_REF = <argument_id=");
  AppendToOutput(std::to_string(argument_ref.argument_id));
  AppendToOutput(">\"];\n");

  return node_id;
}

NodeId DotVisitor::operator()(const std::string& plain_text) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"PLAIN_TEXT = <" + EscapeForDot(plain_text) + ">\"];\n");

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

NodeId DotVisitor::operator()(const ast::TabularEnvironment& tabular_environment) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"TABULAR_ENVIRONMENT = <column_configuration=" +
                 tabular_environment.column_configuration + ">\"];\n");

  for (const auto& content : tabular_environment.content) {
    NodeId child_id = (*this)(content);
    AppendToOutput("  " + node_id + " -> " + child_id + ";\n");
  }

  return node_id;
}

NodeId DotVisitor::operator()(const ast::Environment& environment) {
  NodeId node_id = GenerateNodeId();
  AppendToOutput("  " + node_id + " [label=\"ENVIRONMENT = <begin_name=" + environment.begin_name);
  AppendToOutput(" end_name=" + environment.end_name + ">\"];\n");

  for (const auto& argument : environment.default_arguments) {
    NodeId child_id = (*this)(argument);
    AppendToOutput("  " + node_id + " -> " + child_id + " [style=dotted];\n");
  }

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
