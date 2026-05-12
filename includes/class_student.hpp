#pragma once

#include "global.hpp"
#include "class_user.hpp"
#include "class_class.hpp"

class Student : public User
{
private:
    vector<Class *> classes;

public:
    string major_id;
    int semester;
    Student(STUDENT input_info);
    Student();
    virtual response show_info(vector<string> *output, vector<MAJOR> *majors) override;
    response new_class(Class *input_new_class);
    response delete_class(string input_id);
    response show_class(vector<string> *output);
    bool has_class(string input_id) const;
};
