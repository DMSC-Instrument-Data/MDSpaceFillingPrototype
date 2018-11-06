//
// Created by igudich on 11/6/18.
//

#ifndef MD_WORKSPACE_STRUCTURE_ARGUMENTSPARSING_H
#define MD_WORKSPACE_STRUCTURE_ARGUMENTSPARSING_H

#include <vector>
#include <string>
#include <gflags/gflags.h>

void parse_integer_string_array(std::vector<size_t> &numbers, const std::string &str);
void parse_float_string_array(std::vector<float> &numbers, const std::string &str);
#endif //MD_WORKSPACE_STRUCTURE_ARGUMENTSPARSING_H
