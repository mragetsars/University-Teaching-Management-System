#include "class_student.hpp"

Student::Student(STUDENT input_info)
    : User(input_info.info)
{
    major_id = input_info.info.major_id;
    semester = input_info.semester;
}
Student::Student() {}

response Student::show_info(vector<string> *output, vector<MAJOR> *majors)
{
    output->push_back(name);
    output->push_back(SPACE);
    for (auto major : *majors)
        if (major.id == major_id)
        {
            output->push_back(major.name);
            break;
        }
    output->push_back(SPACE);
    output->push_back(int_to_str(semester));
    if (!classes.empty())
    {
        output->push_back(SPACE);
        for (int i = 0; i < (int)classes.size(); i++)
        {
            if (i != 0)
                output->push_back(string(1, COMMA));
            output->push_back(classes[i]->course->name);
        }
    }
    output->push_back(ENTER);
    return JustInformation;
}

bool Student::has_class(string input_id) const
{
    for (auto input_class : classes)
        if (input_class->id == input_id)
            return true;
    return false;
}

response Student::new_class(Class *input_new_class)
{
    if (input_new_class == nullptr)
        return NotFound;
    if (has_class(input_new_class->id) || input_new_class->is_full())
        return PermissionDenied;

    bool major_allowed = false;
    for (auto major : input_new_class->course->major_ids)
        if (major == major_id)
            major_allowed = true;
    if (!major_allowed)
        return PermissionDenied;

    if (semester < input_new_class->course->prerequisite)
        return PermissionDenied;

    for (auto current_class : classes)
        if (times_overlap(current_class->class_time, input_new_class->class_time) ||
            dates_equal(current_class->exam_date, input_new_class->exam_date))
            return PermissionDenied;

    if (!input_new_class->new_student(this))
        return PermissionDenied;
    classes.push_back(input_new_class);
    send_notifications(GET_COURSE_notif);
    return Ok;
}

response Student::delete_class(string input_id)
{
    for (int i = 0; i < (int)classes.size(); i++)
        if (classes[i]->id == input_id)
        {
            if (!classes[i]->delete_student(id))
                return NotFound;
            classes.erase(classes.begin() + i);
            send_notifications(DELETE_COURSE_notif);
            return Ok;
        }
    return NotFound;
}

response Student::show_class(vector<string> *output)
{
    if (classes.empty())
        return Empty;
    for (auto input_class : classes)
    {
        input_class->append_full_info(output);
        output->push_back(ENTER);
    }
    return JustInformation;
}
