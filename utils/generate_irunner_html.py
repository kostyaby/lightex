#!/usr/bin/python

import codecs
import glob
import os
import subprocess
import sys


def main():
  root_path = sys.argv[1]

  data_prefix = u'\\begin{rawproblem}{input.txt}{output.txt}\n'
  data_suffix = u'\n\\end{rawproblem}\n'

  result_template = u'''\
      <!DOCTYPE html>
      <html>
          <head>
              <meta charset="utf-8">
              <link rel="stylesheet" type="text/css" href="../irunner2.css">
              <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/KaTeX/0.7.1/katex.min.css" integrity="sha384-wITovz90syo1dJWVh32uuETPVEtGigN07tkttEqPv+uR2SE/mbQcG7ATL28aI9H0" crossorigin="anonymous">
              <script src="https://cdnjs.cloudflare.com/ajax/libs/KaTeX/0.7.1/katex.min.js" integrity="sha384-/y1Nn9+QQAipbNQWU65krzJralCnuOasHncUFXGkdwntGeSvQicrYkiUBwsgUqc1" crossorigin="anonymous"></script>
              <title>Problem {problem_id}</title>
          </head>
          <body>
              <div class="ir-problem-statement">{content}</div>
          </body>
      </html>
      '''

  for file_path in glob.glob('{}/**/*.tex'.format(root_path)):
    file = codecs.open(file_path, 'r', 'utf-8')
    data = data_prefix + file.read() + data_suffix
    file.close()

    temporary_tex_file = codecs.open('temp.tex', 'w', 'utf-8')
    temporary_tex_file.write(data)
    temporary_tex_file.close()

    exit_code = subprocess.call(['./build/parse_program_to_html', 'temp.tex', 'temp.html'])

    if exit_code == 0:
      problem_id = os.path.split(os.path.dirname(file_path))[1]
      temporary_html_file = codecs.open('temp.html', 'r', 'utf-8')
      result = result_template.format(problem_id=problem_id, content=temporary_html_file.read())
      temporary_html_file.close()

      html_file_path = '{}/lightex.html'.format(os.path.dirname(file_path))
      html_file = codecs.open(html_file_path, 'w', 'utf-8')
      html_file.write(result)
      html_file.close()

    print file_path, exit_code


if __name__ == "__main__":
  main()
