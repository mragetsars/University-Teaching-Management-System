#include "struct_data.hpp"

int str_to_int(string input)
{
    return stoi(input);
}
string int_to_str(int input)
{
    return to_string(input);
}

pos str_to_pos(string input)
{
    pos output;
    if (input == "instructor")
        output = instructor;
    if (input == "assistant professor")
        output = assistant_professor;
    if (input == "associate professor")
        output = associate_professor;
    if (input == "professor")
        output = professor;
    return output;
}
string pos_to_str(pos input)
{
    string output;
    if (input == instructor)
        output = "instructor";
    if (input == assistant_professor)
        output = "assistant professor";
    if (input == associate_professor)
        output = "associate professor";
    if (input == professor)
        output = "professor";
    return output;
}

day str_to_day(string input)
{
    day output;
    if (input == "Saturday")
        output = Saturday;
    if (input == "Sunday")
        output = Sunday;
    if (input == "Monday")
        output = Monday;
    if (input == "Tuesday")
        output = Tuesday;
    if (input == "Wednesday")
        output = Wednesday;
    if (input == "Thursday")
        output = Thursday;
    if (input == "Friday")
        output = Friday;
    return output;
}
string day_to_str(day input)
{
    string output;
    if (input == Saturday)
        output = "Saturday";
    if (input == Sunday)
        output = "Sunday";
    if (input == Monday)
        output = "Monday";
    if (input == Tuesday)
        output = "Tuesday";
    if (input == Wednesday)
        output = "Wednesday";
    if (input == Thursday)
        output = "Thursday";
    if (input == Friday)
        output = "Friday";
    return output;
}

TIME string_to_time(string input) {
    TIME output;
    string token;
    vector<string> larg_tokens;
    istringstream larg_tokenStream(input);
    while (getline(larg_tokenStream, token, COLON))
    {
        larg_tokens.push_back(token);
    }
    output.week = str_to_day(larg_tokens[0]);
    vector<string> small_tokens;
    istringstream small_tokenStream(larg_tokens[1]);
    while (getline(small_tokenStream, token, DASH))
    {
        small_tokens.push_back(token);
    }
    output.start = stoi(small_tokens[0]);
    output.end = stoi(small_tokens[1]);
    return output;
}

DATE string_to_date(string input) {
    DATE output;
    string token;
    vector<string> tokens;
    istringstream tokenStream(input);
    while (getline(tokenStream, token, SLAH))
    {
        tokens.push_back(token);
    }
    output.year = stoi(tokens[0]);
    output.month = stoi(tokens[1]);
    output.day = stoi(tokens[2]);
    return output;
}