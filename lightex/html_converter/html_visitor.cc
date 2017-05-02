#include <lightex/html_converter/html_visitor.h>

namespace lightex {
namespace html_converter {

HtmlVisitor::HtmlVisitor(std::string* output, std::string* error_message)
    : output_(output), error_message_(error_message) {}

bool HtmlVisitor::operator()(const ast::Program& program) {
  SetErrorMessage("Program node parser is not implemented yet.");
  return false;
}

bool HtmlVisitor::operator()(const std::string& plain_text) {
  SetErrorMessage("Plain text node parser is not implemented yet.");
  return false;
}

bool HtmlVisitor::operator()(const ast::ArgumentRef& argument_ref) {
  SetErrorMessage("ArgumentRef node parser is not implemented yet.");
  return false;
}

bool HtmlVisitor::operator()(const ast::OuterArgumentRef& outer_argument_ref) {
  SetErrorMessage("OuterArgumentRef node parser is not implemented yet.");
  return false;
}

bool HtmlVisitor::operator()(const ast::InlinedMathText& math_text) {
  SetErrorMessage("InlinedMathText node parser is not implemented yet.");
  return false;
}

bool HtmlVisitor::operator()(const ast::MathText& math_text) {
  SetErrorMessage("MathText node parser is not implemented yet.");
  return false;
}

bool HtmlVisitor::operator()(const ast::CommandMacro& command_macro) {
  SetErrorMessage("CommandMacro node parser is not implemented yet.");
  return false;
}

bool HtmlVisitor::operator()(const ast::EnvironmentMacro& environment_macro) {
  SetErrorMessage("EnvironmentMacro node parser is not implemented yet.");
  return false;
}

bool HtmlVisitor::operator()(const ast::Command& command) {
  SetErrorMessage("Command node parser is not implemented yet.");
  return false;
}

bool HtmlVisitor::operator()(const ast::TabularEnvironment& tabular_environment) {
  SetErrorMessage("TabularEnvironment node parser is not implemented yet.");
  return false;
}

bool HtmlVisitor::operator()(const ast::Environment& environment) {
  SetErrorMessage("Environment node parser is not implemented yet.");
  return false;
}

void HtmlVisitor::SetErrorMessage(const std::string& error_message) {
  if (error_message_) {
    *error_message_ = error_message;
  }
}
}  // namespace html_converter
}  // namespace lightex
