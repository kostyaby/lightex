#include <lightex/html_converter/html_visitor.h>

#include <list>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace lightex {
namespace html_converter {
namespace {

const char kUnescapedCommandName[] = "Unescaped";

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
      buffer << "\\u" << std::hex << std::setw(4) << std::setfill('0') << int{c};
    } else {
      buffer << c;
    }
  }

  return buffer.str();
}

template <typename Node>
Result JoinNodeResults(const std::list<Node>& nodes, const std::string& separator, HtmlVisitor* visitor) {
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
  if (active_unescaped_num_) {
    return Result::Success(plain_text);
  } else {
    return Result::Success(EscapeStringForHtml(plain_text));
  }
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
  const std::string* argument = GetArgumentByReference(argument_ref.argument_id - 1, false);
  if (!argument) {
    return Result::Failure("Invalid argument reference.");
  }

  return Result::Success(*argument);
}

Result HtmlVisitor::operator()(const ast::OuterArgumentRef& outer_argument_ref) {
  const std::string* argument = GetArgumentByReference(outer_argument_ref.argument_id - 1, true);
  if (!argument) {
    return Result::Failure("Invalid outer argument reference.");
  }

  return Result::Success(*argument);
}

Result HtmlVisitor::operator()(const ast::InlinedMathText& math_text) {
  return Result::Success(RenderMathFormula(math_text.text, true, &math_text_span_num_));
}

Result HtmlVisitor::operator()(const ast::MathText& math_text) {
  return Result::Success(RenderMathFormula(math_text.text, false, &math_text_span_num_));
}

Result HtmlVisitor::operator()(const ast::CommandMacro& command_macro) {
  if (command_macro.arguments_num.get_value_or(0) < command_macro.default_arguments.size()) {
    return Result::Failure("Invalid number of arguments for command macro " + command_macro.name + ".");
  }

  defined_command_macros_.push_back(command_macro);
  return Result::Success("");
}

Result HtmlVisitor::operator()(const ast::EnvironmentMacro& environment_macro) {
  return Result::Failure("EnvironmentMacro node parser is not implemented yet.");
}

Result HtmlVisitor::operator()(const ast::Command& command) {
  const ast::CommandMacro* command_macro_ptr = GetDefinedCommandMacro(command.name);
  if (!command_macro_ptr) {
    return Result::Failure("Command macro " + command.name + " is not defined yet.");
  }

  int default_args_num = command_macro_ptr->default_arguments.size();
  int redefined_default_args_num = command.default_arguments.size();
  if (redefined_default_args_num > default_args_num) {
    return Result::Failure("Command " + command.name + " has more default arguments than it's been defined. " +
                           "Expected " + std::to_string(default_args_num) + ", got " +
                           std::to_string(redefined_default_args_num) + ".");
  }

  int args_num = command_macro_ptr->default_arguments.size() + command.arguments.size();
  int expected_args_num = command_macro_ptr->arguments_num.get_value_or(0);
  if (args_num != expected_args_num) {
    return Result::Failure("Command " + command.name + " has invalid number of arguments. " + "Expected " +
                           std::to_string(expected_args_num) + ", got " + std::to_string(args_num) + ".");
  }

  const auto get_argument = [&](int i) {
    const auto list_at = [](const std::list<ast::Argument>& arguments, int index) {
      return *std::next(arguments.begin(), index);
    };

    if (i < redefined_default_args_num) {
      return list_at(command.default_arguments, i);
    } else if (i < default_args_num) {
      return list_at(command_macro_ptr->default_arguments, i);
    } else {
      return list_at(command.arguments, i - default_args_num);
    }
  };

  // A little dirty hack: in order not to escape symbols sometimes, we introduce an \Unescaped command macros and
  // check if this is it while generating command arguments.
  int active_unescaped_num_delta = 0;
  if (command.name == kUnescapedCommandName) {
    active_unescaped_num_delta = 1;
  }

  std::vector<std::string> args(args_num);
  for (int i = 0; i < args_num; ++i) {
    active_unescaped_num_ += active_unescaped_num_delta;
    Result child_result = (*this)(get_argument(i));
    active_unescaped_num_ -= active_unescaped_num_delta;
    if (!child_result.is_successful) {
      return child_result;
    }

    args[i] = std::move(child_result.value);
  }

  PushArgumentVector(std::move(args));
  Result result = (*this)(command_macro_ptr->body);
  PopArgumentVector();

  return result;
}

Result HtmlVisitor::operator()(const ast::Environment& environment) {
  return Result::Failure("Environment node parser is not implemented yet.");
}

const ast::CommandMacro* HtmlVisitor::GetDefinedCommandMacro(const std::string& name) const {
  for (auto it = defined_command_macros_.rbegin(); it != defined_command_macros_.rend(); ++it) {
    if (it->name == name) {
      return &(*it);
    }
  }

  return nullptr;
}

const std::string* HtmlVisitor::GetArgumentByReference(int index, bool is_outer) const {
  if (index < 0) {
    return nullptr;
  }

  int argument_vector_index = static_cast<int>(argument_vector_stack_.size()) - (is_outer ? 2 : 1);
  if (argument_vector_index < 0 || argument_vector_stack_[argument_vector_index].size() <= index) {
    return nullptr;
  }

  return &argument_vector_stack_[argument_vector_index][index];
}

void HtmlVisitor::PushArgumentVector(std::vector<std::string> argument_vector) {
  argument_vector_stack_.push_back(std::move(argument_vector));
}

void HtmlVisitor::PopArgumentVector() {
  argument_vector_stack_.pop_back();
}
}  // namespace html_converter
}  // namespace lightex
