#pragma once

#include <iostream>
#include <string>

namespace lightex {

bool ParseProgramToDot(const std::string& input, std::string* error_message, std::string* output);

bool ParseProgramToHtml(const std::string& input, std::string* error_message, std::string* output);

}  // namespace lightex
