#include <lightex/html_converter/html_visitor.h>

#include <list>
#include <iomanip>
#include <sstream>

namespace lightex {
namespace html_converter {
namespace {

std::string EscapeStringForHtml(const std::string& unescaped) {
  std::ostringstream buffer;

  for (char c : unescaped) {
    switch (c) {
      case '&':
        buffer << "&amp;";
        break;

      case '\"':
        buffer << "&quot;";
        break;

      case '\'':
        buffer << "&apos;";
        break;

      case '<':
        buffer << "&lt;";
        break;

      case '>':
        buffer << "&gt;";
        break;

      default:
        buffer << c;
    }
  }

  return buffer.str();
}

std::string EscapeStringForJs(const std::string& unescaped) {
  std::ostringstream buffer;

  for (char c : unescaped) {
    if (c == '"' || c == '\\' || ('\x00' <= c && c <= '\x1f')) {
      buffer << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
    } else {
      buffer << c;
    }
  }

  return buffer.str();
}

template <typename Node, typename Visitor>
Result JoinNodeResults(const std::list<Node>& nodes, const std::string& separator, Visitor* visitor) {
  std::ostringstream buffer;

  std::string current_separator = "";
  for (const auto& node : nodes) {
    Result child_result = boost::apply_visitor(*visitor, node);
    if (!child_result.is_successful) {
      return child_result;
    }

    if (child_result.value.empty()) {
      continue;
    }

    buffer << current_separator;
    buffer << child_result.value;

    current_separator = separator;
  }

  return Result::Success(buffer.str());
};

std::string RenderMathFormula(const std::string& math_text, bool is_inlined, int* math_text_span_num) {
  std::string span_id = "mathTextSpan" + std::to_string(++(*math_text_span_num));
  std::string display_mode = is_inlined ? "false" : "true";

  std::ostringstream buffer;
  buffer << "<span id=\"" << span_id << "\"></span>";

  buffer << "<script type=\"text/javascript\">";
  buffer << "katex.render(";
  buffer << "\"" << EscapeStringForJs(math_text) << "\", ";
  buffer << "document.getElementById(\"" << span_id << "\"), {displayMode: " << display_mode << "});";
  buffer << "</script>";

  return buffer.str();
}
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
  return Result::Success(RenderMathFormula(math_text.text, true, &math_text_span_num_));
}

Result HtmlVisitor::operator()(const ast::MathText& math_text) {
  return Result::Success(RenderMathFormula(math_text.text, false, &math_text_span_num_));
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
