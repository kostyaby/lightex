#pragma once

#include <iostream>
#include <string>

namespace lightex {

bool ParseProgramToDot(const std::string& input, std::string* failed_snippet, std::string* output);

}  // namespace lightex
