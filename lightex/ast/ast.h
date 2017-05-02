#pragma once

#include <list>
#include <string>

#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

// #define BOOST_SPIRIT_X3_DEBUG

namespace lightex {
namespace ast {

namespace x3 = boost::spirit::x3;

struct Program;
struct ArgumentRef;
struct OuterArgumentRef;
struct InlinedMathText;
struct MathText;
struct CommandMacro;
struct EnvironmentMacro;
struct Command;
struct TabularEnvironment;
struct Environment;

struct ProgramNode : x3::variant<std::string,
                                 x3::forward_ast<ArgumentRef>,
                                 x3::forward_ast<OuterArgumentRef>,
                                 x3::forward_ast<InlinedMathText>,
                                 x3::forward_ast<MathText>,
                                 x3::forward_ast<CommandMacro>,
                                 x3::forward_ast<EnvironmentMacro>,
                                 x3::forward_ast<Command>,
                                 x3::forward_ast<TabularEnvironment>,
                                 x3::forward_ast<Environment>> {
  using base_type::base_type;
  using base_type::operator=;
};

struct Program : x3::position_tagged {
  std::list<ProgramNode> nodes;
};

struct ArgumentRef : x3::position_tagged {
  int argument_id;
};

struct OuterArgumentRef : x3::position_tagged {
  int argument_id;
};

struct InlinedMathText : x3::position_tagged {
  std::string text;
};

struct MathText : x3::position_tagged {
  std::string text;
};

struct CommandMacro : x3::position_tagged {
  std::string name;
  boost::optional<int> arguments_num;
  std::list<Program> default_arguments;
  Program program;
};

struct EnvironmentMacro : x3::position_tagged {
  std::string name;
  boost::optional<int> arguments_num;
  std::list<Program> default_arguments;
  Program pre_program;
  Program post_program;
};

struct Command : x3::position_tagged {
  std::string name;
  std::list<Program> default_arguments;
  std::list<Program> arguments;
};

struct TabularEnvironment : x3::position_tagged {
  std::string column_configuration;
  std::list<std::string> content;
};

struct Environment : x3::position_tagged {
  std::string begin_name;
  std::list<Program> default_arguments;
  std::list<Program> arguments;
  Program body;
  std::string end_name;
};

}  // namespace ast
}  // namespace lightex
