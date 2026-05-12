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
    if (input == "instructor")
        return instructor;
    if (input == "assistant professor")
        return assistant_professor;
    if (input == "associate professor")
        return associate_professor;
    if (input == "professor")
        return professor;
    return instructor;
}
string pos_to_str(pos input)
{
    if (input == instructor)
        return "instructor";
    if (input == assistant_professor)
        return "assistant professor";
    if (input == associate_professor)
        return "associate professor";
    if (input == professor)
        return "professor";
    return "instructor";
}

day str_to_day(string input)
{
    if (input == "Saturday")
        return Saturday;
    if (input == "Sunday")
        return Sunday;
    if (input == "Monday")
        return Monday;
    if (input == "Tuesday")
        return Tuesday;
    if (input == "Wednesday")
        return Wednesday;
    if (input == "Thursday")
        return Thursday;
    return Friday;
}
string day_to_str(day input)
{
    if (input == Saturday)
        return "Saturday";
    if (input == Sunday)
        return "Sunday";
    if (input == Monday)
        return "Monday";
    if (input == Tuesday)
        return "Tuesday";
    if (input == Wednesday)
        return "Wednesday";
    if (input == Thursday)
        return "Thursday";
    return "Friday";
}

TIME string_to_time(string input)
{
    TIME output;
    string token;
    vector<string> large_tokens;
    istringstream large_token_stream(input);
    while (getline(large_token_stream, token, COLON))
        large_tokens.push_back(token);
    output.week = str_to_day(large_tokens[0]);
    vector<string> small_tokens;
    istringstream small_token_stream(large_tokens[1]);
    while (getline(small_token_stream, token, DASH))
        small_tokens.push_back(token);
    output.start = stoi(small_tokens[0]);
    output.end = stoi(small_tokens[1]);
    return output;
}

string time_to_string(TIME input)
{
    return day_to_str(input.week) + string(1, COLON) + int_to_str(input.start) + string(1, DASH) + int_to_str(input.end);
}

DATE string_to_date(string input)
{
    DATE output;
    string token;
    vector<string> tokens;
    istringstream token_stream(input);
    while (getline(token_stream, token, SLAH))
        tokens.push_back(token);
    output.year = stoi(tokens[0]);
    output.month = stoi(tokens[1]);
    output.day = stoi(tokens[2]);
    return output;
}

string date_to_string(DATE input)
{
    return int_to_str(input.year) + string(1, SLAH) + int_to_str(input.month) + string(1, SLAH) + int_to_str(input.day);
}

bool times_overlap(TIME first, TIME second)
{
    return first.week == second.week && first.start < second.end && second.start < first.end;
}

bool dates_equal(DATE first, DATE second)
{
    return first.year == second.year && first.month == second.month && first.day == second.day;
}

string quote_text(const string &input)
{
    return string(1, DOUBLEQUOTATION) + input + string(1, DOUBLEQUOTATION);
}
