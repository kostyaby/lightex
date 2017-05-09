#include <lightex/lightex.h>

#include <map>
#include <mutex>

#include <lightex/ast/ast.h>
#include <lightex/dot_converter/dot_visitor.h>
#include <lightex/html_converter/html_preprocessor.h>
#include <lightex/html_converter/html_visitor.h>
#include <lightex/grammar/grammar.h>
#include <lightex/utils/comment_remover.h>
#include <lightex/utils/file_utils.h>

#include <boost/spirit/home/x3.hpp>
#include <boost/variant/static_visitor.hpp>

namespace lightex {
namespace {

const char kSyntaxParsingError[] = "Error while running syntax analysis! Failed on the following snippet: ";
const int kFailedSnippetLength = 20;

namespace x3 = boost::spirit::x3;

struct Workspace {
  html_converter::HtmlVisitor visitor;
  std::mutex mtx;
};

std::map<WorkspaceId, Workspace> workspace_manager;
std::mutex manager_mtx;

bool ParseProgramToAst(const std::string& input, std::string* error_message, ast::Program* output) {
  if (!output) {
    return false;
  }

  std::string preprocessed_input = html_converter::PreprocessHtmlInput(utils::RemoveCommentsFromProgram(input));

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

bool GeneralParseProgramToHtml(const std::string& input, const WorkspaceId& workspace_id, bool preload, std::string* error_message, std::string* output) {
  ast::Program ast;
  if (!ParseProgramToAst(input, error_message, &ast)) {
    return false;
  }

  Workspace* workspace_ptr;
  std::unique_lock<std::mutex> workspace_lock;
  {
    std::lock_guard<std::mutex> manager_lock(manager_mtx);
    workspace_ptr = &workspace_manager[workspace_id];
    workspace_lock = std::unique_lock<std::mutex>(workspace_ptr->mtx);
  }

  html_converter::HtmlVisitor visitor_copy = workspace_ptr->visitor;
  html_converter::Result result = visitor_copy(ast);
  if (preload) {
    workspace_ptr->visitor = visitor_copy;
  }
  if (!result.is_successful) {
    if (error_message) {
      *error_message = result.error_message;
    }
    return false;
  }

  if (output) {
    *output = result.escaped;
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

bool PreloadStyleFileIntoWorkspace(const WorkspaceId& workspace_id, const std::string& style_file_path, std::string* error_message) {
  std::string storage;
  if (!lightex::utils::ReadDataFromFile(style_file_path, &storage)) {
    return false;
  }

  return GeneralParseProgramToHtml(storage, workspace_id, true, error_message, nullptr);
}

bool ParseProgramToHtml(const std::string& input, const WorkspaceId& workspace_id, std::string* error_message, std::string* output) {
  if (!output) {
    return false;
  }

  return GeneralParseProgramToHtml(input, workspace_id, false, error_message, output);
}

}  // namespace lightex
