#include "struct_question.hpp"

bool Check_Error::is_Whole_id(const std::string &input)
{
    for (char const &ch : input)
        if (!isdigit(ch))
            return false;
    return true;
}

bool Check_Error::is_natural_number(const std::string &input)
{
    if (input.empty() || (!isdigit(input[0])))
        return false;
    for (char const &ch : input)
        if (!isdigit(ch))
            return false;
    return input != "0";
}

input_error Check_Error::POST_login_error(vector<string> *input)
{
    if (input->size() == 4)
        if ((*input)[0] == ID &&
            (*input)[2] == PASSWORD &&
            is_Whole_id((*input)[1]))
            return no_error;
        else if ((*input)[2] == ID &&
                 (*input)[0] == PASSWORD)
        {
            string id;
            string password;
            id = (*input)[3];
            password = (*input)[1];
            (*input)[1] = id;
            (*input)[3] = password;
            if (is_Whole_id((*input)[1]))
                return no_error;
        }
    return info_error;
}
input_error Check_Error::GET_courses_error(vector<string> *input)
{
    if ((*input)[0] == EMPTYSTRING)
        return no_error;
    if (input->size() == 2)
        if ((*input)[0] == ID &&
            is_natural_number((*input)[1]))
            return no_error;
    return info_error;
}
input_error Check_Error::POST_post_error(vector<string> *input)
{
    if (input->size() == 4)
    {
        if ((*input)[0] == TITLE &&
            (*input)[2] == MESSAGE)
        {
            input->push_back(IMAGE);
            input->push_back(EMPTYSTRING);
            return no_error;
        }
        else if ((*input)[2] == TITLE &&
                 (*input)[0] == MESSAGE)
        {
            string title;
            string massage;
            title = (*input)[3];
            massage = (*input)[1];
            (*input)[1] = title;
            (*input)[3] = massage;
            input->push_back(IMAGE);
            input->push_back(EMPTYSTRING);
            return no_error;
        }
    }
    else if (input->size() == 6)
        if ((*input)[0] == TITLE &&
            (*input)[2] == MESSAGE &&
            (*input)[4] == IMAGE)
            return no_error;
    return info_error;
}
input_error Check_Error::DELETE_post_error(vector<string> *input)
{
    if (input->size() == 2)
        if ((*input)[0] == ID &&
            is_natural_number((*input)[1]))
            return no_error;
    return info_error;
}
input_error Check_Error::GET_personal_page_error(vector<string> *input)
{
    if (input->size() == 2)
        if ((*input)[0] == ID &&
            is_Whole_id((*input)[1]))
            return no_error;
    return info_error;
}
input_error Check_Error::GET_post_error(vector<string> *input)
{
    if (input->size() == 4)
        if ((*input)[0] == ID && (*input)[2] == POST_ID &&
            is_Whole_id((*input)[1]) && is_natural_number((*input)[1]))
            return no_error;
    return info_error;
}
input_error Check_Error::POST_connect_error(vector<string> *input)
{
    if (input->size() == 2)
        if ((*input)[0] == ID &&
            is_Whole_id((*input)[1]))
            return no_error;
    return info_error;
}
input_error Check_Error::POST_course_offer_error(vector<string> *input)
{
    if (input->size() == 12)
        if ((*input)[0] == COURSE_ID &&
            (*input)[2] == PROFESSOR_ID &&
            (*input)[4] == CAPACITY &&
            (*input)[6] == CLASS_TIME &&
            (*input)[8] == EXAM_DATE &&
            (*input)[10] == CLASS_NUMBER &&
            is_natural_number((*input)[1]) &&
            is_natural_number((*input)[3]) &&
            is_natural_number((*input)[5]) &&
            is_natural_number((*input)[11]))
            return no_error;
    return info_error;
}
input_error Check_Error::PUT_my_courses_error(vector<string> *input)
{
    if (input->size() == 2)
        if ((*input)[0] == ID &&
            is_natural_number((*input)[1]))
            return no_error;
    return info_error;
}
input_error Check_Error::DELETE_my_courses_error(vector<string> *input)
{
    if (input->size() == 2)
        if ((*input)[0] == ID &&
            is_natural_number((*input)[1]))
            return no_error;
    return info_error;
}
input_error Check_Error::POST_profile_photo_error(vector<string> *input)
{
    if (input->size() == 2)
        if ((*input)[0] == PHOTO)
            return no_error;
    return info_error;
}
input_error Check_Error::POST_course_post_error(vector<string> *input)
{
    if (input->size() == 6)
        if ((*input)[0] == ID &&
            (*input)[2] == TITLE &&
            (*input)[4] == MESSAGE)
        {
            input->push_back(IMAGE);
            input->push_back(EMPTYSTRING);
            return no_error;
        }
    if (input->size() == 8)
        if ((*input)[0] == ID &&
            (*input)[2] == TITLE &&
            (*input)[4] == MESSAGE &&
            (*input)[6] == IMAGE)
            return no_error;
    return info_error;
}
input_error Check_Error::GET_course_channel_error(vector<string> *input)
{
    if (input->size() == 2)
        if ((*input)[0] == ID &&
            is_natural_number((*input)[1]))
            return no_error;
    return info_error;
}
input_error Check_Error::GET_course_post_error(vector<string> *input)
{
    if (input->size() == 4)
        if ((*input)[0] == ID && (*input)[2] == POST_ID &&
            is_Whole_id((*input)[1]) && is_natural_number((*input)[1]))
            return no_error;
    return info_error;
}
input_error Check_Error::POST_ta_form_error(vector<string> *input)
{
    if (input->size() == 4)
        if ((*input)[0] == COURSE_ID && (*input)[2] == MESSAGE &&
            is_natural_number((*input)[1]))
            return no_error;
    return info_error;
}
input_error Check_Error::POST_close_ta_form_error(vector<string> *input)
{
    if (input->size() == 2)
        if ((*input)[0] == ID &&
            is_natural_number((*input)[1]))
            return no_error;
    return info_error;
}
input_error Check_Error::POST_ta_request_error(vector<string> *input)
{
    if (input->size() == 4)
        if ((*input)[0] == PROFESSOR_ID && (*input)[2] == FORM_ID &&
            is_Whole_id((*input)[1]) && is_natural_number((*input)[1]))
            return no_error;
    return info_error;
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
            break;
        case Personal_Page_Input:
            return GET_personal_page_error(&input->info);
            break;
        case Post_Input:
            return GET_post_error(&input->info);
            break;
        case Notification_Input:
            return no_error;
            break;
        case My_Courses_Input:
            return no_error;
            break;
        case Course_Channel_Input:
            return GET_course_channel_error(&input->info);
            break;
        case Course_Post_Input:
            return GET_course_post_error(&input->info);
            break;
        case Invalid_Input:
            return command_error;
            break;
        }
        break;
    case Put_Type:
        switch (input->input)
        {
        case My_Courses_Input:
            return PUT_my_courses_error(&input->info);
            break;
        case Invalid_Input:
            return command_error;
            break;
        }
        break;
    case Post_Type:
        switch (input->input)
        {
        case Login_Input:
            return POST_login_error(&input->info);
            break;
        case Logout_Input:
            return no_error;
            break;
        case Post_Input:
            return POST_post_error(&input->info);
            break;
        case Connect_Input:
            return POST_connect_error(&input->info);
            break;
        case Course_Offer_Input:
            return POST_course_offer_error(&input->info);
            break;
        case Profile_Photo_Input:
            return POST_profile_photo_error(&input->info);
            break;
        case Course_Post_Input:
            return POST_course_post_error(&input->info);
            break;
        case Course_Channel_Input:
            return GET_course_channel_error(&input->info);
            break;
        case Ta_Form_Input:
            return POST_ta_form_error(&input->info);
            break;
        case Close_Ta_Form_Input:
            return POST_close_ta_form_error(&input->info);
            break;
        case Ta_Request_Input:
            return POST_ta_request_error(&input->info);
            break;
        case Invalid_Input:
            return command_error;
            break;
        }
        break;
    case Delete_Type:
        switch (input->input)
        {
        case Post_Input:
            return GET_courses_error(&input->info);
            break;
        case My_Courses_Input:
            return DELETE_my_courses_error(&input->info);
            break;
        case Invalid_Input:
            return command_error;
            break;
        }
        break;
    case Invalid_Type:
        return command_type_error;
        break;
    }
    return no_error;
}