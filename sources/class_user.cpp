#include "class_user.hpp"

User::User() {}
User::User(USER input_info)
{
    id = input_info.id;
    password = input_info.password;
    name = input_info.name;
}

response User::login(string input_id, string input_password)
{
    if (id != input_id)
        return NotFound;
    if (password != input_password)
        return PermissionDenied;
    return Ok;
}

response User::new_post(string input_title, string input_message, string input_image_address)
{
    POST the_post;
    the_post.id = posts.empty() ? 1 : posts.back().id + 1;
    the_post.title = input_title;
    the_post.message = input_message;
    the_post.image_address = input_image_address;
    posts.push_back(the_post);
    send_notifications(NEW_POST_notif);
    return Ok;
}

response User::delete_post(int input_id)
{
    for (int i = 0; i < (int)posts.size(); i++)
        if (posts[i].id == input_id && !posts[i].is_ta_form)
        {
            posts.erase(posts.begin() + i);
            return Ok;
        }
    return NotFound;
}

response User::show_post(vector<string> *output, int input_id)
{
    for (int i = 0; i < (int)posts.size(); i++)
        if (posts[i].id == input_id && !posts[i].is_ta_form)
        {
            output->push_back(int_to_str(posts[i].id));
            output->push_back(SPACE);
            output->push_back(quote_text(posts[i].title));
            output->push_back(SPACE);
            output->push_back(quote_text(posts[i].message));
            output->push_back(ENTER);
            return JustInformation;
        }
    return NotFound;
}

response User::show_page(vector<string> *output)
{
    for (int i = (int)posts.size() - 1; i >= 0; i--)
    {
        if (posts[i].is_ta_form)
        {
            output->push_back(int_to_str(posts[i].id));
            output->push_back(SPACE);
            output->push_back(TA_FORM_FOR + posts[i].ta_course_name + SPACE + COURSE_);
            output->push_back(ENTER);
        }
        else
        {
            output->push_back(int_to_str(posts[i].id));
            output->push_back(SPACE);
            output->push_back(quote_text(posts[i].title));
            output->push_back(ENTER);
        }
    }
    return JustInformation;
}

bool User::has_connection(User *connection) const
{
    for (auto item : connections)
        if (item->id == connection->id)
            return true;
    return false;
}

void User::add_connection_one_way(User *connection)
{
    if (connection == nullptr || connection->id == id || has_connection(connection))
        return;
    connections.push_back(connection);
}

response User::new_connection(User *connection)
{
    if (connection == nullptr || connection->id == id || has_connection(connection))
        return BadRequest;
    add_connection_one_way(connection);
    connection->add_connection_one_way(this);
    return Ok;
}

response User::show_notifications(vector<string> *output)
{
    if (notifications.empty())
        return Empty;
    for (int i = (int)notifications.size() - 1; i >= 0; i--)
    {
        output->push_back(notifications[i].id);
        output->push_back(SPACE);
        output->push_back(notifications[i].name);
        output->push_back(string(1, COLON));
        output->push_back(SPACE);
        output->push_back(notifications[i].subject);
        output->push_back(ENTER);
    }
    notifications.clear();
    return JustInformation;
}

void User::send_notifications(string input_subject)
{
    for (auto connection : connections)
        connection->receive_notification(id, name, input_subject);
}

void User::receive_notification(string input_id, string input_name, string input_subject)
{
    notifications.push_back({input_id, input_name, input_subject});
}

response User::set_profile_photo(string input_photo_address)
{
    profile_photo_address = input_photo_address;
    return Ok;
}

string User::profile_photo() const
{
    return profile_photo_address;
}
