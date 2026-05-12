#pragma once

#include "global.hpp"
#include "class_student.hpp"
#include "class_class.hpp"

#define NEW_FORM_notif "New Form"
#define TA_FORM_FOR "TA form for "
#define COURSE_ "course"
#define CLOSE_FORM_notif "Your request to be a teaching assistant has been"

struct FORM
{
    int id;
    Class *form_class;
    vector<User *> requests;
};

class Professor : public User
{
private:
    vector<Class *> classes;

public:
    pos position;
    string major_id;
    vector<FORM> forms;
    Professor(PROFESSOR input_info);
    Professor();
    virtual response show_info(vector<string> *output, vector<MAJOR> *majors) override;
    response new_form(Class *input_class, string input_message);
    response close_form(int input_id);
    response new_request(User *input_user, int input_id);
};