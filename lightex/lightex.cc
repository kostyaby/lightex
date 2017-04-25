#include <lightex/lightex.h>

#include <lightex/ast/ast.h>
#include <lightex/grammar/grammar.h>
#include <lightex/utils/comment_remover.h>
#include <lightex/visitor/dot_visitor.h>

#include <boost/spirit/home/x3.hpp>
#include <boost/variant/static_visitor.hpp>

namespace lightex {

bool ParseProgramToDot(const std::string& input, std::size_t* failed_at, std::string* output) {
  namespace x3 = boost::spirit::x3;

  std::string processed_input = utils::RemoveCommentsFromProgram(input);

  ast::Program ast;
  std::string::const_iterator start = processed_input.begin();
  std::string::const_iterator iter = start;
  std::string::const_iterator end = processed_input.end();
  bool r = x3::phrase_parse(iter, end, grammar::program, x3::space, ast);
  if (!r || iter < end) {
    if (failed_at) {
      *failed_at = iter - start;
    }
    return false;
  }

  if (output) {
    *output += "digraph d {\n";
    visitor::DotVisitor visitor(output);
    visitor(ast);
    *output += "}\n";
  }
  return true;
}

}  // namespace lightex
