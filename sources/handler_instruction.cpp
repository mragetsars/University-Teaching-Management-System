#include "handler_instruction.hpp"
#include "utilities.hpp"

#include <filesystem>

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
    auto finish = [&](ANSWER answer) {
        bool mutating_command = input.type == Put_Type || input.type == Delete_Type ||
                                (input.type == Post_Type && input.input != Login_Input) ||
                                (input.type == Get_Type && input.input == Notification_Input);
        if (mutating_command && (answer.output == Ok || answer.output == JustInformation || answer.output == Empty))
            persist_if_enabled();
        return answer;
    };

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
            return finish(GET_courses(input.info));
        case Personal_Page_Input:
            return finish(GET_personal_page(input.info));
        case Post_Input:
            return finish(GET_post(input.info));
        case Notification_Input:
            return finish(GET_notification());
        case My_Courses_Input:
            return finish(GET_my_courses());
        case Course_Channel_Input:
            return finish(GET_course_channel(input.info));
        case Course_Post_Input:
            return finish(GET_course_post(input.info));
        default:
            break;
        }
        break;
    case Put_Type:
        if (input.input == My_Courses_Input)
            return finish(PUT_my_courses(input.info));
        break;
    case Post_Type:
        switch (input.input)
        {
        case Login_Input:
            return finish(POST_login(input.info));
        case Logout_Input:
            return finish(POST_logout());
        case Post_Input:
            return finish(POST_post(input.info));
        case Connect_Input:
            return finish(POST_connect(input.info));
        case Course_Offer_Input:
            return finish(POST_course_offer(input.info));
        case Profile_Photo_Input:
            return finish(POST_profile_photo(input.info));
        case Course_Post_Input:
            return finish(POST_course_post(input.info));
        case Ta_Form_Input:
            return finish(POST_ta_form(input.info));
        case Close_Ta_Form_Input:
            return finish(POST_close_ta_form(input.info));
        case Ta_Request_Input:
            return finish(POST_ta_request(input.info));
        default:
            break;
        }
        break;
    case Delete_Type:
        switch (input.input)
        {
        case Post_Input:
            return finish(DELETE_post(input.info));
        case My_Courses_Input:
            return finish(DELETE_my_courses(input.info));
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
    if (output.output == Ok || output.output == JustInformation)
        persist_if_enabled();
    return output;
}

namespace
{
vector<string> split_state_line(const string &line)
{
    vector<string> parts;
    string current;
    for (char ch : line)
    {
        if (ch == '\t')
        {
            parts.push_back(current);
            current.clear();
        }
        else
        {
            current.push_back(ch);
        }
    }
    parts.push_back(current);
    return parts;
}

string enc_state(const string &value)
{
    return utils::urlEncode(value);
}

string dec_state(const string &value)
{
    return utils::urlDecode(value);
}

string join_state_ids(const vector<string> &ids)
{
    string out;
    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (i)
            out += ",";
        out += enc_state(ids[i]);
    }
    return out;
}

vector<string> split_state_ids(const string &value)
{
    vector<string> result;
    if (value.empty())
        return result;
    string current;
    for (char ch : value)
    {
        if (ch == ',')
        {
            result.push_back(dec_state(current));
            current.clear();
        }
        else
            current.push_back(ch);
    }
    result.push_back(dec_state(current));
    return result;
}
}

void Instruction_Handler::persist_if_enabled()
{
    if (!loading_state && !state_file_path.empty())
        save_state(state_file_path);
}

void Instruction_Handler::enable_state_persistence(const string &path, bool load_existing)
{
    state_file_path = path;
    if (load_existing && !path.empty())
        load_state(path);
}

string Instruction_Handler::current_state_file() const
{
    return state_file_path;
}

bool Instruction_Handler::save_state(const string &path) const
{
    if (path.empty())
        return false;
    try
    {
        std::filesystem::path state_path(path);
        if (!state_path.parent_path().empty())
            std::filesystem::create_directories(state_path.parent_path());
        ofstream out(path);
        if (!out)
            return false;
        out << "# UTMS_STATE_V1\n";

        auto write_user_state = [&](const User &input_user)
        {
            out << "PROFILE\t" << enc_state(input_user.id) << "\t" << enc_state(input_user.profile_photo_address) << "\n";
            for (const auto &post : input_user.posts)
            {
                out << "POST\t" << enc_state(input_user.id) << "\t" << post.id << "\t" << enc_state(post.title)
                    << "\t" << enc_state(post.message) << "\t" << enc_state(post.image_address) << "\t" << (post.is_ta_form ? "1" : "0")
                    << "\t" << enc_state(post.ta_course_id) << "\t" << enc_state(post.ta_course_name) << "\n";
            }
            for (const auto &target_id : input_user.connection_ids())
                if (input_user.id < target_id)
                    out << "CONNECTION\t" << enc_state(input_user.id) << "\t" << enc_state(target_id) << "\n";
            for (const auto &notification : input_user.pending_notifications())
                out << "NOTIFICATION\t" << enc_state(input_user.id) << "\t" << enc_state(notification.id) << "\t" << enc_state(notification.name) << "\t" << enc_state(notification.subject) << "\n";
        };

        write_user_state(system_manager);
        for (const auto &student : students)
            write_user_state(student);
        for (const auto &professor : professors)
            write_user_state(professor);

        for (const auto &input_class : classes)
        {
            out << "CLASS\t" << enc_state(input_class.id) << "\t" << enc_state(input_class.course == nullptr ? EMPTYSTRING : input_class.course->id)
                << "\t" << input_class.capacity << "\t" << enc_state(input_class.teacher == nullptr ? EMPTYSTRING : input_class.teacher->id)
                << "\t" << enc_state(time_to_string(input_class.class_time)) << "\t" << enc_state(date_to_string(input_class.exam_date))
                << "\t" << input_class.class_number << "\n";
            for (const auto &student_id : input_class.student_ids())
                out << "ENROLL\t" << enc_state(input_class.id) << "\t" << enc_state(student_id) << "\n";
            for (const auto &assistant_id : input_class.teacher_assistant_ids())
                out << "TA\t" << enc_state(input_class.id) << "\t" << enc_state(assistant_id) << "\n";
            for (const auto &post : input_class.get_channel_posts())
            {
                out << "CHANNEL_POST\t" << enc_state(input_class.id) << "\t" << post.id << "\t" << enc_state(post.author_id) << "\t"
                    << enc_state(post.author_name) << "\t" << enc_state(post.title) << "\t" << enc_state(post.message) << "\t" << enc_state(post.image_address) << "\n";
            }
        }

        for (const auto &professor : professors)
            for (const auto &form : professor.forms)
                out << "FORM\t" << enc_state(professor.id) << "\t" << form.id << "\t" << enc_state(form.form_class == nullptr ? EMPTYSTRING : form.form_class->id)
                    << "\t" << enc_state(form.message) << "\t" << join_state_ids([&]() {
                        vector<string> ids;
                        for (auto request : form.requests)
                            if (request != nullptr)
                                ids.push_back(request->id);
                        return ids;
                    }()) << "\n";
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool Instruction_Handler::load_state(const string &path)
{
    ifstream in(path);
    if (!in)
        return false;

    loading_state = true;
    string previous_user_id = current_user_id();
    bool was_login = is_login;
    is_login = false;
    user = nullptr;

    struct PendingClass
    {
        string id, course_id, professor_id, time, exam_date, class_number;
        int capacity = 0;
    };
    struct PendingForm
    {
        string professor_id, class_id, message;
        int id = 0;
        vector<string> request_ids;
    };
    vector<pair<string, string>> pending_enrollments;
    vector<pair<string, string>> pending_tas;
    vector<pair<string, string>> pending_connections;
    vector<PendingClass> pending_classes;
    vector<PendingForm> pending_forms;
    struct PendingChannelPost
    {
        string class_id;
        CHANNEL_POST post;
    };
    vector<PendingChannelPost> pending_channel_posts;

    string line;
    while (getline(in, line))
    {
        if (line.empty() || line[0] == '#')
            continue;
        vector<string> p = split_state_line(line);
        try
        {
            if (p[0] == "PROFILE" && p.size() >= 3)
            {
                User *target = nullptr;
                if (find_user_by_id(&target, dec_state(p[1])))
                    target->set_profile_photo(dec_state(p[2]));
            }
            else if (p[0] == "POST" && p.size() >= 9)
            {
                User *target = nullptr;
                if (find_user_by_id(&target, dec_state(p[1])))
                {
                    POST post;
                    post.id = str_to_int(p[2]);
                    post.title = dec_state(p[3]);
                    post.message = dec_state(p[4]);
                    post.image_address = dec_state(p[5]);
                    post.is_ta_form = p[6] == "1";
                    post.ta_course_id = dec_state(p[7]);
                    post.ta_course_name = dec_state(p[8]);
                    target->restore_post(post);
                }
            }
            else if (p[0] == "NOTIFICATION" && p.size() >= 5)
            {
                User *target = nullptr;
                if (find_user_by_id(&target, dec_state(p[1])))
                    target->restore_notification({dec_state(p[2]), dec_state(p[3]), dec_state(p[4])});
            }
            else if (p[0] == "CONNECTION" && p.size() >= 3)
            {
                pending_connections.push_back({dec_state(p[1]), dec_state(p[2])});
            }
            else if (p[0] == "CLASS" && p.size() >= 8)
            {
                pending_classes.push_back({dec_state(p[1]), dec_state(p[2]), dec_state(p[4]), dec_state(p[5]), dec_state(p[6]), dec_state(p[7]), str_to_int(p[3])});
            }
            else if (p[0] == "ENROLL" && p.size() >= 3)
            {
                pending_enrollments.push_back({dec_state(p[1]), dec_state(p[2])});
            }
            else if (p[0] == "TA" && p.size() >= 3)
            {
                pending_tas.push_back({dec_state(p[1]), dec_state(p[2])});
            }
            else if (p[0] == "CHANNEL_POST" && p.size() >= 8)
            {
                CHANNEL_POST post;
                post.id = str_to_int(p[2]);
                post.author_id = dec_state(p[3]);
                post.author_name = dec_state(p[4]);
                post.title = dec_state(p[5]);
                post.message = dec_state(p[6]);
                post.image_address = dec_state(p[7]);
                pending_channel_posts.push_back({dec_state(p[1]), post});
            }
            else if (p[0] == "FORM" && p.size() >= 6)
            {
                PendingForm form;
                form.professor_id = dec_state(p[1]);
                form.id = str_to_int(p[2]);
                form.class_id = dec_state(p[3]);
                form.message = dec_state(p[4]);
                form.request_ids = split_state_ids(p[5]);
                pending_forms.push_back(form);
            }
        }
        catch (...)
        {
            // Corrupted state lines are ignored so a bad optional state file cannot prevent startup.
        }
    }

    for (const auto &pending : pending_classes)
    {
        Class *existing = nullptr;
        if (find_class_by_id(&existing, pending.id))
            continue;
        COURSE *course = nullptr;
        User *professor = nullptr;
        if (find_course_by_id(&course, pending.course_id) && find_user_by_id(&professor, pending.professor_id))
        {
            classes.emplace_back(pending.id, course, pending.capacity, professor, pending.time, pending.exam_date, pending.class_number);
            if (auto prof = dynamic_cast<Professor *>(professor))
                prof->offer_class(&classes.back());
        }
    }

    for (const auto &enrollment : pending_enrollments)
    {
        Class *class_ptr = nullptr;
        User *student_user = nullptr;
        if (find_class_by_id(&class_ptr, enrollment.first) && find_user_by_id(&student_user, enrollment.second))
            if (auto student = dynamic_cast<Student *>(student_user))
            {
                class_ptr->restore_student(student_user);
                student->restore_class(class_ptr);
            }
    }

    for (const auto &assistant : pending_tas)
    {
        Class *class_ptr = nullptr;
        User *student_user = nullptr;
        if (find_class_by_id(&class_ptr, assistant.first) && find_user_by_id(&student_user, assistant.second))
            class_ptr->restore_teacher_assistant(student_user);
    }

    for (const auto &pending_post : pending_channel_posts)
    {
        Class *class_ptr = nullptr;
        if (find_class_by_id(&class_ptr, pending_post.class_id))
            class_ptr->restore_channel_post(pending_post.post);
    }

    for (const auto &connection : pending_connections)
    {
        User *first = nullptr;
        User *second = nullptr;
        if (find_user_by_id(&first, connection.first) && find_user_by_id(&second, connection.second))
            first->new_connection(second);
    }

    for (const auto &pending : pending_forms)
    {
        User *professor_user = nullptr;
        Class *class_ptr = nullptr;
        if (find_user_by_id(&professor_user, pending.professor_id) && find_class_by_id(&class_ptr, pending.class_id))
            if (auto professor = dynamic_cast<Professor *>(professor_user))
            {
                FORM form;
                form.id = pending.id;
                form.form_class = class_ptr;
                form.message = pending.message;
                for (const auto &student_id : pending.request_ids)
                {
                    User *student_user = nullptr;
                    if (find_user_by_id(&student_user, student_id))
                        if (auto student = dynamic_cast<Student *>(student_user))
                            form.requests.push_back(student);
                }
                professor->restore_form(form);
            }
    }

    if (was_login)
    {
        User *previous = nullptr;
        if (find_user_by_id(&previous, previous_user_id))
        {
            user = previous;
            is_login = true;
        }
    }
    loading_state = false;
    return true;
}
