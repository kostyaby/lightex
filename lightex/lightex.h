#pragma once

#include <iostream>
#include <string>

namespace lightex {

using WorkspaceId = std::string;

bool ParseProgramToDot(const std::string& input, std::string* error_message, std::string* output);
bool PreloadStyleFileIntoWorkspace(const WorkspaceId& workspace_id, const std::string& style_file_path, std::string* error_message);
bool ParseProgramToHtml(const std::string& input,
                        const WorkspaceId& workspace_id,
                        std::string* error_message,
                        std::string* output);

}  // namespace lightex
