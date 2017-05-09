#include <lightex/utils/file_utils.h>

#include <fstream>
#include <iostream>
#include <iterator>

namespace lightex {
namespace utils {

bool ReadDataFromFile(const std::string& path, std::string* output) {
  if (!output) {
    std::cerr << "Error: output pointer points to invalid memory." << std::endl;
    return false;
  }

  std::ifstream in(path);
  if (!in) {
    std::cerr << "Error: failed to open input file for reading: " << path << "." << std::endl;
    return false;
  }

  in.unsetf(std::ios::skipws);  // Avoids white space skipping.
  std::copy(std::istream_iterator<char>(in), std::istream_iterator<char>(), std::back_inserter(*output));

  return true;
}
}  // namespace utils
}  // namespace lightex
