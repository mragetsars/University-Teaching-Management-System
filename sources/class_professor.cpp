#include "class_professor.hpp"

Professor::Professor(PROFESSOR input_info)
    : User(input_info.info)
{
    major_id = input_info.info.major_id;
    position = input_info.position;
    position_name = input_info.position_name.empty() ? pos_to_str(input_info.position) : input_info.position_name;
}
Professor::Professor() {}

void Professor::offer_class(Class *input_class)
{
    if (input_class != nullptr && !owns_class(input_class))
        classes.push_back(input_class);
}

bool Professor::owns_class(Class *input_class) const
{
    if (input_class == nullptr)
        return false;
    for (auto current_class : classes)
        if (current_class->id == input_class->id)
            return true;
    return false;
}

response Professor::show_info(vector<string> *output, vector<MAJOR> *majors)
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
    output->push_back(position_name);
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

response Professor::new_form(Class *input_class, string input_message)
{
    if (input_class == nullptr)
        return NotFound;
    if (!owns_class(input_class))
        return PermissionDenied;

    POST the_post;
    the_post.id = posts.empty() ? 1 : posts.back().id + 1;
    the_post.is_ta_form = true;
    the_post.ta_course_id = input_class->id;
    the_post.ta_course_name = input_class->course->name;
    the_post.title = TA_FORM_FOR + input_class->course->name + SPACE + COURSE_;
    the_post.message = input_message;
    posts.push_back(the_post);

    FORM the_form;
    the_form.id = the_post.id;
    the_form.form_class = input_class;
    the_form.message = input_message;
    forms.push_back(the_form);
    send_notifications(NEW_FORM_notif);
    return Ok;
}

response Professor::show_post(vector<string> *output, int input_id)
{
    for (auto form : forms)
        if (form.id == input_id)
        {
            output->push_back(int_to_str(form.id));
            output->push_back(SPACE);
            output->push_back(TA_FORM_FOR + form.form_class->course->name + SPACE + COURSE_);
            output->push_back(ENTER);
            form.form_class->append_full_info(output);
            output->push_back(ENTER);
            output->push_back(quote_text(form.message));
            output->push_back(ENTER);
            return JustInformation;
        }
    return User::show_post(output, input_id);
}

response Professor::new_request(Student *input_user, int input_id)
{
    if (input_user == nullptr)
        return NotFound;
    for (int i = 0; i < (int)forms.size(); i++)
        if (forms[i].id == input_id)
        {
            if (input_user->semester <= forms[i].form_class->course->prerequisite)
                return PermissionDenied;
            for (auto request : forms[i].requests)
                if (request->id == input_user->id)
                    return BadRequest;
            forms[i].requests.push_back(input_user);
            return Ok;
        }
    return NotFound;
}

response Professor::close_form(int input_id)
{
    int form_index = -1;
    for (int i = 0; i < (int)forms.size(); i++)
        if (forms[i].id == input_id)
            form_index = i;
    if (form_index == -1)
        return NotFound;

    FORM form = forms[form_index];
    cout << "We have received " << form.requests.size() << " requests for the teaching assistant position" << endl;
    for (int i = 0; i < (int)form.requests.size();)
    {
        Student *candidate = form.requests[i];
        cout << candidate->id << SPACE << candidate->name << SPACE << candidate->semester << string(1, COLON) << SPACE;
        string answer;
        if (!(cin >> answer))
        {
            answer = "reject";
            cin.clear();
        }
        if (answer == "accept")
        {
            form.form_class->new_teacher_assistant(candidate);
            candidate->receive_notification(form.form_class->id, form.form_class->course->name, string(CLOSE_FORM_notif) + "accepted.");
            i++;
        }
        else if (answer == "reject")
        {
            candidate->receive_notification(form.form_class->id, form.form_class->course->name, string(CLOSE_FORM_notif) + "rejected.");
            i++;
        }
        else
        {
            cout << endl;
        }
    }

    for (int i = 0; i < (int)posts.size(); i++)
        if (posts[i].id == input_id)
        {
            posts.erase(posts.begin() + i);
            break;
        }
    forms.erase(forms.begin() + form_index);
    return JustInformation;
}

response Professor::close_form_with_decisions(int input_id, const map<string, string> &decisions, vector<string> *output)
{
    int form_index = -1;
    for (int i = 0; i < (int)forms.size(); i++)
        if (forms[i].id == input_id)
            form_index = i;
    if (form_index == -1)
        return NotFound;

    FORM form = forms[form_index];
    if (output != nullptr)
    {
        output->push_back("We have received ");
        output->push_back(int_to_str((int)form.requests.size()));
        output->push_back(" requests for the teaching assistant position");
        output->push_back(ENTER);
    }

    for (Student *candidate : form.requests)
    {
        string answer = "reject";
        auto it = decisions.find(candidate->id);
        if (it != decisions.end() && it->second == "accept")
            answer = "accept";

        if (output != nullptr)
        {
            output->push_back(candidate->id);
            output->push_back(SPACE);
            output->push_back(candidate->name);
            output->push_back(SPACE);
            output->push_back(int_to_str(candidate->semester));
            output->push_back(string(1, COLON));
            output->push_back(SPACE);
            output->push_back(answer);
            output->push_back(ENTER);
        }

        if (answer == "accept")
        {
            form.form_class->new_teacher_assistant(candidate);
            candidate->receive_notification(form.form_class->id, form.form_class->course->name, string(CLOSE_FORM_notif) + "accepted.");
        }
        else
        {
            candidate->receive_notification(form.form_class->id, form.form_class->course->name, string(CLOSE_FORM_notif) + "rejected.");
        }
    }

    for (int i = 0; i < (int)posts.size(); i++)
        if (posts[i].id == input_id)
        {
            posts.erase(posts.begin() + i);
            break;
        }
    forms.erase(forms.begin() + form_index);
    return JustInformation;
}

void Professor::restore_form(const FORM &form)
{
    for (const auto &existing : forms)
        if (existing.id == form.id)
            return;
    forms.push_back(form);
}

vector<string> Professor::class_ids() const
{
    vector<string> ids;
    for (auto input_class : classes)
        if (input_class != nullptr)
            ids.push_back(input_class->id);
    return ids;
}
