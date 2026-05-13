#include "class_class.hpp"

#include <set>

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
    class_number = stoi(input_class_number);
}
Class::Class() {}

bool Class::has_accsess(User *input_user, int level)
{
    if (input_user == nullptr)
        return false;
    if (teacher->id == input_user->id)
        return true;
    if (level == 3) // only professor of the class
        return false;
    if (level == 2) // professor and TAs may post
        return has_teacher_assistant(input_user->id);
    // level 1: professor, TAs, and enrolled students may view
    return has_teacher_assistant(input_user->id) || has_student(input_user->id);
}

bool Class::has_student(string input_id) const
{
    for (auto student : students)
        if (student->id == input_id)
            return true;
    return false;
}

bool Class::has_teacher_assistant(string input_id) const
{
    for (auto ta : teacher_assistants)
        if (ta->id == input_id)
            return true;
    return false;
}

bool Class::is_full() const
{
    return (int)students.size() >= capacity;
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
    output->push_back(SPACE);
    output->push_back(time_to_string(class_time));
    output->push_back(SPACE);
    output->push_back(date_to_string(exam_date));
    output->push_back(SPACE);
    output->push_back(to_string(class_number));
    return JustInformation;
}

void Class::append_full_info(vector<string> *output)
{
    show_info(output);
    show_page(output);
}

bool Class::new_student(User *new_student)
{
    if (new_student == nullptr || has_student(new_student->id) || is_full())
        return false;
    students.push_back(new_student);
    return true;
}

bool Class::new_teacher_assistant(User *new_student)
{
    if (new_student == nullptr || has_teacher_assistant(new_student->id))
        return false;
    teacher_assistants.push_back(new_student);
    return true;
}

bool Class::delete_student(string input_id)
{
    for (int i = 0; i < (int)students.size(); i++)
        if (students[i]->id == input_id)
        {
            students.erase(students.begin() + i);
            return true;
        }
    return false;
}

response Class::new_post(User *input_author, string input_title, string input_message, string input_image_address)
{
    if (!has_accsess(input_author, 2))
        return PermissionDenied;
    CHANNEL_POST the_post;
    the_post.id = channel_posts.empty() ? 1 : channel_posts.back().id + 1;
    the_post.author_id = input_author->id;
    the_post.author_name = input_author->name;
    the_post.title = input_title;
    the_post.message = input_message;
    the_post.image_address = input_image_address;
    channel_posts.push_back(the_post);
    send_notifications(NEW_COURSE_POST_notif, input_author);
    return Ok;
}

void Class::send_notifications(string input_subject, User *excluded_user)
{
    std::set<string> notified_ids;
    auto notify_if_needed = [&](User *receiver)
    {
        if (receiver == nullptr)
            return;
        if (excluded_user != nullptr && receiver->id == excluded_user->id)
            return;
        if (notified_ids.insert(receiver->id).second)
            receiver->receive_notification(id, course->name, input_subject);
    };
    notify_if_needed(teacher);
    for (auto student : students)
        notify_if_needed(student);
    for (auto ta : teacher_assistants)
        notify_if_needed(ta);
}

response Class::show_channel(User *input_user, vector<string> *output)
{
    if (!has_accsess(input_user, 1))
        return PermissionDenied;
    append_full_info(output);
    output->push_back(ENTER);
    for (int i = (int)channel_posts.size() - 1; i >= 0; i--)
    {
        output->push_back(int_to_str(channel_posts[i].id));
        output->push_back(SPACE);
        output->push_back(channel_posts[i].author_name);
        output->push_back(SPACE);
        output->push_back(quote_text(channel_posts[i].title));
        output->push_back(ENTER);
    }
    return JustInformation;
}

response Class::show_post(vector<string> *output, int input_id)
{
    for (int i = 0; i < (int)channel_posts.size(); i++)
        if (channel_posts[i].id == input_id)
        {
            output->push_back(int_to_str(channel_posts[i].id));
            output->push_back(SPACE);
            output->push_back(channel_posts[i].author_name);
            output->push_back(SPACE);
            output->push_back(quote_text(channel_posts[i].title));
            output->push_back(SPACE);
            output->push_back(quote_text(channel_posts[i].message));
            output->push_back(ENTER);
            return JustInformation;
        }
    return NotFound;
}

vector<CHANNEL_POST> Class::get_channel_posts() const
{
    return channel_posts;
}
