#include "class_professor.hpp"

Professor::Professor(PROFESSOR input_info)
    : User(input_info.info)
{
    major_id = input_info.info.major_id;
    position = input_info.position;
}
Professor::Professor() {}

response Professor::show_info(vector<string> *output, vector<MAJOR> *majors)
{
    output->push_back(name);
    output->push_back(SPACE);
    output->push_back((*majors)[str_to_int(major_id) - 1].name);
    output->push_back(SPACE);
    output->push_back(pos_to_str(position));
    output->push_back(ENTER);
    return JustInformation;
}

response Professor::new_form(Class *input_class, string input_message)
{
    POST the_post;
    if (posts.size() != 0)
        the_post.id = posts[posts.size() - 1].id + 1;
    else
        the_post.id = 1;
    string input_title = TA_FORM_FOR + input_class->course->name + SPACE + COURSE_;
    input_message = input_class->id + SPACE + input_class->course->name + SPACE + int_to_str(input_class->capacity) + SPACE + input_class->teacher->name + SPACE + day_to_str(input_class->class_time.week) + string(1, COLON) + int_to_str(input_class->class_time.start) + string(1, DASH) + int_to_str(input_class->class_time.end) + SPACE + int_to_str(input_class->exam_date.year) + string(1, SLAH) + int_to_str(input_class->exam_date.month) + string(1, SLAH) + int_to_str(input_class->exam_date.day) + SPACE + int_to_str(input_class->claee_number) + ENTER + input_message;
    the_post.title = input_title;
    the_post.message = input_message;
    posts.push_back(the_post);
    FORM the_form;
    the_form.id = posts[posts.size() - 1].id;
    the_form.form_class = input_class;
    forms.push_back(the_form);
    send_notifications(NEW_FORM_notif);
    return Ok;
}
response Professor::new_request(User *input_user, int input_id)
{
    int n = -1;
    for (int i = 0; i < forms.size(); i++)
        if (forms[i].id == input_id)
            n = i;
    if (n == -1)
        return NotFound;
    forms[n].requests.push_back(input_user);
    return Ok;
}
response Professor::close_form(int input_id)
{
    int n = -1;
    for (int i = 0; i < posts.size(); i++)
        if (posts[i].id == input_id)
            n = i;
    if (n == -1)
        return NotFound;
    else
        posts.erase(posts.begin() + n);
    n = -1;
    for (int i = 0; i < forms.size(); i++)
        if (forms[i].id == input_id)
            n = i;
    if (n == -1)
        return NotFound;
    string answer;
    int r;
    while (r < forms[n].requests.size())
    {
        cout << forms[n].requests[r]->id << SPACE << forms[n].requests[r]->name << SPACE << dynamic_cast<Student *>(forms[n].requests[r])->semester << string(1, COLON) << SPACE;
        cin >> answer;
        if (answer == "accepted.")
        {
            forms[n].form_class->send_notifications(CLOSE_FORM_notif + answer);
            forms[n].form_class->new_teacher_assistant(forms[n].requests[r]);
            r++;
        }
        if (answer == "rejected")
        {
            forms[n].form_class->send_notifications(CLOSE_FORM_notif + answer);
            r++;
        }
    }
    forms.erase(forms.begin() + n);
    return Ok;
}