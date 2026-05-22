#pragma once

#include "global.hpp"
#include "struct_answer.hpp"
#include "struct_data.hpp"
#include "class_user.hpp"

#define NEW_COURSE_POST_notif "New Course Post"

class Class
{
private:
    vector<User *> students;
    vector<User *> teacher_assistants;
    vector<CHANNEL_POST> channel_posts;

public:
    TIME class_time;
    DATE exam_date;
    string id;
    COURSE *course;
    int capacity;
    User *teacher;
    int class_number;

    Class(string input_id,
          COURSE *input_course,
          int input_capacity,
          User *input_teacher,
          string input_class_time,
          string input_exam_date,
          string input_class_number);
    Class();

    bool has_accsess(User *input_user, int level);
    bool has_student(string input_id) const;
    bool has_teacher_assistant(string input_id) const;
    bool is_full() const;

    response show_info(vector<string> *output);
    response show_page(vector<string> *output);
    void append_full_info(vector<string> *output);
    bool new_student(User *new_student);
    bool new_teacher_assistant(User *new_student);
    bool delete_student(string input_id);
    response new_post(User *input_author, string input_title, string input_message, string input_image_address);
    response show_post(vector<string> *output, int input_id);
    void send_notifications(string input_subject, User *excluded_user = nullptr);
    response show_channel(User *input_user, vector<string> *output);
    vector<CHANNEL_POST> get_channel_posts() const;
    void restore_student(User *student);
    void restore_teacher_assistant(User *assistant);
    void restore_channel_post(const CHANNEL_POST &post);
    vector<string> student_ids() const;
    vector<string> teacher_assistant_ids() const;
};
