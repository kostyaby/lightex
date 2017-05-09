#include <lightex/html_converter/html_visitor.h>

#include <list>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace lightex {
namespace html_converter {
namespace {

std::string FormatText(const std::string& unformatted) {
  std::ostringstream buffer;

  bool previous_is_space = false;
  for (char c : unformatted) {
    if (std::isspace(c)) {
      if (!previous_is_space) {
        buffer << ' ';
      }
      previous_is_space = true;
      continue;
    }

    buffer << c;
    previous_is_space = false;
  }

  return buffer.str();
}

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

template <typename MacroDefinition>
const MacroDefinition* GetMacroDefinition(const std::list<MacroDefinition>& macro_definitions,
                                          const std::string& name) {
  for (auto it = macro_definitions.rbegin(); it != macro_definitions.rend(); ++it) {
    if (it->name == name) {
      return &(*it);
    }
  }

  return nullptr;
}
}  // namespace

Result Result::Failure(const std::string& error_message) {
  return {false, "", "", error_message};
}

Result Result::Success(const std::string& escaped, const std::string& unescaped) {
  return {true, escaped, unescaped, ""};
}

Result HtmlVisitor::operator()(const ast::Program& program) {
  return JoinNodeResults(program.nodes);
}

Result HtmlVisitor::operator()(const ast::PlainText& plain_text) {
  return Result::Success(EscapeStringForHtml(FormatText(plain_text.text)), plain_text.text);
}

Result HtmlVisitor::operator()(const ast::Paragraph& paragraph) {
  Result result = JoinNodeResults(paragraph.nodes);
  if (!result.is_successful) {
    return result;
  }

  if (active_environment_definitions_num_ == 0) {
    result.escaped = "<p>" + result.escaped + "</p>";
    result.unescaped = "<p>" + result.unescaped + "</p>";
  }

  return result;
}

Result HtmlVisitor::operator()(const ast::ParagraphBreaker& paragraph_breaker) {
  return Result::Success("", "");
}

Result HtmlVisitor::operator()(const ast::Argument& argument) {
  return JoinNodeResults(argument.nodes);
}

Result HtmlVisitor::operator()(const ast::ArgumentRef& argument_ref) {
  const Result* argument = GetArgumentByReference(argument_ref.argument_id - 1, false);
  if (!argument) {
    return Result::Failure("Invalid argument reference.");
  }

  return *argument;
}

Result HtmlVisitor::operator()(const ast::OuterArgumentRef& outer_argument_ref) {
  const Result* argument = GetArgumentByReference(outer_argument_ref.argument_id - 1, true);
  if (!argument) {
    return Result::Failure("Invalid outer argument reference.");
  }

  return *argument;
}

Result HtmlVisitor::operator()(const ast::InlinedMathText& math_text) {
  std::string render_result = RenderMathFormula(math_text.text, true, &math_text_span_num_);
  return Result::Success(render_result, render_result);
}

Result HtmlVisitor::operator()(const ast::MathText& math_text) {
  std::string render_result = RenderMathFormula(math_text.text, false, &math_text_span_num_);
  return Result::Success(render_result, render_result);
}

Result HtmlVisitor::operator()(const ast::CommandMacro& command_macro) {
  if (command_macro.arguments_num.get_value_or(0) < command_macro.default_arguments.size()) {
    return Result::Failure("Invalid number of arguments for command macro " + command_macro.name + ".");
  }

  defined_command_macros_.push_back(command_macro);
  return Result::Success("", "");
}

Result HtmlVisitor::operator()(const ast::EnvironmentMacro& environment_macro) {
  if (environment_macro.arguments_num.get_value_or(0) < environment_macro.default_arguments.size()) {
    return Result::Failure("Invalid number of arguments for environment macro " + environment_macro.name + ".");
  }

  defined_environment_macros_.push_back(environment_macro);
  return Result::Success("", "");
}

Result HtmlVisitor::operator()(const ast::Command& command) {
  const ast::CommandMacro* command_macro_ptr = GetDefinedCommandMacro(command.name);
  if (!command_macro_ptr) {
    return Result::Failure("Command macro " + command.name + " is not defined yet.");
  }

  std::vector<Result> args;
  Result intermediate_result = PrepareMacroArguments(command, *command_macro_ptr, &args);
  if (!intermediate_result.is_successful) {
    return intermediate_result;
  }

  arguments_stack_.push_back(std::move(args));
  intermediate_result = (*this)(command_macro_ptr->body);
  arguments_stack_.pop_back();

  return intermediate_result;
}

Result HtmlVisitor::operator()(const ast::UnescapedCommand& unescaped_command) {
  Result result = (*this)(unescaped_command.body);

  if (result.is_successful) {
    result.escaped = result.unescaped;
  }

  return result;
}

Result HtmlVisitor::operator()(const ast::Environment& environment) {
  if (environment.name != environment.end_name) {
    return Result::Failure("Environment name doesn't match the end name: " + environment.name + " != " +
                           environment.end_name);
  }

  const ast::EnvironmentMacro* environment_macro_ptr = GetDefinedEnvironmentMacro(environment.name);
  if (!environment_macro_ptr) {
    return Result::Failure("Environment macro " + environment.name + " is not defined yet.");
  }

  std::vector<Result> args;
  Result intermediate_result = PrepareMacroArguments(environment, *environment_macro_ptr, &args);
  if (!intermediate_result.is_successful) {
    return intermediate_result;
  }

  std::size_t cached_defined_command_macros_num = defined_command_macros_.size();

  const auto rollback = [&]() {
    while (cached_defined_command_macros_num < defined_command_macros_.size()) {
      defined_command_macros_.pop_back();
    }

    arguments_stack_.pop_back();
  };

  arguments_stack_.push_back(std::move(args));

  std::string escaped;
  std::string unescaped;

  active_environment_definitions_num_ += 1;
  intermediate_result = (*this)(environment_macro_ptr->pre_program);
  active_environment_definitions_num_ -= 1;
  if (intermediate_result.is_successful) {
    escaped += intermediate_result.escaped;
    unescaped += intermediate_result.unescaped;
  } else {
    rollback();
    return intermediate_result;
  }

  intermediate_result = (*this)(environment.program);
  if (intermediate_result.is_successful) {
    escaped += intermediate_result.escaped;
    unescaped += intermediate_result.unescaped;
  } else {
    rollback();
    return intermediate_result;
  }

  active_environment_definitions_num_ += 1;
  intermediate_result = (*this)(environment_macro_ptr->post_program);
  active_environment_definitions_num_ -= 1;
  if (intermediate_result.is_successful) {
    escaped += intermediate_result.escaped;
    unescaped += intermediate_result.unescaped;
  } else {
    rollback();
    return intermediate_result;
  }

  rollback();
  return Result::Success(escaped, unescaped);
}

Result HtmlVisitor::operator()(const ast::VerbatimEnvironment& verbatim_environment) {
  Result result = (*this)(verbatim_environment.program);

  if (result.is_successful) {
    result.unescaped = "<pre>" + result.unescaped + "</pre>";
    result.escaped = result.unescaped;
  }

  return result;
}

template <typename Node>
Result HtmlVisitor::JoinNodeResults(const std::list<Node>& nodes) {
  std::ostringstream escaped;
  std::ostringstream unescaped;

  for (const auto& node : nodes) {
    Result child_result = boost::apply_visitor(*this, node);
    if (!child_result.is_successful) {
      return child_result;
    }

    escaped << child_result.escaped;
    unescaped << child_result.unescaped;
  }

  return Result::Success(escaped.str(), unescaped.str());
};

template <typename Macro, typename MacroDefinition>
Result HtmlVisitor::PrepareMacroArguments(const Macro& macro,
                                          const MacroDefinition& macro_definition,
                                          std::vector<Result>* output_args) {
  if (!output_args) {
    return Result::Failure("No place for preparing macro arguments is provided.");
  }

  int default_args_num = macro_definition.default_arguments.size();
  int redefined_default_args_num = macro.default_arguments.size();
  if (redefined_default_args_num > default_args_num) {
    return Result::Failure("Macro " + macro.name + " has more default arguments than it's been defined. " +
                           "Expected " + std::to_string(default_args_num) + ", got " +
                           std::to_string(redefined_default_args_num) + ".");
  }

  int args_num = macro_definition.default_arguments.size() + macro.arguments.size();
  int expected_args_num = macro_definition.arguments_num.get_value_or(0);
  if (args_num != expected_args_num) {
    return Result::Failure("Macro " + macro.name + " has invalid number of arguments. " + "Expected " +
                           std::to_string(expected_args_num) + ", got " + std::to_string(args_num) + ".");
  }

  const auto get_argument = [&](int i) {
    const auto list_at = [](const std::list<ast::Argument>& arguments, int index) {
      return *std::next(arguments.begin(), index);
    };

    if (i < redefined_default_args_num) {
      return list_at(macro.default_arguments, i);
    } else if (i < default_args_num) {
      return list_at(macro_definition.default_arguments, i);
    } else {
      return list_at(macro.arguments, i - default_args_num);
    }
  };

  output_args->resize(args_num);
  for (int i = 0; i < args_num; ++i) {
    Result child_result = (*this)(get_argument(i));
    if (!child_result.is_successful) {
      return child_result;
    }

    output_args->at(i) = std::move(child_result);
  }

  return Result::Success("", "");
}

const ast::CommandMacro* HtmlVisitor::GetDefinedCommandMacro(const std::string& name) const {
  return GetMacroDefinition(defined_command_macros_, name);
}

const ast::EnvironmentMacro* HtmlVisitor::GetDefinedEnvironmentMacro(const std::string& name) const {
  return GetMacroDefinition(defined_environment_macros_, name);
}

const Result* HtmlVisitor::GetArgumentByReference(int index, bool is_outer) const {
  // std::cerr << "Index = " << index << ", is_outer = " << is_outer << "\n";
  // for (int i = 0; i < arguments_stack_.size(); ++i) {
  //   std::cerr << i << " --> ";
  //   for (int j = 0; j < arguments_stack_[i].size(); ++j) {
  //     std::cerr << arguments_stack_[i][j] << " ";
  //   }
  //   std::cerr << "\n";
  // }

  int arguments_stack_index = static_cast<int>(arguments_stack_.size()) - (is_outer ? 2 : 1);
  if (index < 0 || arguments_stack_index < 0) {
    return nullptr;
  }
  const auto& arguments = arguments_stack_[arguments_stack_index];

  if (arguments.size() <= index) {
    return nullptr;
  }

  return &arguments[index];
}
}  // namespace html_converter
}  // namespace lightex
