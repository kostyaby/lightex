#define BOOST_TEST_MAIN

#include <lightex/workspace.h>

#include <boost/test/unit_test.hpp>

namespace {
  class Tester {
    public:
      Tester()
        : workspace_(lightex::MakeHtmlWorkspace()) {}

      void check(const std::string& tex_input, const std::string& expected_output) const {
        std::string error_message;
        std::string output;
        const bool result = workspace_->ParseProgram(tex_input, &error_message, &output);
        BOOST_CHECK(result);
        if (result) {
          BOOST_CHECK_EQUAL(output, expected_output);
        }
      }

      void fail(const std::string& tex_input) const {
        std::string error_message;
        std::string output;
        const bool result = workspace_->ParseProgram(tex_input, &error_message, &output);
        BOOST_CHECK(!result);
      }

    private:
      std::shared_ptr<lightex::Workspace> workspace_;
  };
}

BOOST_AUTO_TEST_CASE(TestSimple) {
  Tester t;
  t.check("", "");
  t.check(" ", " ");
  t.check("hello", "<p>hello</p>");
  t.check("hello\n\n", "<p>hello</p>");
  t.check("hello world", "<p>hello world</p>");
  t.check("hello  world", "<p>hello world</p>");
  t.check("hello\n\nworld", "<p>hello</p><p>world</p>");
  t.check("hello\n\n\nworld", "<p>hello</p><p>world</p>");
  t.check("hello\n\n\n\nworld", "<p>hello</p><p>world</p>");
}

BOOST_AUTO_TEST_CASE(TestComments) {
  Tester t;
  t.check("% this is a comment", "");
  t.check("\\% this is a percent sign", "<p>% this is a percent sign</p>");

  t.check("a%\nb", "<p>ab</p>");
  t.check("a %\nb", "<p>a b</p>");
  t.check("a  %\nb", "<p>a b</p>");
  t.check("a %\n b", "<p>a  b</p>");
  t.check("a     %\n       b", "<p>a  b</p>");
  t.check("a%     \nb", "<p>ab</p>");
  t.check("a%     \nb%cd", "<p>ab</p>");
  t.check("a%%%%%\\%%%%%%%%\\%", "<p>a</p>");

  t.check("a%%\\%\\\\%%b\nc", "<p>ac</p>");

  t.check("Lorem ipsum% comment starts\ndolor sit amet", "<p>Lorem ipsumdolor sit amet</p>");

  t.check(
    "Lorem ipsum dolor sit amet, % some comment\n"
    "% more comment\n"
    "% and more\n"
    "quis impetus adolescens ei vim, eu quo summo percipit scribentur.",
    "<p>Lorem ipsum dolor sit amet, quis impetus adolescens ei vim, eu quo summo percipit scribentur.</p>"
  );
}

BOOST_AUTO_TEST_CASE(TestEscaping) {
  Tester t;
  t.check("<", "<p>&lt;</p>");
  t.check(">", "<p>&gt;</p>");
  t.check("'", "<p>&apos;</p>");
  t.check("\"", "<p>&quot;</p>");
  t.check("\\&", "<p>&amp;</p>");
  t.fail("&");
  t.fail("\\");
  t.fail("\\a");
  t.check("\\\\", "<p>\\</p>");
}
