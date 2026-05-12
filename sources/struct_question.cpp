#include "struct_question.hpp"

bool Check_Error::is_whole_number(const std::string &input)
{
    if (input.empty())
        return false;
    for (char const &ch : input)
        if (!isdigit(ch))
            return false;
    return true;
}

bool Check_Error::is_natural_number(const std::string &input)
{
    return is_whole_number(input) && input != "0";
}

bool Check_Error::has_no_arguments(vector<string> *input)
{
    return input->size() == 1 && (*input)[0] == EMPTYSTRING;
}

string Check_Error::value_of(const vector<string> &input, const string &key)
{
    for (int i = 0; i + 1 < (int)input.size(); i += 2)
        if (input[i] == key)
            return input[i + 1];
    return EMPTYSTRING;
}

bool Check_Error::canonicalize(vector<string> *input, const vector<string> &required, const vector<string> &optional)
{
    if (input->size() % 2 != 0)
        return false;
    map<string, string> values;
    for (int i = 0; i < (int)input->size(); i += 2)
    {
        string key = (*input)[i];
        if (values.count(key))
            return false;
        values[key] = (*input)[i + 1];
    }

    vector<string> normalized;
    for (auto key : required)
    {
        if (!values.count(key))
            return false;
        normalized.push_back(key);
        normalized.push_back(values[key]);
        values.erase(key);
    }
    for (auto key : optional)
    {
        normalized.push_back(key);
        if (values.count(key))
        {
            normalized.push_back(values[key]);
            values.erase(key);
        }
        else
            normalized.push_back(EMPTYSTRING);
    }
    if (!values.empty())
        return false;
    *input = normalized;
    return true;
}

input_error Check_Error::POST_login_error(vector<string> *input)
{
    if (!canonicalize(input, {ID, PASSWORD}))
        return info_error;
    return is_whole_number((*input)[1]) ? no_error : info_error;
}

input_error Check_Error::GET_courses_error(vector<string> *input)
{
    if (has_no_arguments(input))
        return no_error;
    if (!canonicalize(input, {ID}))
        return info_error;
    return is_natural_number((*input)[1]) ? no_error : info_error;
}

input_error Check_Error::POST_post_error(vector<string> *input)
{
    if (!canonicalize(input, {TITLE, MESSAGE}, {IMAGE}))
        return info_error;
    return no_error;
}

input_error Check_Error::DELETE_post_error(vector<string> *input)
{
    if (!canonicalize(input, {ID}))
        return info_error;
    return is_natural_number((*input)[1]) ? no_error : info_error;
}

input_error Check_Error::GET_personal_page_error(vector<string> *input)
{
    if (!canonicalize(input, {ID}))
        return info_error;
    return is_whole_number((*input)[1]) ? no_error : info_error;
}

input_error Check_Error::GET_post_error(vector<string> *input)
{
    if (!canonicalize(input, {ID, POST_ID}))
        return info_error;
    return is_whole_number((*input)[1]) && is_natural_number((*input)[3]) ? no_error : info_error;
}

input_error Check_Error::POST_connect_error(vector<string> *input)
{
    if (!canonicalize(input, {ID}))
        return info_error;
    return is_whole_number((*input)[1]) ? no_error : info_error;
}

input_error Check_Error::POST_course_offer_error(vector<string> *input)
{
    if (!canonicalize(input, {COURSE_ID, PROFESSOR_ID, CAPACITY, CLASS_TIME, EXAM_DATE, CLASS_NUMBER}))
        return info_error;
    return is_natural_number((*input)[1]) && is_natural_number((*input)[3]) &&
                   is_natural_number((*input)[5]) && is_natural_number((*input)[11])
               ? no_error
               : info_error;
}

input_error Check_Error::PUT_my_courses_error(vector<string> *input)
{
    if (!canonicalize(input, {ID}))
        return info_error;
    return is_natural_number((*input)[1]) ? no_error : info_error;
}

input_error Check_Error::DELETE_my_courses_error(vector<string> *input)
{
    if (!canonicalize(input, {ID}))
        return info_error;
    return is_natural_number((*input)[1]) ? no_error : info_error;
}

input_error Check_Error::POST_profile_photo_error(vector<string> *input)
{
    return canonicalize(input, {PHOTO}) ? no_error : info_error;
}

input_error Check_Error::POST_course_post_error(vector<string> *input)
{
    if (!canonicalize(input, {ID, TITLE, MESSAGE}, {IMAGE}))
        return info_error;
    return is_natural_number((*input)[1]) ? no_error : info_error;
}

input_error Check_Error::GET_course_channel_error(vector<string> *input)
{
    if (!canonicalize(input, {ID}))
        return info_error;
    return is_natural_number((*input)[1]) ? no_error : info_error;
}

input_error Check_Error::GET_course_post_error(vector<string> *input)
{
    if (!canonicalize(input, {ID, POST_ID}))
        return info_error;
    return is_natural_number((*input)[1]) && is_natural_number((*input)[3]) ? no_error : info_error;
}

input_error Check_Error::POST_ta_form_error(vector<string> *input)
{
    if (!canonicalize(input, {COURSE_ID, MESSAGE}))
        return info_error;
    return is_natural_number((*input)[1]) ? no_error : info_error;
}

input_error Check_Error::POST_close_ta_form_error(vector<string> *input)
{
    if (!canonicalize(input, {ID}))
        return info_error;
    return is_natural_number((*input)[1]) ? no_error : info_error;
}

input_error Check_Error::POST_ta_request_error(vector<string> *input)
{
    if (!canonicalize(input, {PROFESSOR_ID, FORM_ID}))
        return info_error;
    return is_whole_number((*input)[1]) && is_natural_number((*input)[3]) ? no_error : info_error;
}

input_error Check_Error::run(QUESTION *input)
{
    switch (input->type)
    {
    case Get_Type:
        switch (input->input)
        {
        case Courses_Input:
            return GET_courses_error(&input->info);
        case Personal_Page_Input:
            return GET_personal_page_error(&input->info);
        case Post_Input:
            return GET_post_error(&input->info);
        case Notification_Input:
            return has_no_arguments(&input->info) ? no_error : info_error;
        case My_Courses_Input:
            return has_no_arguments(&input->info) ? no_error : info_error;
        case Course_Channel_Input:
            return GET_course_channel_error(&input->info);
        case Course_Post_Input:
            return GET_course_post_error(&input->info);
        case Invalid_Input:
            return command_error;
        default:
            return command_error;
        }
    case Put_Type:
        switch (input->input)
        {
        case My_Courses_Input:
            return PUT_my_courses_error(&input->info);
        case Invalid_Input:
            return command_error;
        default:
            return command_error;
        }
    case Post_Type:
        switch (input->input)
        {
        case Login_Input:
            return POST_login_error(&input->info);
        case Logout_Input:
            return has_no_arguments(&input->info) ? no_error : info_error;
        case Post_Input:
            return POST_post_error(&input->info);
        case Connect_Input:
            return POST_connect_error(&input->info);
        case Course_Offer_Input:
            return POST_course_offer_error(&input->info);
        case Profile_Photo_Input:
            return POST_profile_photo_error(&input->info);
        case Course_Post_Input:
            return POST_course_post_error(&input->info);
        case Ta_Form_Input:
            return POST_ta_form_error(&input->info);
        case Close_Ta_Form_Input:
            return POST_close_ta_form_error(&input->info);
        case Ta_Request_Input:
            return POST_ta_request_error(&input->info);
        case Invalid_Input:
            return command_error;
        default:
            return command_error;
        }
    case Delete_Type:
        switch (input->input)
        {
        case Post_Input:
            return DELETE_post_error(&input->info);
        case My_Courses_Input:
            return DELETE_my_courses_error(&input->info);
        case Invalid_Input:
            return command_error;
        default:
            return command_error;
        }
    case Invalid_Type:
        return command_type_error;
    }
    return no_error;
}
