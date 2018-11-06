//
// Created by igudich on 11/6/18.
//

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>

#include "ArgumentsParsing.h"

void parse_integer_string_array(std::vector<size_t> &numbers,
                                const std::string &str) {
  std::vector<std::string> subStrings;
  boost::algorithm::split(subStrings, str, boost::algorithm::is_any_of(","));
  for (const auto &p : subStrings) {
    numbers.push_back(std::stol(p));
  }
}

void parse_float_string_array(std::vector<float> &numbers,
                              const std::string &str) {
  std::vector<std::string> subStrings;
  boost::algorithm::split(subStrings, str, boost::algorithm::is_any_of(","));
  for (const auto &p : subStrings) {
    numbers.push_back(std::stof(p));
  }
}
