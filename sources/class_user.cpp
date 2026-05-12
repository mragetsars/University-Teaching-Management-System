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
    if (id == input_id)
        if (password == input_password)
            return Ok;
        else
            return PermissionDenied;
    else
        return NotFound;
}

response User::new_post(string input_title, string input_message, string input_image_address)
{
    POST the_post;
    if (posts.size() != 0)
        the_post.id = posts[posts.size() - 1].id + 1;
    else
        the_post.id = 1;
    the_post.title = input_title;
    the_post.message = input_message;
    the_post.image_address = input_image_address;
    posts.push_back(the_post);
    send_notifications(NEW_POST_notif);
    return Ok;
}

response User::delete_post(int input_id)
{
    int n = -1;
    for (int i = 0; i < posts.size(); i++)
        if (posts[i].id == input_id)
            n = i;
    if (n == -1)
        return NotFound;
    else
        posts.erase(posts.begin() + n);
    return Ok;
}

response User::show_post(vector<string> *output, int input_id)
{

    if (posts.size() == 0)
        return Empty;
    int n = -1;
    for (int i = 0; i < posts.size(); i++)
        if (posts[i].id == input_id)
            n = i;
    if (n == -1)
        return NotFound;
    else
    {
        output->push_back(int_to_str(posts[n].id));
        output->push_back(SPACE);
        output->push_back(posts[n].title);
        output->push_back(SPACE);
        output->push_back(posts[n].message);
        output->push_back(ENTER);
        return JustInformation;
    }
}

response User::show_page(vector<string> *output)
{
    for (int i = 0; i < posts.size(); i++)
    {
        output->push_back(int_to_str(posts[i].id));
        output->push_back(SPACE);
        output->push_back(posts[i].title);
        output->push_back(SPACE);
        output->push_back(posts[i].message);
        output->push_back(ENTER);
    }
    return JustInformation;
}

response User::new_connection(User *connection)
{
    for (int i = 0; i < connections.size(); i++)
        if (connections[i]->id == connection->id)
            return BadRequest;
    connections.push_back(connection);
    return Ok;
}

response User::show_notifications(vector<string> *output)
{
    if (notifications.size() == 0)
        return Empty;
    for (int i = notifications.size() - 1; i >= 0; i--)
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
    for (int i = 0; i < connections.size(); i++)
        connections[i]->receive_notification(id, name, input_subject);
}
void User::receive_notification(string input_id, string input_name, string input_subject)
{
    NOTIFICATION new_notification;
    new_notification.id = input_id;
    new_notification.name = input_name;
    new_notification.subject = input_subject;
    notifications.push_back(new_notification);
}

response User::set_profile_photo(string input_photo_address)
{
    profile_photo_address = input_photo_address;
    return Ok;
}