#pragma once

#include "global.hpp"
#include "struct_question.hpp"
#include "struct_answer.hpp"

vector<string> line_extractor(string input);

command_type type_extractor(vector<string> &tokens);
command input_extractor(vector<string> &tokens);
vector<string> info_extractor(vector<string> &tokens);

void write_terminal(ANSWER input);
QUESTION read_terminal(string line);