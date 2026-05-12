#include "io_terminal.hpp"

vector<string> line_extractor(string input)
{
    vector<string> output;
    string token;
    istringstream tokenStream(input);
    char delimiter = ' ';
    while (getline(tokenStream, token, delimiter))
        if (token.front() == '\"' && token.back() != '\"')
        {
            string quotedToken = token;
            while (getline(tokenStream, token, delimiter))
            {
                quotedToken += delimiter + token;
                if (token.back() == '\"')
                    break;
            }
            output.push_back(quotedToken);
        }
        else
            output.push_back(token);
    return output;
}

vector<string> extractStrings(string input)
{
    vector<string> output;
    istringstream stream(input);
    string segment;

    getline(stream, segment, '\"');
    output.push_back(segment);
    getline(stream, segment, '\"');
    output.push_back(segment);
    getline(stream, segment, '\"');
    output.push_back(segment);
    getline(stream, segment, '\"');
    output.push_back(segment);

    return output;
}

command_type type_extractor(vector<string> &tokens)
{
    if (tokens.size() >= 1)
    {
        if (tokens[0] == GET_Type)
            return Get_Type;
        else if (tokens[0] == PUT_Type)
            return Put_Type;
        else if (tokens[0] == POST_Type)
            return Post_Type;
        else if (tokens[0] == DELETE_Type)
            return Delete_Type;
        else
            return Invalid_Type;
    }
    else
        return Invalid_Type;
}

command input_extractor(vector<string> &tokens)
{
    if (tokens.size() >= 2)
    {
        if (tokens[1] == POST_Input)
            return Post_Input;
        else if (tokens[1] == LOGIN_Input)
            return Login_Input;
        else if (tokens[1] == LOGOUT_Input)
            return Logout_Input;
        else if (tokens[1] == COURSES_Input)
            return Courses_Input;
        else if (tokens[1] == CONNECT_Input)
            return Connect_Input;
        else if (tokens[1] == MY_COURSES_Input)
            return My_Courses_Input;
        else if (tokens[1] == NOTIFICATION_Input)
            return Notification_Input;
        else if (tokens[1] == COURSE_OFFER_Input)
            return Course_Offer_Input;
        else if (tokens[1] == PERSONAL_PAGE_Input)
            return Personal_Page_Input;
        else if (tokens[1] == PROFILE_PHOTO)
            return Profile_Photo_Input;
        else if (tokens[1] == COURSE_POST)
            return Course_Post_Input;
        else if (tokens[1] == COURSE_CHANNEL)
            return Course_Channel_Input;
        else if (tokens[1] == TA_FORM)
            return Ta_Form_Input;
        else if (tokens[1] == CLOSE_TA_FORM)
            return Close_Ta_Form_Input;
        else if (tokens[1] == TA_REQUEST)
            return Ta_Request_Input;
        else
            return Invalid_Input;
    }
    else
        return Invalid_Input;
}

vector<string> info_extractor(vector<string> &tokens)
{
    vector<string> output;
    if (tokens.size() > 4)
        for (int i = 3; i < tokens.size(); i++)
            output.push_back(tokens[i]);
    else
        output.push_back(EMPTYSTRING);
    return output;
}

void write_terminal(ANSWER input)
{
    switch (input.output)
    {
    case Ok:
        cout << OK << endl;
        break;
    case Empty:
        cout << EMPTY << endl;
        break;
    case NotFound:
        cout << NOT_FOUND << endl;
        break;
    case BadRequest:
        cout << BAD_REQUEST << endl;
        break;
    case PermissionDenied:
        cout << PERMISSION_DENIED << endl;
        break;
    case JustInformation:
        for (int i = 0; i < input.info.size(); i++)
            cout << input.info[i];
        break;
    }
}

QUESTION read_terminal(string line)
{
    QUESTION output;
    output.invalid = true;
    vector<string> tokens = line_extractor(line);
    output.type = type_extractor(tokens);
    output.input = input_extractor(tokens);
    output.info = info_extractor(tokens);
    ANSWER result;
    Check_Error ce;
    switch (ce.run(&output))
    {
    case command_type_error:
        result.output = BadRequest;
        write_terminal(result);
        output.invalid = false;
        break;
    case command_error:
        result.output = NotFound;
        write_terminal(result);
        output.invalid = false;
        break;
    case info_error:
        output.info[0] = INFO_ERROR;
        break;
    default:
        break;
    }
    return output;
}