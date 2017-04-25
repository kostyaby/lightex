#pragma once

#include <lightex/ast/ast.h>
#include <lightex/ast/ast_adapted.h>

#include <boost/spirit/home/x3.hpp>

namespace lightex {
namespace grammar {

namespace x3 = boost::spirit::x3;

x3::rule<class Program, ast::Program> program = "program";
x3::rule<class ProgramNode, ast::ProgramNode> program_node = "program_node";
x3::rule<class PlainText, ast::PlainText> plain_text = "plain_text";
x3::rule<class Command, ast::Command> command = "command";
x3::rule<class MathText, ast::MathText> math_text = "math_text";

const auto special_symbol = x3::char_("\\{}$&#^_%~");
const auto control_symbol = x3::lexeme[x3::lit('\\') >> special_symbol];
const auto control_word = x3::lexeme[x3::lit('\\') >> +x3::alpha];
const auto math_text_symbol = (x3::char_ - x3::char_('$'));

const auto plain_text_def = x3::lexeme[+((x3::char_ - special_symbol - x3::space) | control_symbol)];

const auto program_def = *program_node;

const auto program_node_def = plain_text | command | math_text;

const auto command_def = control_word >> *(x3::lit('{') >> program >> x3::lit('}'));

const auto math_text_def = (x3::lit('$') >> x3::no_skip[+math_text_symbol] >> x3::lit('$')) |
                           (x3::lit("$$") >> x3::no_skip[+math_text_symbol] >> x3::lit("$$"));

BOOST_SPIRIT_DEFINE(plain_text)
BOOST_SPIRIT_DEFINE(program)
BOOST_SPIRIT_DEFINE(program_node)
BOOST_SPIRIT_DEFINE(command)
BOOST_SPIRIT_DEFINE(math_text)

}  // namespace grammar
}  // namespace lightex
