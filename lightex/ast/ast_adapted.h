#pragma once

#include <list>
#include <string>

#include <lightex/ast/ast.h>

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::PlainText, (std::string, text))

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::Program, (std::list<lightex::ast::ProgramNode>, nodes))

// BOOST_FUSION_ADAPT_STRUCT(
//     CommandDefinition,
//     (std::string, name),
//     (int_t, arguments_num),
//     (Program, program)
// )

// BOOST_FUSION_ADAPT_STRUCT(
//     EnvironmentDefinition,
//     (std::string, name),
//     (int_t, arguments_num),
//     (Program, pre_program),
//     (Program, post_program)
// )

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::MathText, (std::string, text))

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::Command, (std::string, name), (std::list<lightex::ast::Program>, arguments))

// BOOST_FUSION_ADAPT_STRUCT(
//     TabularEnvironment,
//     (vector_t<std::string>, text)
// )

BOOST_FUSION_ADAPT_STRUCT(lightex::ast::Environment,
                          (std::string, begin_name),
                          (std::list<lightex::ast::Program>, arguments),
                          (lightex::ast::Program, body),
                          (std::string, end_name));
