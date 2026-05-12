#include "class_class.hpp"

Class::Class(string input_id,
             COURSE *input_course,
             int input_capacity,
             User *input_teacher,
             string input_class_time,
             string input_exam_date,
             string input_class_number)
{
    id = input_id;
    course = input_course;
    capacity = input_capacity;
    teacher = input_teacher;
    class_time = string_to_time(input_class_time);
    exam_date = string_to_date(input_exam_date);
    claee_number = stoi(input_class_number);
}
Class::Class() {}

bool Class::has_accsess(User *input_user, int level)
{
    bool output = false;
    if (teacher->id == input_user->id)
        output = true;
    if (level == 3)
        return output;
    for (int i = 0; i < students.size(); i++)
        if (students[i]->id == input_user->id)
            output = true;
    if (level == 2)
        return output;
    for (int i = 0; i < teacher_assistants.size(); i++)
        if (teacher_assistants[i]->id == input_user->id)
            output = true;
    return output;
}

response Class::show_info(vector<string> *output)
{
    output->push_back(id);
    output->push_back(SPACE);
    output->push_back(course->name);
    output->push_back(SPACE);
    output->push_back(to_string(capacity));
    output->push_back(SPACE);
    output->push_back(teacher->name);
    return JustInformation;
}
response Class::show_page(vector<string> *output)
{
    (*output)[output->size() - 1] = SPACE;
    output->push_back(day_to_str(class_time.week));
    output->push_back(string(1, COLON));
    output->push_back(to_string(class_time.start));
    output->push_back(string(1, DASH));
    output->push_back(to_string(class_time.end));
    output->push_back(SPACE);
    output->push_back(to_string(exam_date.year));
    output->push_back(string(1, SLAH));
    output->push_back(to_string(exam_date.month));
    output->push_back(string(1, SLAH));
    output->push_back(to_string(exam_date.day));
    return JustInformation;
}
bool Class::new_student(User *new_student)
{
    if (students.size() < capacity)
        students.push_back(new_student);
    else
        return false;
    return true;
}
bool Class::new_teacher_assistant(User *new_student)
{
    teacher_assistants.push_back(new_student);
    return true;
}
bool Class::delete_student(string input_id)
{
    int n = -1;
    for (int i = 0; i < students.size(); i++)
        if (students[i]->id == input_id)
            n = i;
    if (n == -1)
        return false;
    else
        students.erase(students.begin() + n);
    return true;
}
response Class::new_post(User *input_author, string input_title, string input_message, string input_image_address)
{
    if (!has_accsess(input_author, 2))
        return PermissionDenied;
    CHANNEL_POST the_post;
    if (channel_posts.size() != 0)
        the_post.id = channel_posts[channel_posts.size() - 1].id + 1;
    else
        the_post.id = 1;
    the_post.author_name = input_author->name;
    the_post.title = input_title;
    the_post.message = input_message;
    the_post.image_address = input_image_address;
    channel_posts.push_back(the_post);
    send_notifications(NEW_COURS_POST_notif);
    return Ok;
}
void Class::send_notifications(string input_subject)
{
    teacher->receive_notification(id, course->name, input_subject);
    for (int i = 0; i < students.size(); i++)
        students[i]->receive_notification(id, course->name, input_subject);
    for (int i = 0; i < teacher_assistants.size(); i++)
        teacher_assistants[i]->receive_notification(id, course->name, input_subject);
}
response Class::show_channel(User *input_user, vector<string> *output)
{
    if (!has_accsess(input_user, 1))
        return PermissionDenied;
    show_info(output);
    show_page(output);
    output->push_back(ENTER);
    for (int i = 0; i < channel_posts.size(); i++)
    {
        output->push_back(int_to_str(channel_posts[i].id));
        output->push_back(SPACE);
        output->push_back(channel_posts[i].author_name);
        output->push_back(SPACE);
        output->push_back(channel_posts[i].title);
        output->push_back(ENTER);
    }
    return JustInformation;
}
response Class::show_post(vector<string> *output, int input_id)
{
    if (channel_posts.size() == 0)
        return Empty;
    int n = -1;
    for (int i = 0; i < channel_posts.size(); i++)
        if (channel_posts[i].id == input_id)
            n = i;
    if (n == -1)
        return NotFound;
    else
    {
        output->push_back(int_to_str(channel_posts[n].id));
        output->push_back(SPACE);
        output->push_back(channel_posts[n].author_name);
        output->push_back(SPACE);
        output->push_back(channel_posts[n].title);
        output->push_back(SPACE);
        output->push_back(channel_posts[n].message);
        output->push_back(ENTER);
    }
    return JustInformation;
}