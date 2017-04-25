#pragma once

#include <iostream>
#include <string>

namespace lightex {

bool ParseProgramToDot(const std::string& input, std::size_t* failed_at, std::string* output);

}  // namespace lightex
