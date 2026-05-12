#pragma once

#include "global.hpp"
#include "struct_data.hpp"
#include "struct_answer.hpp"

#define NEW_POST_notif "New Post"
#define NEW_COURSE_OFFERING_notif "New Course Offering"
#define GET_COURSE_notif "Get Course"
#define DELETE_COURSE_notif "Delete Course"
#define NEW_FORM_notif "New Form"
#define TA_FORM_FOR "TA form for "
#define COURSE_ "course"
#define CLOSE_FORM_notif "Your request to be a teaching assistant has been "

class User
{
private:
    string password;

protected:
    vector<User *> connections;
    vector<NOTIFICATION> notifications;

public:
    string id;
    string name;
    vector<POST> posts;
    string profile_photo_address;

    User();
    User(USER input_info);
    virtual ~User() = default;

    response login(string input_id, string input_password);
    response new_post(string input_title, string input_message, string input_image_address);
    response delete_post(int input_id);
    virtual response show_post(vector<string> *output, int input_id);
    virtual response show_page(vector<string> *output);
    virtual response show_info(vector<string> *output, vector<MAJOR> *majors) = 0;
    response new_connection(User *connection);
    void add_connection_one_way(User *connection);
    bool has_connection(User *connection) const;
    response show_notifications(vector<string> *output);
    void send_notifications(string input_subject);
    void receive_notification(string input_id, string input_name, string input_subject);
    response set_profile_photo(string input_photo_address);
    string profile_photo() const;
};
