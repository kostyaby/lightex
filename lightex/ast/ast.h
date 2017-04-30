#pragma once

#include <list>
#include <string>

#include <boost/optional/optional.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

namespace lightex {
namespace ast {

namespace x3 = boost::spirit::x3;

struct Program;
struct ProgramNode;
struct CommandDefinition;
struct ArgumentReference;
struct EnvironmentDefinition;
struct MathText;
struct Command;
struct TabularEnvironment;
struct Environment;

struct ProgramNode : x3::variant<std::string,
                                 x3::forward_ast<ArgumentReference>,
                                 x3::forward_ast<MathText>,
                                 x3::forward_ast<CommandDefinition>,
                                 x3::forward_ast<EnvironmentDefinition>,
                                 x3::forward_ast<Command>,
                                 x3::forward_ast<TabularEnvironment>,
                                 x3::forward_ast<Environment>> {
  using base_type::base_type;
  using base_type::operator=;
};

struct Program : x3::position_tagged {
  std::list<ProgramNode> nodes;
};

struct CommandDefinition : x3::position_tagged {
  std::string name;
  boost::optional<int> arguments_num;
  Program program;
};

struct ArgumentReference : x3::position_tagged {
  int argument_id;
};

struct EnvironmentDefinition : x3::position_tagged {
  std::string name;
  boost::optional<int> arguments_num;
  Program pre_program;
  Program post_program;
};

struct MathText : x3::position_tagged {
  std::string text;
};

struct Command : x3::position_tagged {
  std::string name;
  std::list<Program> arguments;
};

struct TabularEnvironment : x3::position_tagged {
  std::string column_configuration;
  std::list<std::string> content;
};

struct Environment : x3::position_tagged {
  std::string begin_name;
  std::list<Program> arguments;
  Program body;
  std::string end_name;
};

}  // namespace ast
}  // namespace lightex
