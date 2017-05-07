#include <lightex/html_converter/html_visitor.h>

#include <list>

namespace lightex {
namespace html_converter {
namespace {

std::string EscapeCharForHtml(char c) {
  switch (c) {
    case '&':
      return "&amp;";

    case '\"':
      return "&quot;";

    case '\'':
      return "&apos;";

    case '<':
      return "&lt;";

    case '>':
      return "&gt;";

    default: { return std::string(1, c); }
  }
}

std::string EscapeStringForHtml(const std::string& unescaped) {
  std::string result;

  for (char c : unescaped) {
    result += EscapeCharForHtml(c);
  }

  return result;
}

template<typename Node, typename Visitor>
Result JoinNodeResults(const std::list<Node>& nodes, const std::string& separator, Visitor* visitor) {
  std::string result;

  std::string current_separator = "";
  for (const auto& node : nodes) {
    Result child_result = boost::apply_visitor(*visitor, node);
    if (!child_result.is_successful) {
      return child_result;
    }

    if (child_result.value.empty()) {
      continue;
    }

    result += current_separator;
    result += child_result.value;

    current_separator = separator;
  }

  return Result::Success(result);
};
}  // namespace

Result Result::Failure(const std::string& error_message) {
  return {false, error_message};
}

Result Result::Success(const std::string& html_text) {
  return {true, html_text};
}

Result HtmlVisitor::operator()(const std::string& plain_text) {
  return Result::Success(EscapeStringForHtml(plain_text));
}

Result HtmlVisitor::operator()(const ast::Program& program) {
  return JoinNodeResults(program.nodes, "\n", this);
}

Result HtmlVisitor::operator()(const ast::Paragraph& paragraph) {
  Result result = JoinNodeResults(paragraph.nodes, " ", this);
  if (!result.is_successful) {
    return result;
  }

  if (active_macro_definitions_num_ == 0) {
    result.value = "<p>" + result.value + "</p>";
  }

  return result;
}

Result HtmlVisitor::operator()(const ast::ParagraphBreaker& paragraph_breaker) {
  return Result::Success("");
}

Result HtmlVisitor::operator()(const ast::Argument& argument) {
  return JoinNodeResults(argument.nodes, " ", this);
}

Result HtmlVisitor::operator()(const ast::ArgumentRef& argument_ref) {
  return Result::Failure("ArgumentRef node parser is not implemented yet.");
}

Result HtmlVisitor::operator()(const ast::OuterArgumentRef& outer_argument_ref) {
  return Result::Failure("OuterArgumentRef node parser is not implemented yet.");
}

Result HtmlVisitor::operator()(const ast::InlinedMathText& math_text) {
  return Result::Failure("InlinedMathText node parser is not implemented yet.");
}

Result HtmlVisitor::operator()(const ast::MathText& math_text) {
  return Result::Failure("MathText node parser is not implemented yet.");
}

Result HtmlVisitor::operator()(const ast::CommandMacro& command_macro) {
  return Result::Failure("CommandMacro node parser is not implemented yet.");
}

Result HtmlVisitor::operator()(const ast::EnvironmentMacro& environment_macro) {
  return Result::Failure("EnvironmentMacro node parser is not implemented yet.");
}

Result HtmlVisitor::operator()(const ast::Command& command) {
  return Result::Failure("Command node parser is not implemented yet.");
}

Result HtmlVisitor::operator()(const ast::Environment& environment) {
  return Result::Failure("Environment node parser is not implemented yet.");
}
}  // namespace html_converter
}  // namespace lightex
