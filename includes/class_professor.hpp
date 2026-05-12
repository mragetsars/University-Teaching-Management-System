#pragma once

#include "global.hpp"
#include "class_student.hpp"
#include "class_class.hpp"

struct FORM
{
    int id;
    Class *form_class;
    string message;
    vector<Student *> requests;
};

class Professor : public User
{
private:
    vector<Class *> classes;

public:
    pos position;
    string position_name;
    string major_id;
    vector<FORM> forms;

    Professor(PROFESSOR input_info);
    Professor();
    virtual response show_info(vector<string> *output, vector<MAJOR> *majors) override;
    virtual response show_post(vector<string> *output, int input_id) override;
    response new_form(Class *input_class, string input_message);
    response close_form(int input_id);
    response new_request(Student *input_user, int input_id);
    void offer_class(Class *input_class);
    bool owns_class(Class *input_class) const;
};
