#pragma once

#include "global.hpp"
#include "struct_data.hpp"
#include "struct_answer.hpp"

#define NEW_POST_notif "New Post"
#define NEW_COURSE_OFFERING_notif "New Course Offering"
#define GET_COURSE_notif "Get Course"
#define DELETE_COURSE_notif "Delete Post"

class User
{
private:
    string password;
    vector<User *> connections;
    vector<NOTIFICATION> notifications;

public:
    string id;
    string name;
    vector<POST> posts;
    string profile_photo_address;

    User();
    User(USER input_info);
    response login(string input_id, string input_password);
    response new_post(string input_title, string input_message, string input_image_address);
    response delete_post(int input_id);
    response show_post(vector<string> *output, int input_id);
    response show_page(vector<string> *output);
    virtual response show_info(vector<string> *output, vector<MAJOR> *majors) = 0;
    response new_connection(User *connection);
    response show_notifications(vector<string> *output);
    void send_notifications(string input_subject);
    void receive_notification(string input_id, string input_name, string input_subject);
    response set_profile_photo(string input_photo_address);
};