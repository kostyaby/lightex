#pragma once

#include <list>
#include <string>

#include <lightex/ast/ast.h>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/optional/optional.hpp>

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::Program, (std::list<lightex::ast::ProgramNode>, nodes))

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::PlainText, (std::string, text))

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::Paragraph, (std::list<lightex::ast::ParagraphNode>, nodes))

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::ParagraphBreaker)

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::Argument, (std::list<lightex::ast::ArgumentNode>, nodes))

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::ArgumentRef, (int, argument_id))

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::OuterArgumentRef, (int, argument_id))

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::InlinedMathText, (std::string, text))

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::MathText, (std::string, text))

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::CommandMacro,
                          (std::string, name),
                          (boost::optional<int>, arguments_num),
                          (std::list<lightex::ast::Argument>, default_arguments),
                          (lightex::ast::Argument, body))

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::EnvironmentMacro,
                          (std::string, name),
                          (boost::optional<int>, arguments_num),
                          (std::list<lightex::ast::Argument>, default_arguments),
                          (lightex::ast::Program, pre_program),
                          (lightex::ast::Program, post_program))

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::Command,
                          (std::string, name),
                          (std::list<lightex::ast::Argument>, default_arguments),
                          (std::list<lightex::ast::Argument>, arguments))

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::UnescapedCommand, (lightex::ast::Argument, body))

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::Environment,
                          (std::string, name),
                          (std::list<lightex::ast::Argument>, default_arguments),
                          (std::list<lightex::ast::Argument>, arguments),
                          (lightex::ast::Program, program),
                          (std::string, end_name))

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::VerbatimEnvironment, (std::string, content))
