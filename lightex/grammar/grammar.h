#pragma once

#include <string>

#include <lightex/ast/ast.h>
#include <lightex/ast/ast_adapted.h>

#include <boost/spirit/home/x3.hpp>

namespace lightex {
namespace grammar {

namespace x3 = boost::spirit::x3;

x3::rule<class ProgramNodeId, ast::ProgramNode> program_node = "program_node";
x3::rule<class ParagraphNodeId, ast::ParagraphNode> paragraph_node = "paragraph_node";
x3::rule<class ArgumentNodeId, ast::ArgumentNode> argument_node = "argument_node";

x3::rule<class ProgramId, ast::Program> program = "program";
x3::rule<class PlainTextId, ast::PlainText> plain_text = "plain_text";
x3::rule<class ParagraphId, ast::Paragraph> paragraph = "paragraph";
x3::rule<class ParagraphBreakerId, ast::ParagraphBreaker> paragraph_breaker = "paragraph_breaker";
x3::rule<class ArgumentId, ast::Argument> argument = "argument";
x3::rule<class ArgumentRefId, ast::ArgumentRef> argument_ref = "argument_ref";
x3::rule<class OuterArgumentRefId, ast::OuterArgumentRef> outer_argument_ref = "outer_argument_ref";
x3::rule<class InlinedMathTextId, ast::InlinedMathText> inlined_math_text = "inlined_math_text";
x3::rule<class MathTextId, ast::MathText> math_text = "math_text";
x3::rule<class CommandMacroId, ast::CommandMacro> command_macro = "command_macro";
x3::rule<class EnvironmentMacroId, ast::EnvironmentMacro> environment_macro = "environment_macro";
x3::rule<class CommandId, ast::Command> command = "command";
x3::rule<class UnescapedCommandId, ast::UnescapedCommand> unescaped_command = "unescaped_command";
x3::rule<class EnvironmentId, ast::Environment> environment = "environment";
x3::rule<class VerbatimEnvironmentId, ast::VerbatimEnvironment> verbatim_environment = "verbatim_environment";

const auto special_symbol = x3::char_("\\{}$&#^_%~[]");
const auto control_symbol = x3::lexeme['\\' >> special_symbol];
const auto unicode_symbol = x3::lexeme[x3::char_('&') >> +x3::alpha >> x3::char_(';')];
const auto special_command_identifier = x3::lit("begin") | "end" | "newcommand" | "newenvironment" | "unescaped";
const auto command_identifier = x3::lexeme['\\' >> (+x3::alpha - special_command_identifier)];
const auto math_text_symbol = (x3::char_ - x3::char_('$'));
const auto verbatim_environment_symbol = (x3::char_ - x3::char_('\\'));
const auto environment_identifier = x3::lexeme[+x3::alpha] - "verbatim";

const auto program_node_def = paragraph_breaker | paragraph | math_text | environment | verbatim_environment |
                              command_macro | environment_macro | argument_ref | outer_argument_ref;

const auto plain_text_def =
    x3::no_skip[unicode_symbol | (+(control_symbol | (x3::char_ - special_symbol - x3::lit('\n')))) | x3::string("\n")];

const auto paragraph_node_def = &(!x3::omit[paragraph_breaker]) >>
                                (plain_text | inlined_math_text | command | unescaped_command);

const auto argument_node_def =
    plain_text | inlined_math_text | command | unescaped_command | argument_ref | outer_argument_ref;

const auto program_def = *program_node;

const auto paragraph_def = +paragraph_node;

const auto paragraph_breaker_def = x3::omit[x3::skip(x3::lit(' '))[x3::repeat(2, x3::inf)[x3::lit('\n')]]];

const auto argument_def = *argument_node;

const auto argument_ref_def = x3::lexeme[x3::lit('#') >> x3::int_];

const auto outer_argument_ref_def = x3::lexeme[x3::lit("##") >> x3::int_];

const auto inlined_math_text_def = x3::lit('$') >> x3::no_skip[+math_text_symbol] >> '$';

const auto math_text_def = x3::lit("$$") >> x3::no_skip[+math_text_symbol] >> "$$";

const auto command_macro_def = x3::lit("\\newcommand") >> '{' >> command_identifier >> '}' >>
                               -('[' >> x3::int_ >> ']' >> *('[' >> argument >> ']')) >> '{' >> argument >> '}';

const auto environment_macro_def = x3::lit("\\newenvironment") >> '{' >> environment_identifier >> '}' >>
                                   -('[' >> x3::int_ >> ']' >> *('[' >> argument >> ']')) >> '{' >> program >> '}' >>
                                   '{' >> program >> '}';

const auto command_def = command_identifier >> *('[' >> argument >> ']') >> *('{' >> argument >> '}');

const auto unescaped_command_def = x3::lit("\\unescaped") >> '{' >> argument >> '}';

const auto environment_def = x3::lit("\\begin") >> '{' >> environment_identifier >> '}' >> *('[' >> argument >> ']') >>
                             *('{' >> argument >> '}') >> program >> "\\end" >> '{' >> environment_identifier >> '}';

const auto verbatim_environment_def =
    x3::lit("\\begin") >> '{' >> "verbatim" >> '}' >> program >> "\\end" >> '{' >> "verbatim" >> '}';

BOOST_SPIRIT_DEFINE(program_node)
BOOST_SPIRIT_DEFINE(paragraph_node)
BOOST_SPIRIT_DEFINE(argument_node)

BOOST_SPIRIT_DEFINE(program)
BOOST_SPIRIT_DEFINE(plain_text)
BOOST_SPIRIT_DEFINE(paragraph)
BOOST_SPIRIT_DEFINE(paragraph_breaker)
BOOST_SPIRIT_DEFINE(argument)
BOOST_SPIRIT_DEFINE(argument_ref)
BOOST_SPIRIT_DEFINE(outer_argument_ref)
BOOST_SPIRIT_DEFINE(inlined_math_text)
BOOST_SPIRIT_DEFINE(math_text)
BOOST_SPIRIT_DEFINE(command_macro)
BOOST_SPIRIT_DEFINE(environment_macro)
BOOST_SPIRIT_DEFINE(command)
BOOST_SPIRIT_DEFINE(unescaped_command)
BOOST_SPIRIT_DEFINE(environment)
BOOST_SPIRIT_DEFINE(verbatim_environment)

}  // namespace grammar
}  // namespace lightex
