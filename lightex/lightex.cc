#include <lightex/lightex.h>

#include <lightex/ast/ast.h>
#include <lightex/dot_converter/dot_visitor.h>
#include <lightex/html_converter/html_visitor.h>
#include <lightex/grammar/grammar.h>
#include <lightex/utils/comment_remover.h>

#include <boost/spirit/home/x3.hpp>
#include <boost/variant/static_visitor.hpp>

namespace lightex {
namespace {

const char kSyntaxParsingError[] = "Error while running syntax analysis! Failed on the following snippet: ";
const int kFailedSnippetLength = 20;

namespace x3 = boost::spirit::x3;

bool ParseProgramToAst(const std::string& input, std::string* error_message, ast::Program* output) {
  if (!output) {
    return false;
  }

  std::string preprocessed_input = utils::RemoveCommentsFromProgram(input);

  std::string::const_iterator start = preprocessed_input.begin();
  std::string::const_iterator iter = start;
  std::string::const_iterator end = preprocessed_input.end();
  if (!x3::phrase_parse(iter, end, grammar::program, x3::space, *output) || iter < end) {
    if (error_message) {
      std::size_t failed_at = iter - start;
      *error_message = kSyntaxParsingError;
      *error_message = preprocessed_input.substr(failed_at, failed_at + kFailedSnippetLength);
      if (failed_at + kFailedSnippetLength + 1 < preprocessed_input.size()) {
        *error_message += "...";
      }
    }
    return false;
  }

  return true;
}
}  // namespace

bool ParseProgramToDot(const std::string& input, std::string* error_message, std::string* output) {
  if (!output) {
    return false;
  }

  ast::Program ast;
  if (!ParseProgramToAst(input, error_message, &ast)) {
    return false;
  }

  *output += "digraph d {\n";
  dot_converter::DotVisitor visitor(output);
  visitor(ast);
  *output += "}\n";

  return true;
}

bool ParseProgramToHtml(const std::string& input, std::string* error_message, std::string* output) {
  if (!output) {
    return false;
  }

  ast::Program ast;
  if (!ParseProgramToAst(input, error_message, &ast)) {
    return false;
  }

  html_converter::HtmlVisitor visitor;
  html_converter::Result result = visitor(ast);
  if (!result.is_successful) {
    if (error_message) {
      *error_message = result.value;
    }
    return false;
  }

  *output = "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"utf-8\"><title>LighTeX</title>\n"
            "<style>.tex-center p {text-align: center;}</style>\n"
            "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/KaTeX/0.7.1/katex.min.css\" "
            "integrity=\"sha384-wITovz90syo1dJWVh32uuETPVEtGigN07tkttEqPv+uR2SE/mbQcG7ATL28aI9H0\" "
            " crossorigin=\"anonymous\">\n"
            "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/KaTeX/0.7.1/katex.min.js\" "
            "integrity=\"sha384-/y1Nn9+QQAipbNQWU65krzJralCnuOasHncUFXGkdwntGeSvQicrYkiUBwsgUqc1\" "
            "crossorigin=\"anonymous\"></script>\n</head>\n<body>\n";
  *output += result.value;
  *output += "</body></html>";
  return true;
}

}  // namespace lightex
