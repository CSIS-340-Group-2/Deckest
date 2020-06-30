#pragma once
#include <sstream>
#include <optional>
#include <iomanip>
#include <iostream>

template<typename T>
std::optional<double> parse_double(const T& input);
std::string           double_to_string(double d);
