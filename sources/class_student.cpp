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
    output->push_back((*majors)[str_to_int(major_id) - 1].name);
    output->push_back(SPACE);
    output->push_back(int_to_str(semester));
    output->push_back(SPACE);
    if (classes.size() != 0)
        for (int i = 0; i < classes.size(); i++)
        {
            if (i != 0)
                output->push_back(string(1, COMMA));
            output->push_back(classes[i]->course->name);
        }
    output->push_back(ENTER);
    return JustInformation;
}

response Student::new_class(Class *input_new_class)
{
    for (auto m : input_new_class->course->major_ids)
        if (m == major_id)
        {
            if (semester < input_new_class->course->prerequisite)
                return PermissionDenied;
            for (auto c : classes)
                if (c->class_time.week == input_new_class->class_time.week)
                    if (((c->class_time.start < input_new_class->class_time.end &&
                          c->class_time.end >= input_new_class->class_time.end) ||
                         (c->class_time.start <= input_new_class->class_time.start &&
                          c->class_time.end > input_new_class->class_time.start)) ||
                        ((c->exam_date.day == input_new_class->exam_date.day) &&
                         (c->exam_date.month == input_new_class->exam_date.month) &&
                         (c->exam_date.year == input_new_class->exam_date.year)))
                        return PermissionDenied;
            classes.push_back(input_new_class);
            return Ok;
        }
    return PermissionDenied;
}
response Student::delete_class(string input_id)
{
    int n = -1;
    for (int i = 0; i < classes.size(); i++)
        if (classes[i]->id == input_id)
            n = i;
    if (n == -1)
        return NotFound;
    if (!classes[n]->delete_student(id))
        return NotFound;
    else
        classes.erase(classes.begin() + n);
    return Ok;
}
response Student::show_class(vector<string> *output)
{
    if (classes.size() == 0)
        return Empty;
    for (int i = 0; i < classes.size(); i++)
    {
        classes[i]->show_info(output);
        classes[i]->show_page(output);
    }
    return JustInformation;
}