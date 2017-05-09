#include <fstream>
#include <iostream>
#include <string>

#include <lightex/lightex.h>
#include <lightex/utils/file_utils.h>

int main(int argc, char** argv) {
  char const* input_file;
  char const* output_file;
  if (argc == 3) {
    input_file = argv[1];
    output_file = argv[2];
  } else {
    std::cerr << "Error: invalid number of arguments! (Expected 2, got " << std::to_string(argc - 1) << ")"
              << std::endl;
    return 1;
  }

  std::string storage;
  if (!lightex::utils::ReadDataFromFile(input_file, &storage)) {
    return 1;
  }

  lightex::WorkspaceId workspace_id = "1";
  std::string error_message;
  if (!lightex::PreloadStyleFileIntoWorkspace(workspace_id, "lightex/styles/lightex.sty", &error_message)) {
    std::cerr << "Error: failed to preload style file!" << std::endl;
    std::cerr << error_message << std::endl;
    return 1;
  }

  std::string result;
  if (!lightex::ParseProgramToHtml(storage, workspace_id, &error_message, &result)) {
    std::cerr << "Error: failed to parse input!" << std::endl;
    std::cerr << error_message << std::endl;
    return 1;
  }

  {
    std::ofstream out(output_file);
    if (!out) {
      std::cerr << "Error: failed to open output file for writing: " << input_file << std::endl;
      return 1;
    }
    out << result;
  }

  return 0;
}
