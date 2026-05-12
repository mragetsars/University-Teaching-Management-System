#include "handler_instruction.hpp"

Instruction_Handler::Instruction_Handler(DATA input_data)
{
    majors = input_data.majors;
    courses = input_data.courses;
    is_login = false;
    user = nullptr;
    system_manager = System_Manager(input_data.system_manager);
    for (auto s : input_data.students)
        students.push_back(Student(s));
    for (auto p : input_data.professors)
        professors.push_back(Professor(p));

    for (auto &student : students)
        system_manager.add_connection_one_way(&student);
    for (auto &professor : professors)
        system_manager.add_connection_one_way(&professor);
}

bool Instruction_Handler::logged_in() const
{
    return is_login;
}

string Instruction_Handler::current_user_id() const
{
    return user == nullptr ? EMPTYSTRING : user->id;
}

string Instruction_Handler::current_user_name() const
{
    return user == nullptr ? EMPTYSTRING : user->name;
}

bool Instruction_Handler::find_user_by_id(User **user_ptr, string id)
{
    if (system_manager.id == id)
    {
        *user_ptr = &system_manager;
        return true;
    }
    for (auto &student : students)
        if (student.id == id)
        {
            *user_ptr = &student;
            return true;
        }
    for (auto &professor : professors)
        if (professor.id == id)
        {
            *user_ptr = &professor;
            return true;
        }
    return false;
}

bool Instruction_Handler::find_class_by_id(Class **class_ptr, string id)
{
    for (auto &input_class : classes)
        if (input_class.id == id)
        {
            *class_ptr = &input_class;
            return true;
        }
    return false;
}

bool Instruction_Handler::find_course_by_id(COURSE **course_ptr, string id)
{
    for (auto &course : courses)
        if (course.id == id)
        {
            *course_ptr = &course;
            return true;
        }
    return false;
}

bool Instruction_Handler::check_class_detail(User *input_teacher, string input_class_time)
{
    TIME new_class_time = string_to_time(input_class_time);
    if (new_class_time.start >= new_class_time.end)
        return false;
    for (auto &input_class : classes)
        if (input_class.teacher == input_teacher && times_overlap(input_class.class_time, new_class_time))
            return false;
    return true;
}

void Instruction_Handler::broadcast_notification(User *sender, string subject)
{
    if (sender == nullptr)
        return;
    for (auto &student : students)
        student.receive_notification(sender->id, sender->name, subject);
    for (auto &professor : professors)
        professor.receive_notification(sender->id, sender->name, subject);
}

ANSWER Instruction_Handler::POST_login(vector<string> input)
{
    ANSWER output;
    if (is_login)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    User *user_ptr;
    if (!find_user_by_id(&user_ptr, input[1]))
    {
        output.output = NotFound;
        return output;
    }
    output.output = user_ptr->login(input[1], input[3]);
    if (output.output == Ok)
    {
        user = user_ptr;
        is_login = true;
    }
    return output;
}

ANSWER Instruction_Handler::POST_logout()
{
    ANSWER output;
    if (!is_login)
    {
        output.output = PermissionDenied;
        return output;
    }
    is_login = false;
    user = nullptr;
    output.output = Ok;
    return output;
}

ANSWER Instruction_Handler::GET_courses(vector<string> input)
{
    ANSWER output;
    if (!is_login || dynamic_cast<System_Manager *>(user) != nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    if (input[0] == EMPTYSTRING)
    {
        if (classes.empty())
            output.output = Empty;
        else
        {
            output.output = JustInformation;
            for (auto &input_class : classes)
            {
                input_class.show_info(&output.info);
                output.info.push_back(ENTER);
            }
        }
        return output;
    }
    Class *class_ptr;
    if (!find_class_by_id(&class_ptr, input[1]))
    {
        output.output = NotFound;
        return output;
    }
    output.output = JustInformation;
    class_ptr->append_full_info(&output.info);
    output.info.push_back(ENTER);
    return output;
}

ANSWER Instruction_Handler::POST_post(vector<string> input)
{
    ANSWER output;
    if (!is_login)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    output.output = user->new_post(input[1], input[3], input[5]);
    return output;
}

ANSWER Instruction_Handler::DELETE_post(vector<string> input)
{
    ANSWER output;
    if (!is_login)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    output.output = user->delete_post(str_to_int(input[1]));
    return output;
}

ANSWER Instruction_Handler::GET_personal_page(vector<string> input)
{
    ANSWER output;
    if (!is_login || dynamic_cast<System_Manager *>(user) != nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    User *user_ptr;
    if (!find_user_by_id(&user_ptr, input[1]))
    {
        output.output = NotFound;
        return output;
    }
    output.output = JustInformation;
    user_ptr->show_info(&output.info, &majors);
    user_ptr->show_page(&output.info);
    return output;
}

ANSWER Instruction_Handler::GET_post(vector<string> input)
{
    ANSWER output;
    if (!is_login || dynamic_cast<System_Manager *>(user) != nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    User *user_ptr;
    if (!find_user_by_id(&user_ptr, input[1]))
    {
        output.output = NotFound;
        return output;
    }
    output.output = JustInformation;
    user_ptr->show_info(&output.info, &majors);
    response post_response = user_ptr->show_post(&output.info, str_to_int(input[3]));
    if (post_response != JustInformation)
    {
        output.info.clear();
        output.output = post_response;
    }
    return output;
}

ANSWER Instruction_Handler::POST_connect(vector<string> input)
{
    ANSWER output;
    if (!is_login || dynamic_cast<System_Manager *>(user) != nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    User *user_ptr;
    if (!find_user_by_id(&user_ptr, input[1]))
    {
        output.output = NotFound;
        return output;
    }
    output.output = user->new_connection(user_ptr);
    return output;
}

ANSWER Instruction_Handler::GET_notification()
{
    ANSWER output;
    if (!is_login || dynamic_cast<System_Manager *>(user) != nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    output.output = user->show_notifications(&output.info);
    return output;
}

ANSWER Instruction_Handler::POST_course_offer(vector<string> input)
{
    ANSWER output;
    if (!is_login || dynamic_cast<System_Manager *>(user) == nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }

    COURSE *course_ptr;
    User *user_ptr;
    if (!find_course_by_id(&course_ptr, input[1]) || !find_user_by_id(&user_ptr, input[3]))
    {
        output.output = NotFound;
        return output;
    }
    Professor *professor_ptr = dynamic_cast<Professor *>(user_ptr);
    if (professor_ptr == nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }

    bool major_allowed = false;
    for (auto major_id : course_ptr->major_ids)
        if (major_id == professor_ptr->major_id)
            major_allowed = true;
    if (!major_allowed || !check_class_detail(user_ptr, input[7]))
    {
        output.output = PermissionDenied;
        return output;
    }

    classes.push_back(Class(int_to_str(classes.size() + 1), course_ptr, str_to_int(input[5]), user_ptr, input[7], input[9], input[11]));
    professor_ptr->offer_class(&classes.back());
    broadcast_notification(user_ptr, NEW_COURSE_OFFERING_notif);
    output.output = Ok;
    return output;
}

ANSWER Instruction_Handler::PUT_my_courses(vector<string> input)
{
    ANSWER output;
    if (!is_login || dynamic_cast<Student *>(user) == nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    Class *class_ptr;
    if (!find_class_by_id(&class_ptr, input[1]))
    {
        output.output = NotFound;
        return output;
    }
    output.output = dynamic_cast<Student *>(user)->new_class(class_ptr);
    return output;
}

ANSWER Instruction_Handler::DELETE_my_courses(vector<string> input)
{
    ANSWER output;
    if (!is_login || dynamic_cast<Student *>(user) == nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    output.output = dynamic_cast<Student *>(user)->delete_class(input[1]);
    return output;
}

ANSWER Instruction_Handler::GET_my_courses()
{
    ANSWER output;
    if (!is_login || dynamic_cast<Student *>(user) == nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    output.output = dynamic_cast<Student *>(user)->show_class(&output.info);
    return output;
}

ANSWER Instruction_Handler::POST_profile_photo(vector<string> input)
{
    ANSWER output;
    if (!is_login || dynamic_cast<System_Manager *>(user) != nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    output.output = user->set_profile_photo(input[1]);
    return output;
}

ANSWER Instruction_Handler::POST_course_post(vector<string> input)
{
    ANSWER output;
    if (!is_login || dynamic_cast<System_Manager *>(user) != nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    Class *class_ptr;
    if (!find_class_by_id(&class_ptr, input[1]))
    {
        output.output = NotFound;
        return output;
    }
    output.output = class_ptr->new_post(user, input[3], input[5], input[7]);
    return output;
}

ANSWER Instruction_Handler::GET_course_channel(vector<string> input)
{
    ANSWER output;
    if (!is_login || dynamic_cast<System_Manager *>(user) != nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    Class *class_ptr;
    if (!find_class_by_id(&class_ptr, input[1]))
    {
        output.output = NotFound;
        return output;
    }
    output.output = class_ptr->show_channel(user, &output.info);
    return output;
}

ANSWER Instruction_Handler::GET_course_post(vector<string> input)
{
    ANSWER output;
    if (!is_login || dynamic_cast<System_Manager *>(user) != nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    Class *class_ptr;
    if (!find_class_by_id(&class_ptr, input[1]))
    {
        output.output = NotFound;
        return output;
    }
    if (!class_ptr->has_accsess(user, 1))
    {
        output.output = PermissionDenied;
        return output;
    }
    class_ptr->append_full_info(&output.info);
    output.info.push_back(ENTER);
    response post_response = class_ptr->show_post(&output.info, str_to_int(input[3]));
    if (post_response != JustInformation)
    {
        output.info.clear();
        output.output = post_response;
    }
    else
        output.output = JustInformation;
    return output;
}

ANSWER Instruction_Handler::POST_ta_form(vector<string> input)
{
    ANSWER output;
    if (!is_login || dynamic_cast<Professor *>(user) == nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    Class *class_ptr;
    if (!find_class_by_id(&class_ptr, input[1]))
    {
        output.output = NotFound;
        return output;
    }
    output.output = dynamic_cast<Professor *>(user)->new_form(class_ptr, input[3]);
    return output;
}

ANSWER Instruction_Handler::POST_close_ta_form(vector<string> input)
{
    ANSWER output;
    if (!is_login || dynamic_cast<Professor *>(user) == nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    output.output = dynamic_cast<Professor *>(user)->close_form(str_to_int(input[1]));
    return output;
}

ANSWER Instruction_Handler::POST_ta_request(vector<string> input)
{
    ANSWER output;
    if (!is_login || dynamic_cast<Student *>(user) == nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    if (input[0] == INFO_ERROR)
    {
        output.output = BadRequest;
        return output;
    }
    User *user_ptr;
    if (!find_user_by_id(&user_ptr, input[1]) || dynamic_cast<Professor *>(user_ptr) == nullptr)
    {
        output.output = NotFound;
        return output;
    }
    output.output = dynamic_cast<Professor *>(user_ptr)->new_request(dynamic_cast<Student *>(user), str_to_int(input[3]));
    return output;
}

ANSWER Instruction_Handler::run(QUESTION input)
{
    ANSWER output;
    if (!input.invalid)
    {
        output.output = JustInformation;
        return output;
    }
    switch (input.type)
    {
    case Get_Type:
        switch (input.input)
        {
        case Courses_Input:
            return GET_courses(input.info);
        case Personal_Page_Input:
            return GET_personal_page(input.info);
        case Post_Input:
            return GET_post(input.info);
        case Notification_Input:
            return GET_notification();
        case My_Courses_Input:
            return GET_my_courses();
        case Course_Channel_Input:
            return GET_course_channel(input.info);
        case Course_Post_Input:
            return GET_course_post(input.info);
        default:
            break;
        }
        break;
    case Put_Type:
        if (input.input == My_Courses_Input)
            return PUT_my_courses(input.info);
        break;
    case Post_Type:
        switch (input.input)
        {
        case Login_Input:
            return POST_login(input.info);
        case Logout_Input:
            return POST_logout();
        case Post_Input:
            return POST_post(input.info);
        case Connect_Input:
            return POST_connect(input.info);
        case Course_Offer_Input:
            return POST_course_offer(input.info);
        case Profile_Photo_Input:
            return POST_profile_photo(input.info);
        case Course_Post_Input:
            return POST_course_post(input.info);
        case Ta_Form_Input:
            return POST_ta_form(input.info);
        case Close_Ta_Form_Input:
            return POST_close_ta_form(input.info);
        case Ta_Request_Input:
            return POST_ta_request(input.info);
        default:
            break;
        }
        break;
    case Delete_Type:
        switch (input.input)
        {
        case Post_Input:
            return DELETE_post(input.info);
        case My_Courses_Input:
            return DELETE_my_courses(input.info);
        default:
            break;
        }
        break;
    default:
        break;
    }
    output.output = NotFound;
    return output;
}

bool Instruction_Handler::current_user_is_student() const
{
    return dynamic_cast<Student *>(user) != nullptr;
}

bool Instruction_Handler::current_user_is_professor() const
{
    return dynamic_cast<Professor *>(user) != nullptr;
}

bool Instruction_Handler::current_user_is_system_manager() const
{
    return dynamic_cast<System_Manager *>(user) != nullptr;
}

string Instruction_Handler::current_user_type_label() const
{
    if (current_user_is_student())
        return "Student";
    if (current_user_is_professor())
        return "Professor";
    if (current_user_is_system_manager())
        return "System Manager";
    return "Guest";
}

static string major_name_from_id(const vector<MAJOR> &majors, const string &major_id)
{
    for (auto major : majors)
        if (major.id == major_id)
            return major.name;
    return EMPTYSTRING;
}

static WEB_CLASS_VIEW make_class_view(Class &input_class, User *current_user)
{
    WEB_CLASS_VIEW view;
    view.id = input_class.id;
    view.name = input_class.course == nullptr ? EMPTYSTRING : input_class.course->name;
    view.capacity = int_to_str(input_class.capacity);
    view.professor_name = input_class.teacher == nullptr ? EMPTYSTRING : input_class.teacher->name;
    view.professor_id = input_class.teacher == nullptr ? EMPTYSTRING : input_class.teacher->id;
    view.time = time_to_string(input_class.class_time);
    view.exam_date = date_to_string(input_class.exam_date);
    view.class_number = int_to_str(input_class.class_number);
    view.current_user_can_view = current_user != nullptr && input_class.has_accsess(current_user, 1);
    view.current_user_can_post = current_user != nullptr && input_class.has_accsess(current_user, 2);
    view.channel_posts = input_class.get_channel_posts();
    return view;
}

vector<WEB_USER_MINI_VIEW> Instruction_Handler::web_all_users()
{
    vector<WEB_USER_MINI_VIEW> output;
    output.push_back({system_manager.id, system_manager.name, "System Manager"});
    for (auto &student : students)
        output.push_back({student.id, student.name, "Student"});
    for (auto &professor : professors)
        output.push_back({professor.id, professor.name, "Professor"});
    return output;
}

vector<WEB_COURSE_DEF_VIEW> Instruction_Handler::web_course_definitions()
{
    vector<WEB_COURSE_DEF_VIEW> output;
    for (auto &course : courses)
        output.push_back({course.id, course.name, course.credit, course.prerequisite, course.major_ids});
    return output;
}

vector<WEB_USER_MINI_VIEW> Instruction_Handler::web_professors()
{
    vector<WEB_USER_MINI_VIEW> output;
    for (auto &professor : professors)
        output.push_back({professor.id, professor.name, "Professor"});
    return output;
}

vector<WEB_CLASS_VIEW> Instruction_Handler::web_all_classes()
{
    vector<WEB_CLASS_VIEW> output;
    for (auto &input_class : classes)
        output.push_back(make_class_view(input_class, user));
    return output;
}

vector<WEB_CLASS_VIEW> Instruction_Handler::web_my_student_classes()
{
    vector<WEB_CLASS_VIEW> output;
    if (!current_user_is_student())
        return output;
    for (auto &input_class : classes)
        if (input_class.has_student(user->id))
            output.push_back(make_class_view(input_class, user));
    return output;
}

bool Instruction_Handler::web_class_view(const string &id, WEB_CLASS_VIEW *output)
{
    Class *class_ptr = nullptr;
    if (!find_class_by_id(&class_ptr, id) || output == nullptr)
        return false;
    *output = make_class_view(*class_ptr, user);
    return true;
}

bool Instruction_Handler::web_user_view(const string &id, WEB_USER_VIEW *output)
{
    User *user_ptr = nullptr;
    if (!find_user_by_id(&user_ptr, id) || output == nullptr)
        return false;

    output->id = user_ptr->id;
    output->name = user_ptr->name;
    output->profile_photo_address = user_ptr->profile_photo();
    output->posts = user_ptr->posts;
    output->major = EMPTYSTRING;
    output->semester = EMPTYSTRING;
    output->position = EMPTYSTRING;
    output->course_names.clear();

    if (dynamic_cast<System_Manager *>(user_ptr) != nullptr)
    {
        output->type_label = "System Manager";
    }
    else if (Student *student_ptr = dynamic_cast<Student *>(user_ptr))
    {
        output->type_label = "Student";
        output->major = major_name_from_id(majors, student_ptr->major_id);
        output->semester = int_to_str(student_ptr->semester);
        for (auto &input_class : classes)
            if (input_class.has_student(student_ptr->id))
                output->course_names.push_back(input_class.course->name);
    }
    else if (Professor *professor_ptr = dynamic_cast<Professor *>(user_ptr))
    {
        output->type_label = "Professor";
        output->major = major_name_from_id(majors, professor_ptr->major_id);
        output->position = professor_ptr->position_name;
        for (auto &input_class : classes)
            if (input_class.teacher != nullptr && input_class.teacher->id == professor_ptr->id)
                output->course_names.push_back(input_class.course->name);
    }
    return true;
}

static WEB_TA_FORM_VIEW make_form_view(const Professor &professor, const FORM &form)
{
    WEB_TA_FORM_VIEW view;
    view.id = form.id;
    view.professor_id = professor.id;
    view.professor_name = professor.name;
    view.course_id = form.form_class == nullptr ? EMPTYSTRING : form.form_class->id;
    view.course_name = (form.form_class == nullptr || form.form_class->course == nullptr) ? EMPTYSTRING : form.form_class->course->name;
    view.message = form.message;
    for (auto request : form.requests)
        if (request != nullptr)
            view.requests.push_back({request->id, request->name, request->semester});
    return view;
}

vector<WEB_TA_FORM_VIEW> Instruction_Handler::web_all_ta_forms()
{
    vector<WEB_TA_FORM_VIEW> output;
    for (auto &professor : professors)
        for (auto &form : professor.forms)
            output.push_back(make_form_view(professor, form));
    return output;
}

vector<WEB_TA_FORM_VIEW> Instruction_Handler::web_my_ta_forms()
{
    vector<WEB_TA_FORM_VIEW> output;
    if (!current_user_is_professor())
        return output;
    for (auto &professor : professors)
        if (professor.id == user->id)
            for (auto &form : professor.forms)
                output.push_back(make_form_view(professor, form));
    return output;
}

ANSWER Instruction_Handler::web_close_ta_form(int form_id, const map<string, string> &decisions)
{
    ANSWER output;
    if (!is_login || dynamic_cast<Professor *>(user) == nullptr)
    {
        output.output = PermissionDenied;
        return output;
    }
    output.output = dynamic_cast<Professor *>(user)->close_form_with_decisions(form_id, decisions, &output.info);
    return output;
}
