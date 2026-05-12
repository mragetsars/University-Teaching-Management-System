#include "io_terminal.hpp"

static string strip_outer_quotes(const string &input)
{
    if (input.size() >= 2 && input.front() == DOUBLEQUOTATION && input.back() == DOUBLEQUOTATION)
        return input.substr(1, input.size() - 2);
    return input;
}

vector<string> line_extractor(string input)
{
    vector<string> output;
    string token;
    istringstream tokenStream(input);
    char delimiter = ' ';
    while (getline(tokenStream, token, delimiter))
    {
        if (token.empty())
            continue;
        if (token.front() == DOUBLEQUOTATION && token.back() != DOUBLEQUOTATION)
        {
            string quotedToken = token;
            while (getline(tokenStream, token, delimiter))
            {
                quotedToken += delimiter + token;
                if (!token.empty() && token.back() == DOUBLEQUOTATION)
                    break;
            }
            output.push_back(strip_outer_quotes(quotedToken));
        }
        else
            output.push_back(strip_outer_quotes(token));
    }
    return output;
}

command_type type_extractor(vector<string> &tokens)
{
    if (tokens.empty())
        return Invalid_Type;
    if (tokens[0] == GET_Type)
        return Get_Type;
    if (tokens[0] == PUT_Type)
        return Put_Type;
    if (tokens[0] == POST_Type)
        return Post_Type;
    if (tokens[0] == DELETE_Type)
        return Delete_Type;
    return Invalid_Type;
}

command input_extractor(vector<string> &tokens)
{
    if (tokens.size() < 2)
        return Invalid_Input;
    if (tokens[1] == POST_Input)
        return Post_Input;
    if (tokens[1] == LOGIN_Input)
        return Login_Input;
    if (tokens[1] == LOGOUT_Input)
        return Logout_Input;
    if (tokens[1] == COURSES_Input)
        return Courses_Input;
    if (tokens[1] == CONNECT_Input)
        return Connect_Input;
    if (tokens[1] == MY_COURSES_Input)
        return My_Courses_Input;
    if (tokens[1] == NOTIFICATION_Input)
        return Notification_Input;
    if (tokens[1] == COURSE_OFFER_Input)
        return Course_Offer_Input;
    if (tokens[1] == PERSONAL_PAGE_Input)
        return Personal_Page_Input;
    if (tokens[1] == PROFILE_PHOTO)
        return Profile_Photo_Input;
    if (tokens[1] == COURSE_POST)
        return Course_Post_Input;
    if (tokens[1] == COURSE_CHANNEL)
        return Course_Channel_Input;
    if (tokens[1] == TA_FORM)
        return Ta_Form_Input;
    if (tokens[1] == CLOSE_TA_FORM)
        return Close_Ta_Form_Input;
    if (tokens[1] == TA_REQUEST)
        return Ta_Request_Input;
    return Invalid_Input;
}

vector<string> info_extractor(vector<string> &tokens)
{
    vector<string> output;
    if (tokens.size() < 3 || tokens[2] != "?")
    {
        output.push_back(INFO_ERROR);
        return output;
    }
    if (tokens.size() == 3)
    {
        output.push_back(EMPTYSTRING);
        return output;
    }
    for (int i = 3; i < (int)tokens.size(); i++)
        output.push_back(tokens[i]);
    return output;
}

string answer_to_string(ANSWER input)
{
    ostringstream output;
    switch (input.output)
    {
    case Ok:
        output << OK << endl;
        break;
    case Empty:
        output << EMPTY << endl;
        break;
    case NotFound:
        output << NOT_FOUND << endl;
        break;
    case BadRequest:
        output << BAD_REQUEST << endl;
        break;
    case PermissionDenied:
        output << PERMISSION_DENIED << endl;
        break;
    case JustInformation:
        for (auto item : input.info)
            output << item;
        break;
    }
    return output.str();
}

void write_terminal(ANSWER input)
{
    cout << answer_to_string(input);
}

TERMINAL_PARSE_RESULT parse_terminal_command(string line)
{
    TERMINAL_PARSE_RESULT parsed;
    parsed.should_run = true;
    parsed.parse_error = Ok;
    parsed.question.invalid = true;

    vector<string> tokens = line_extractor(line);
    parsed.question.type = type_extractor(tokens);
    parsed.question.input = input_extractor(tokens);
    parsed.question.info = info_extractor(tokens);

    Check_Error ce;
    switch (ce.run(&parsed.question))
    {
    case command_type_error:
        parsed.should_run = false;
        parsed.question.invalid = false;
        parsed.parse_error = BadRequest;
        break;
    case command_error:
        parsed.should_run = false;
        parsed.question.invalid = false;
        parsed.parse_error = NotFound;
        break;
    case info_error:
        parsed.question.info.clear();
        parsed.question.info.push_back(INFO_ERROR);
        break;
    default:
        break;
    }
    return parsed;
}

QUESTION read_terminal(string line)
{
    TERMINAL_PARSE_RESULT parsed = parse_terminal_command(line);
    if (!parsed.should_run)
    {
        ANSWER result;
        result.output = parsed.parse_error;
        write_terminal(result);
    }
    return parsed.question;
}
