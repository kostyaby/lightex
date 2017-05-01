#pragma once

#include <string>

#include <lightex/ast/ast.h>
#include <lightex/ast/ast_adapted.h>

#include <boost/spirit/home/x3.hpp>

namespace lightex {
namespace grammar {

namespace x3 = boost::spirit::x3;

x3::rule<class ProgramId, ast::Program> program = "program";
x3::rule<class ProgramNodeId, ast::ProgramNode> program_node = "program_node";
x3::rule<class CommandMacroId, ast::CommandMacro> command_macro = "command_macro";
x3::rule<class EnvironmentMacroId, ast::EnvironmentMacro> environment_macro = "environment_macro";
x3::rule<class ArgumentRefId, ast::ArgumentRef> argument_ref = "argument_ref";
x3::rule<class OuterArgumentRefId, ast::OuterArgumentRef> outer_argument_ref = "outer_argument_ref";
x3::rule<class CommandId, ast::Command> command = "command";
x3::rule<class InlinedMathTextId, ast::InlinedMathText> inlined_math_text = "inlined_math_text";
x3::rule<class MathTextId, ast::MathText> math_text = "math_text";
x3::rule<class TabularEnvironmentId, ast::TabularEnvironment> tabular_environment = "tabular_environment";
x3::rule<class EnvironmentId, ast::Environment> environment = "environment";

const auto special_symbol = x3::char_("\\{}$&#^_%~[]");
const auto control_symbol = x3::lexeme[x3::lit('\\') >> special_symbol];
const auto control_word = x3::lexeme[x3::lit('\\') >> +x3::alpha] - x3::lit("\\begin") - x3::lit("\\end") -
                          x3::lit("\\newcommand") - x3::lit("\\newenvironment");
const auto math_text_symbol = (x3::char_ - x3::char_('$'));
const auto environment_identifier = x3::lexeme[+x3::alpha] - "tabular";
const auto plain_text = x3::no_skip[+(control_symbol | (x3::char_ - special_symbol))];

const auto program_def = *program_node;

const auto program_node_def = plain_text | argument_ref | outer_argument_ref | inlined_math_text | math_text |
                              command_macro | environment_macro | command | tabular_environment | environment;

const auto command_macro_def =
    x3::lit("\\newcommand{") >> control_word >> x3::lit('}') >>
    -(x3::lit('[') >> x3::int_ >> x3::lit(']') >> *(x3::lit('[') >> program >> x3::lit(']'))) >> x3::lit('{') >> program
    >> x3::lit('}');

const auto environment_macro_def =
    x3::lit("\\newenvironment{") >> environment_identifier >> x3::lit('}') >>
    -(x3::lit('[') >> x3::int_ >> x3::lit(']') >> *(x3::lit('[') >> program >> x3::lit(']'))) >> x3::lit('{') >> program
    >> x3::lit('}') >> x3::lit('{') >> program >> x3::lit('}');

const auto argument_ref_def = x3::lexeme[x3::lit('#') >> x3::int_];

const auto outer_argument_ref_def = x3::lexeme[x3::lit("##") >> x3::int_];

const auto command_def = control_word >> *(x3::lit('[') >> program >> x3::lit(']')) >>
                         *(x3::lit('{') >> program >> x3::lit('}'));

const auto inlined_math_text_def = x3::lit('$') >> x3::no_skip[+math_text_symbol] >> x3::lit('$');

const auto math_text_def = x3::lit("$$") >> x3::no_skip[+math_text_symbol] >> x3::lit("$$");

const auto tabular_environment_def =
    x3::lit("\\begin{tabular}") >> x3::lit('{') >> *x3::char_("lrc|") >> x3::lit('}') >>
    *(x3::string("\\hline") | x3::string("&") | x3::string("\\\\") | plain_text) >> x3::lit("\\end{tabular}");

const auto environment_def = x3::lit("\\begin{") >> environment_identifier >> x3::lit('}') >>
                             *(x3::lit('[') >> program >> x3::lit(']')) >>
                             *(x3::lit('{') >> program >> x3::lit('}')) >> program
                             >> x3::lit("\\end{") >> environment_identifier >> x3::lit('}');

BOOST_SPIRIT_DEFINE(program)
BOOST_SPIRIT_DEFINE(program_node)
BOOST_SPIRIT_DEFINE(command_macro)
BOOST_SPIRIT_DEFINE(environment_macro)
BOOST_SPIRIT_DEFINE(argument_ref)
BOOST_SPIRIT_DEFINE(outer_argument_ref)
BOOST_SPIRIT_DEFINE(command)
BOOST_SPIRIT_DEFINE(inlined_math_text)
BOOST_SPIRIT_DEFINE(math_text)
BOOST_SPIRIT_DEFINE(environment)
BOOST_SPIRIT_DEFINE(tabular_environment)

}  // namespace grammar
}  // namespace lightex
