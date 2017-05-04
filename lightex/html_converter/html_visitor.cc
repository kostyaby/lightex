#include <lightex/html_converter/html_visitor.h>

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
}  // namespace

Result Result::Failure(const std::string& error_message) {
  return {false, error_message};
}

Result Result::Success(const std::string& html_text) {
  return {true, html_text};
}

Result HtmlVisitor::operator()(const ast::Program& program) {
  std::string html_text = "";

  for (const auto& node : program.nodes) {
    Result child_result = boost::apply_visitor(*this, node);
    if (!child_result.is_successful) {
      return child_result;
    }

    html_text += child_result.value;
  }

  if (is_paragraph_open.back()) {
    html_text += "</p>";
  }

  return Result::Success(html_text);
}

Result HtmlVisitor::operator()(const ast::ParagraphBreaker& paragraph_breaker) {
  return Result::Failure("ParagraphBreaker node parser is not implemented yet.");
}

Result HtmlVisitor::operator()(const std::string& plain_text) {
  std::string html_text = "";

  int consecutive_new_lines = 0;
  int consecutive_spaces = 0;
  for (char c : plain_text) {
    if (c == '\n') {
      ++consecutive_new_lines;
      continue;
    } else if (c == ' ') {
      ++consecutive_spaces;
      continue;
    }

    if (consecutive_new_lines > 0) {
      if (is_paragraphical_program.back() && consecutive_new_lines > 1) {
        html_text += "</p>";
        is_paragraph_open.back() = false;
      } else {
        consecutive_spaces = 1;
      }
      consecutive_new_lines = 0;
    }

    if (consecutive_spaces > 0) {
      if (is_paragraph_open.back()) {
        html_text += ' ';
      }
      consecutive_spaces = 0;
    }

    if (is_paragraphical_program.back() && !is_paragraph_open.back()) {
      html_text += "<p>";
      is_paragraph_open.back() = true;
    }

    html_text += EscapeCharForHtml(c);
  }
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

Result HtmlVisitor::operator()(const ast::TabularEnvironment& tabular_environment) {
  return Result::Failure("TabularEnvironment node parser is not implemented yet.");
}

Result HtmlVisitor::operator()(const ast::Environment& environment) {
  return Result::Failure("Environment node parser is not implemented yet.");
}
}  // namespace html_converter
}  // namespace lightex
