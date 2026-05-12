#pragma once

#include "global.hpp"

#define GET_Type "GET"
#define PUT_Type "PUT"
#define POST_Type "POST"
#define DELETE_Type "DELETE"

#define POST_Input "post"
#define LOGIN_Input "login"
#define LOGOUT_Input "logout"
#define COURSES_Input "courses"
#define CONNECT_Input "connect"
#define GET_COURSES_Input "get_courses"
#define MY_COURSES_Input "my_courses"
#define NOTIFICATION_Input "notification"
#define COURSE_OFFER_Input "course_offer"
#define PERSONAL_PAGE_Input "personal_page"
#define PROFILE_PHOTO "profile_photo"
#define COURSE_POST "course_post"
#define COURSE_CHANNEL "course_channel"
#define TA_FORM "ta_form"
#define CLOSE_TA_FORM "close_ta_form"
#define TA_REQUEST "ta_request"

#define OK "OK"
#define EMPTY "Empty"
#define NOT_FOUND "Not Found"
#define BAD_REQUEST "Bad Request"
#define PERMISSION_DENIED "Permission Denied"

enum command_type
{
    Get_Type,
    Put_Type,
    Post_Type,
    Delete_Type,
    Invalid_Type,
};

enum command
{
    Post_Input,
    Login_Input,
    Logout_Input,
    Courses_Input,
    Connect_Input,
    Invalid_Input,
    My_Courses_Input,
    Notification_Input,
    Course_Offer_Input,
    Personal_Page_Input,
    Profile_Photo_Input,
    Course_Post_Input,
    Course_Channel_Input,
    Ta_Form_Input,
    Close_Ta_Form_Input,
    Ta_Request_Input
};

struct QUESTION
{
    bool invalid;
    command input;
    command_type type;
    vector<string> info;
};

#define ID "id"
#define PASSWORD "password"
#define POST_ID "post_id"
#define TITLE "title"
#define MESSAGE "message"
#define COURSE_ID "course_id"
#define PROFESSOR_ID "professor_id"
#define CAPACITY "capacity"
#define CLASS_TIME "time"
#define EXAM_DATE "exam_date"
#define CLASS_NUMBER "class_number"
#define IMAGE "image"
#define PHOTO "photo"
#define FORM_ID "form_id"

#define INFO_ERROR "IE"

enum input_error
{
    no_error,
    command_error,
    command_type_error,
    info_error
};

class Check_Error
{
private:
    bool is_whole_number(const std::string &input);
    bool is_natural_number(const std::string &input);
    bool has_no_arguments(vector<string> *input);
    bool canonicalize(vector<string> *input, const vector<string> &required, const vector<string> &optional = {});
    string value_of(const vector<string> &input, const string &key);

    input_error POST_login_error(vector<string> *input);
    input_error GET_courses_error(vector<string> *input);
    input_error POST_post_error(vector<string> *input);
    input_error DELETE_post_error(vector<string> *input);
    input_error GET_personal_page_error(vector<string> *input);
    input_error GET_post_error(vector<string> *input);
    input_error POST_connect_error(vector<string> *input);
    input_error POST_course_offer_error(vector<string> *input);
    input_error PUT_my_courses_error(vector<string> *input);
    input_error DELETE_my_courses_error(vector<string> *input);
    input_error POST_profile_photo_error(vector<string> *input);
    input_error POST_course_post_error(vector<string> *input);
    input_error GET_course_channel_error(vector<string> *input);
    input_error GET_course_post_error(vector<string> *input);
    input_error POST_ta_form_error(vector<string> *input);
    input_error POST_close_ta_form_error(vector<string> *input);
    input_error POST_ta_request_error(vector<string> *input);

public:
    input_error run(QUESTION *input);
};
