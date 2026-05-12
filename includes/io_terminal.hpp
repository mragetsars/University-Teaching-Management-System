#pragma once

#include "global.hpp"
#include "struct_question.hpp"
#include "struct_answer.hpp"

struct TERMINAL_PARSE_RESULT
{
    QUESTION question;
    bool should_run;
    response parse_error;
};

vector<string> line_extractor(string input);
command_type type_extractor(vector<string> &tokens);
command input_extractor(vector<string> &tokens);
vector<string> info_extractor(vector<string> &tokens);
void write_terminal(ANSWER input);
string answer_to_string(ANSWER input);
TERMINAL_PARSE_RESULT parse_terminal_command(string line);
QUESTION read_terminal(string line);
