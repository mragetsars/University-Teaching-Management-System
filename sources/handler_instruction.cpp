#include "handler_instruction.hpp"

Instruction_Handler::Instruction_Handler(DATA input_data)
{
    majors = input_data.majors;
    courses = input_data.courses;
    is_login = false;
    system_manager = System_Manager(input_data.system_manager);
    for (auto s : input_data.students)
        students.push_back(Student(s));
    for (auto p : input_data.professors)
        professors.push_back(Professor(p));
    for (int s = 0; s < students.size(); s++)
        system_manager.new_connection(&students[s]);
    for (int p = 0; p < professors.size(); p++)
        system_manager.new_connection(&professors[p]);
}

bool Instruction_Handler::find_user_by_id(User **user_ptr, string id)
{
    if (system_manager.id == id)
        return *user_ptr = &system_manager;
    for (int s = 0; s < students.size(); s++)
        if (students[s].id == id)
            return *user_ptr = &students[s];
    for (int p = 0; p < professors.size(); p++)
        if (professors[p].id == id)
            return *user_ptr = &professors[p];
    return false;
}
bool Instruction_Handler::find_class_by_id(Class **class_ptr, string id)
{
    for (int p = 0; p < classes.size(); p++)
        if (classes[p].id == id)
            return *class_ptr = &classes[p];
    return false;
}
bool Instruction_Handler::check_class_detail(User *input_teacher, string input_class_time)
{
    TIME new_class_time = string_to_time(input_class_time);
    for (auto c : classes)
        if (c.teacher == input_teacher)
            if (c.class_time.week == new_class_time.week)
                if ((c.class_time.start < new_class_time.end &&
                     c.class_time.end >= new_class_time.end) ||
                    (c.class_time.start <= new_class_time.start &&
                     c.class_time.end > new_class_time.start))
                    return false;
    if (new_class_time.start >= new_class_time.end)
        return false;
    return true;
}

ANSWER Instruction_Handler::POST_login(vector<string> input)
{
    ANSWER output;
    output.output = BadRequest;
    if (is_login)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = NotFound;
    User *user_ptr;
    if (find_user_by_id(&user_ptr, input[1]))
    {
        output.output = user_ptr->login(input[1], input[3]);
        if (output.output == Ok)
        {
            user = user_ptr;
            is_login = true;
        }
        return output;
    }
    return output;
}
ANSWER Instruction_Handler::POST_logout()
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    is_login = false;
    output.output = Ok;
    return output;
}
ANSWER Instruction_Handler::GET_courses(vector<string> input)
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    if (dynamic_cast<System_Manager *>(user) != nullptr)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = NotFound;
    if (input[0] == EMPTYSTRING)
        if (classes.size() == 0)
            output.output = Empty;
        else
            for (auto c : classes)
            {
                output.output = c.show_info(&output.info);
                output.info.push_back(ENTER);
            }
    else
        for (auto c : classes)
            if (c.id == input[1])
            {
                output.output = c.show_info(&output.info);
                output.info.push_back(SPACE);
                output.output = c.show_page(&output.info);
                output.info.push_back(ENTER);
            }
    return output;
}
ANSWER Instruction_Handler::POST_post(vector<string> input)
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = user->new_post(input[1], input[3], input[5]);
    return output;
}
ANSWER Instruction_Handler::DELETE_post(vector<string> input)
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = user->delete_post(str_to_int(input[1]));
    return output;
}
ANSWER Instruction_Handler::GET_personal_page(vector<string> input)
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    if (dynamic_cast<System_Manager *>(user) != nullptr)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = NotFound;
    User *user_ptr;
    if (find_user_by_id(&user_ptr, input[1]))
    {
        output.output = user_ptr->show_info(&output.info, &majors);
        output.output = user_ptr->show_page(&output.info);
    }
    return output;
}
ANSWER Instruction_Handler::GET_post(vector<string> input)
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    if (dynamic_cast<System_Manager *>(user) != nullptr)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = NotFound;
    User *user_ptr;
    if (find_user_by_id(&user_ptr, input[1]))
    {
        output.output = user_ptr->show_info(&output.info, &majors);
        output.output = user_ptr->show_post(&output.info, str_to_int(input[3]));
    }
    return output;
}
ANSWER Instruction_Handler::POST_connect(vector<string> input)
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    if (dynamic_cast<System_Manager *>(user) != nullptr)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = NotFound;
    User *user_ptr;
    if (find_user_by_id(&user_ptr, input[1]))
        output.output = user->new_connection(user_ptr);
    return output;
}
ANSWER Instruction_Handler::GET_notification()
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    if (dynamic_cast<System_Manager *>(user) != nullptr)
        return output;
    output.output = user->show_notifications(&output.info);
    return output;
}
ANSWER Instruction_Handler::POST_course_offer(vector<string> input)
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    if (dynamic_cast<System_Manager *>(user) == nullptr)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = NotFound;
    User *user_ptr;
    for (int c = 0; c < courses.size(); c++)
        if (courses[c].id == input[1] &&
            find_user_by_id(&user_ptr, input[3]))
        {
            output.output = PermissionDenied;
            if (dynamic_cast<Professor *>(user_ptr) == nullptr)
                return output;
            for (auto m : courses[c].major_ids)
                if (m == dynamic_cast<Professor *>(user_ptr)->major_id &&
                    check_class_detail(user_ptr, input[7]))
                {
                    classes.push_back(Class(int_to_str(classes.size() + 1),
                                            &courses[c],
                                            str_to_int(input[5]),
                                            user_ptr,
                                            input[7],
                                            input[9],
                                            input[11]));
                    system_manager.send_notifications(NEW_COURSE_OFFERING_notif);
                    output.output = Ok;
                    return output;
                }
        }
    return output;
}
ANSWER Instruction_Handler::PUT_my_courses(vector<string> input)
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    if (dynamic_cast<Student *>(user) == nullptr)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = NotFound;
    for (int c = 0; c < classes.size(); c++)
        if (classes[c].id == input[1])
            if (classes[c].new_student(user))
                output.output = dynamic_cast<Student *>(user)->new_class(&classes[c]);
    return output;
}
ANSWER Instruction_Handler::DELETE_my_courses(vector<string> input)
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    if (dynamic_cast<Student *>(user) == nullptr)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = dynamic_cast<Student *>(user)->delete_class(input[1]);
    return output;
}
ANSWER Instruction_Handler::GET_my_courses()
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    if (dynamic_cast<Student *>(user) == nullptr)
        return output;
    output.output = dynamic_cast<Student *>(user)->show_class(&output.info);
    return output;
}
ANSWER Instruction_Handler::POST_profile_photo(vector<string> input)
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    if (dynamic_cast<System_Manager *>(user) != nullptr)
        return output;
    output.output = user->set_profile_photo(input[1]);
    return output;
}
ANSWER Instruction_Handler::POST_course_post(vector<string> input)
{
    ANSWER output;
    if (!is_login)
        return output;
    if (dynamic_cast<System_Manager *>(user) != nullptr)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = NotFound;
    Class *class_ptr;
    if (find_class_by_id(&class_ptr, input[1]))
        output.output = class_ptr->new_post(user, input[3], input[5], input[7]);
    return output;
}
ANSWER Instruction_Handler::GET_course_channel(vector<string> input)
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    if (dynamic_cast<System_Manager *>(user) != nullptr)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = NotFound;
    Class *class_ptr;
    if (find_class_by_id(&class_ptr, input[1]))
        output.output = class_ptr->show_channel(user, &output.info);
    return output;
}
ANSWER Instruction_Handler::GET_course_post(vector<string> input)
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    if (dynamic_cast<System_Manager *>(user) != nullptr)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = NotFound;
    Class *class_ptr;
    if (find_class_by_id(&class_ptr, input[1]))
        if (class_ptr->has_accsess(user, 1))
        {
            output.output = class_ptr->show_info(&output.info);
            output.output = class_ptr->show_post(&output.info, str_to_int(input[1]));
        }
    return output;
}
ANSWER Instruction_Handler::POST_ta_form(vector<string> input)
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    if (dynamic_cast<Professor *>(user) == nullptr)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = NotFound;
    Class *class_ptr;
    if (!find_class_by_id(&class_ptr, input[1]))
        return output;
    output.output = dynamic_cast<Professor *>(user)->new_form(class_ptr, input[5]);
    return output;
}

ANSWER Instruction_Handler::POST_close_ta_form(vector<string> input)
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    if (dynamic_cast<Professor *>(user) == nullptr)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = dynamic_cast<Professor *>(user)->close_form(str_to_int(input[1]));
    return output;
}

ANSWER Instruction_Handler::POST_ta_request(vector<string> input)
{
    ANSWER output;
    output.output = PermissionDenied;
    if (!is_login)
        return output;
    if (dynamic_cast<Student *>(user) == nullptr)
        return output;
    output.output = BadRequest;
    if (input[0] == INFO_ERROR)
        return output;
    output.output = NotFound;
    User *user_ptr;
    if (find_user_by_id(&user_ptr, input[1]))
        if (dynamic_cast<Professor *>(user_ptr) == nullptr)
            return output;
        else
            dynamic_cast<Professor *>(user_ptr)->new_request(user, str_to_int(input[3]));
    return output;
}

ANSWER Instruction_Handler::run(QUESTION input)
{
    ANSWER output;
    output.output = JustInformation;
    if (!input.invalid)
        return output;
    output.output = Ok;
    switch (input.type)
    {
    case Get_Type:
        switch (input.input)
        {
        case Courses_Input:
            return GET_courses(input.info);
            break;
        case Personal_Page_Input:
            return GET_personal_page(input.info);
            break;
        case Post_Input:
            return GET_post(input.info);
            break;
        case Notification_Input:
            return GET_notification();
            break;
        case My_Courses_Input:
            return GET_my_courses();
            break;
        case Course_Channel_Input:
            return GET_course_channel(input.info);
            break;
        case Course_Post_Input:
            return GET_course_post(input.info);
            break;
        }
        break;
    case Put_Type:
        switch (input.input)
        {
        case My_Courses_Input:
            return PUT_my_courses(input.info);
            break;
        }
        break;
    case Post_Type:
        switch (input.input)
        {
        case Login_Input:
            return POST_login(input.info);
            break;
        case Logout_Input:
            return POST_logout();
            break;
        case Post_Input:
            return POST_post(input.info);
            break;
        case Connect_Input:
            return POST_connect(input.info);
            break;
        case Course_Offer_Input:
            return POST_course_offer(input.info);
            break;
        case Profile_Photo_Input:
            return POST_profile_photo(input.info);
            break;
        case Course_Post_Input:
            return POST_course_post(input.info);
            break;
        case Ta_Form_Input:
            return POST_ta_form(input.info);
            break;
        case Close_Ta_Form_Input:
            return POST_close_ta_form(input.info);
            break;
        case Ta_Request_Input:
            return POST_ta_request(input.info);
            break;
        }
        break;
    case Delete_Type:
        switch (input.input)
        {
        case Post_Input:
            return DELETE_post(input.info);
            break;
        case My_Courses_Input:
            return DELETE_my_courses(input.info);
            break;
        }
        break;
    }
    return output;
}