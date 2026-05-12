#pragma once

#include "global.hpp"
#include "struct_question.hpp"
#include "class_user.hpp"
#include "class_class.hpp"
#include "class_student.hpp"
#include "class_professor.hpp"
#include "class_systemmanager.hpp"

class Instruction_Handler
{
private:
    bool is_login;
    vector<MAJOR> majors;
    vector<COURSE> courses;
    vector<Class> classes;
    System_Manager system_manager;
    vector<Student> students;
    vector<Professor> professors;

    bool find_user_by_id(User **user_ptr, string id);
    bool find_class_by_id(Class **class_ptr, string id);
    bool check_class_detail(User *input_teacher, string input_class_time);

    ANSWER POST_login(vector<string> input);
    ANSWER POST_logout();
    ANSWER GET_courses(vector<string> input);
    ANSWER POST_post(vector<string> input);
    ANSWER DELETE_post(vector<string> input);
    ANSWER GET_personal_page(vector<string> input);
    ANSWER GET_post(vector<string> input);
    ANSWER POST_connect(vector<string> input);
    ANSWER GET_notification();
    ANSWER POST_course_offer(vector<string> input);
    ANSWER PUT_my_courses(vector<string> input);
    ANSWER DELETE_my_courses(vector<string> input);
    ANSWER GET_my_courses();
    ANSWER POST_profile_photo(vector<string> input);
    ANSWER POST_course_post(vector<string> input);
    ANSWER GET_course_channel(vector<string> input);
    ANSWER GET_course_post(vector<string> input);
    ANSWER POST_ta_form(vector<string> input);
    ANSWER POST_close_ta_form(vector<string> input);
    ANSWER POST_ta_request(vector<string> input);

public:
    User *user;
    Instruction_Handler(DATA input_data);
    ANSWER run(QUESTION input);
};
