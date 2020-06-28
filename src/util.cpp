#include "util.hpp"

template<typename T>
std::optional<double> parse_double(const T& input) {
  std::optional<double> res;
  try {
    res = std::stod(input);
  } catch (std::exception& e) {}

  return res;
}
std::string double_to_string(double d) {
  // Because of course the easiest way to convert a double to a string in C++
  // requires allocating an extra object
  std::stringstream ss;
  ss << std::fixed << std::setprecision(2);
  ss << d;
  return ss.str();
}
