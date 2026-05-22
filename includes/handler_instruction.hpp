#pragma once

#include "global.hpp"
#include "struct_question.hpp"
#include "class_user.hpp"
#include "class_class.hpp"
#include "class_student.hpp"
#include "class_professor.hpp"
#include "class_systemmanager.hpp"

struct WEB_USER_MINI_VIEW
{
    string id;
    string name;
    string type_label;
};

struct WEB_COURSE_DEF_VIEW
{
    string id;
    string name;
    int credit = 0;
    int prerequisite = 0;
    vector<string> major_ids;
};

struct WEB_CLASS_VIEW
{
    string id;
    string name;
    string capacity;
    string professor_name;
    string professor_id;
    string time;
    string exam_date;
    string class_number;
    bool current_user_can_view = false;
    bool current_user_can_post = false;
    vector<CHANNEL_POST> channel_posts;
};

struct WEB_USER_VIEW
{
    string id;
    string name;
    string type_label;
    string major;
    string semester;
    string position;
    string profile_photo_address;
    vector<string> course_names;
    vector<POST> posts;
};

struct WEB_TA_REQUEST_VIEW
{
    string student_id;
    string student_name;
    int semester = 0;
};

struct WEB_TA_FORM_VIEW
{
    int id = 0;
    string professor_id;
    string professor_name;
    string course_id;
    string course_name;
    string message;
    vector<WEB_TA_REQUEST_VIEW> requests;
};

struct UTMS_STATE_SNAPSHOT
{
    vector<string> lines;
};

class Instruction_Handler
{
private:
    bool is_login;
    vector<MAJOR> majors;
    vector<COURSE> courses;
    deque<Class> classes;
    System_Manager system_manager;
    vector<Student> students;
    vector<Professor> professors;
    string state_file_path;
    bool loading_state = false;

    bool find_user_by_id(User **user_ptr, string id);
    bool find_class_by_id(Class **class_ptr, string id);
    bool find_course_by_id(COURSE **course_ptr, string id);
    bool check_class_detail(User *input_teacher, string input_class_time);
    void broadcast_notification(User *sender, string subject);
    void persist_if_enabled();

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
    bool logged_in() const;
    bool current_user_is_student() const;
    bool current_user_is_professor() const;
    bool current_user_is_system_manager() const;
    string current_user_id() const;
    string current_user_name() const;
    string current_user_type_label() const;

    vector<WEB_USER_MINI_VIEW> web_all_users();
    vector<WEB_COURSE_DEF_VIEW> web_course_definitions();
    vector<WEB_USER_MINI_VIEW> web_professors();
    vector<WEB_CLASS_VIEW> web_all_classes();
    vector<WEB_CLASS_VIEW> web_my_student_classes();
    bool web_class_view(const string &id, WEB_CLASS_VIEW *output);
    bool web_user_view(const string &id, WEB_USER_VIEW *output);
    vector<WEB_TA_FORM_VIEW> web_all_ta_forms();
    vector<WEB_TA_FORM_VIEW> web_my_ta_forms();
    ANSWER web_close_ta_form(int form_id, const map<string, string> &decisions);

    void enable_state_persistence(const string &path, bool load_existing = true);
    bool save_state(const string &path) const;
    bool load_state(const string &path);
    string current_state_file() const;
};
