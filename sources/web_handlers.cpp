#include "web_handlers.hpp"

#include <cstdio>
#include <ctime>
#include <filesystem>
#include <sstream>

using std::map;

namespace
{
string html_escape(const string &input)
{
    string output;
    for (char ch : input)
    {
        if (ch == '&')
            output += "&amp;";
        else if (ch == '<')
            output += "&lt;";
        else if (ch == '>')
            output += "&gt;";
        else if (ch == '"')
            output += "&quot;";
        else if (ch == '\'')
            output += "&#39;";
        else
            output += ch;
    }
    return output;
}

string attr_escape(const string &input)
{
    return html_escape(input);
}

string join_strings(const vector<string> &items, const string &separator)
{
    string result;
    for (int i = 0; i < (int)items.size(); i++)
    {
        if (i)
            result += separator;
        result += items[i];
    }
    return result;
}

string basename_only(string path)
{
    while (!path.empty() && (path.find('/') != string::npos || path.find('\\') != string::npos))
    {
        size_t pos = path.find_last_of("/\\");
        path = path.substr(pos + 1);
    }
    string clean;
    for (char ch : path)
        if (isalnum((unsigned char)ch) || ch == '.' || ch == '_' || ch == '-')
            clean += ch;
    return clean;
}

const string INVALID_UPLOAD = "__UTMS_INVALID_UPLOAD__";
const string UPLOAD_PREFIX = "web/uploads/";

bool is_png_data(const string &data)
{
    const unsigned char signature[] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};
    if (data.size() < sizeof(signature))
        return false;
    for (size_t i = 0; i < sizeof(signature); i++)
        if ((unsigned char)data[i] != signature[i])
            return false;
    return true;
}

bool is_managed_upload_path(const string &path)
{
    return path.rfind(UPLOAD_PREFIX, 0) == 0 && basename_only(path) == path.substr(UPLOAD_PREFIX.size());
}

void remove_managed_upload(const string &path)
{
    if (is_managed_upload_path(path))
        std::remove(path.c_str());
}

bool has_current_session(Instruction_Handler *ih, Request *req)
{
    return ih != nullptr && req != nullptr && ih->logged_in() && req->getSessionId() == ih->current_user_id();
}

string public_asset_src(const string &stored_path)
{
    if (stored_path.empty())
        return EMPTYSTRING;
    if (stored_path.rfind(UPLOAD_PREFIX, 0) == 0)
        return "/asset?file=" + basename_only(stored_path.substr(UPLOAD_PREFIX.size()));
    return stored_path;
}

string save_uploaded_png(Request *req, const string &field, const string &prefix)
{
    string data = req->getBodyParam(field);
    if (data.empty())
        return EMPTYSTRING;
    if (!is_png_data(data))
        return INVALID_UPLOAD;
    std::filesystem::create_directories(UPLOAD_PREFIX);
    static long long counter = 0;
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    string filename = prefix + "_" + to_string(now) + "_" + to_string(counter++) + ".png";
    string path = UPLOAD_PREFIX + filename;
    if (!utils::writeToFile(data, path))
        return INVALID_UPLOAD;
    return path;
}

ANSWER make_answer(response output)
{
    ANSWER answer;
    answer.output = output;
    return answer;
}

ANSWER run_checked(Instruction_Handler *ih, QUESTION question)
{
    Check_Error checker;
    input_error error = checker.run(&question);
    if (error == command_type_error)
        return make_answer(BadRequest);
    if (error == command_error)
        return make_answer(NotFound);
    if (error == info_error)
    {
        question.info.clear();
        question.info.push_back(INFO_ERROR);
    }
    return ih->run(question);
}

QUESTION question(command_type type, command input, vector<string> info)
{
    QUESTION q;
    q.invalid = true;
    q.type = type;
    q.input = input;
    q.info = info;
    return q;
}

string answer_css_class(response output)
{
    if (output == Ok || output == JustInformation)
        return "notice success";
    if (output == Empty)
        return "notice muted-box";
    return "notice error";
}

string answer_title(response output)
{
    switch (output)
    {
    case Ok:
        return "OK";
    case Empty:
        return "Empty";
    case NotFound:
        return "Not Found";
    case BadRequest:
        return "Bad Request";
    case PermissionDenied:
        return "Permission Denied";
    case JustInformation:
        return "Result";
    }
    return "Result";
}

string result_banner(ANSWER answer)
{
    string text = answer_to_string(answer);
    if (text.empty() && answer.output == JustInformation)
        text = "OK\n";
    string html = "<div class='" + answer_css_class(answer.output) + "'>";
    html += "<strong>" + answer_title(answer.output) + "</strong>";
    if (!text.empty() && text != "OK\n")
        html += "<pre>" + html_escape(text) + "</pre>";
    html += "</div>";
    return html;
}

string sidebar_link(const string &href, const string &label, const string &active, const string &key)
{
    string cls = "nav-link";
    if (active == key)
        cls += " active";
    return "<a class='" + cls + "' href='" + href + "'>" + html_escape(label) + "</a>";
}

string sidebar(Instruction_Handler *ih, const string &active)
{
    string html;
    html += "<aside class='sidebar'>";
    html += "<div class='brand-mini'><img src='/UTMS.png' alt='UTMS'><div><strong>UTMS</strong><span>University Panel</span></div></div>";
    html += "<nav>";
    html += sidebar_link("/home", "Dashboard", active, "home");
    html += sidebar_link("/post/new", "New Post", active, "new-post");
    if (!ih->current_user_is_system_manager())
        html += sidebar_link("/profile_photo", "Profile Photo", active, "profile-photo");
    if (!ih->current_user_is_system_manager())
        html += sidebar_link("/courses", "Course Offerings", active, "courses");
    if (!ih->current_user_is_system_manager())
        html += sidebar_link("/personal_page", "Personal Pages", active, "personal-page");
    if (!ih->current_user_is_system_manager())
        html += sidebar_link("/connect", "Connect", active, "connect");
    if (!ih->current_user_is_system_manager())
        html += sidebar_link("/notifications", "Notifications", active, "notifications");
    if (ih->current_user_is_student())
    {
        html += "<div class='nav-section'>Student</div>";
        html += sidebar_link("/my_courses", "My Courses", active, "my-courses");
        html += sidebar_link("/my_courses/add", "Enroll Course", active, "enroll");
        html += sidebar_link("/my_courses/delete", "Drop Course", active, "drop");
        html += sidebar_link("/course_channel", "Course Channels", active, "course-channel");
        html += sidebar_link("/ta_request", "TA Requests", active, "ta-request");
    }
    if (ih->current_user_is_professor())
    {
        html += "<div class='nav-section'>Professor</div>";
        html += sidebar_link("/course_channel", "Course Channels", active, "course-channel");
        html += sidebar_link("/course_post/new", "New Course Post", active, "new-course-post");
        html += sidebar_link("/ta_form/new", "New TA Form", active, "new-ta-form");
        html += sidebar_link("/ta_form/close", "Close TA Form", active, "close-ta-form");
    }
    if (ih->current_user_is_system_manager())
    {
        html += "<div class='nav-section'>Manager</div>";
        html += sidebar_link("/courses/add", "Offer Course", active, "course-offer");
    }
    html += "</nav><a class='logout' href='/logout'>Logout</a></aside>";
    return html;
}

string base_styles()
{
    return R"CSS(
    :root{
        --primary-color:#004b61;--primary-rgb:0,75,97;--secondary-color:#063341;--accent-color:#00a6d6;
        --text-color:#e8edf0;--muted-color:#9aaeb7;--bg-color:#0d1113;--panel-color:#050708;
        --card-color:#0b1215;--border-color:#1b4754;--danger-color:#b83c3c;--success-color:#0c7a58;
        --shadow:0 18px 45px rgba(0,0,0,.32);--radius:18px;
    }
    body.theme{
        --primary-color:#0085ad;--primary-rgb:0,133,173;--secondary-color:#00617d;--accent-color:#004b61;
        --text-color:#17242a;--muted-color:#516971;--bg-color:#e8eef1;--panel-color:#ffffff;
        --card-color:#f7fafb;--border-color:#b8d0d8;--danger-color:#a83232;--success-color:#0b7456;
        --shadow:0 18px 45px rgba(8,52,67,.16);
    }
    *{box-sizing:border-box}
    body{font-family:Inter,Segoe UI,Arial,sans-serif;margin:0;min-height:100vh;color:var(--text-color);background:radial-gradient(circle at 16% -10%,rgba(var(--primary-rgb),.30),transparent 30rem),linear-gradient(135deg,var(--bg-color),#050607 68%);transition:background-color .3s,color .3s;line-height:1.5}
    body.theme{background:radial-gradient(circle at 18% -10%,rgba(var(--primary-rgb),.18),transparent 28rem),linear-gradient(135deg,var(--bg-color),#f7fbfc 72%)}
    a{color:inherit;text-decoration:none}.app{display:grid;grid-template-columns:280px minmax(0,1fr);min-height:100vh}
    .sidebar{background:linear-gradient(180deg,var(--primary-color),#003748);padding:22px;display:flex;flex-direction:column;gap:18px;position:sticky;top:0;height:100vh;overflow:auto;box-shadow:10px 0 30px rgba(0,0,0,.20)}
    .brand-mini{display:flex;gap:12px;align-items:center;margin-bottom:12px;color:#fff}.brand-mini img{width:58px;height:58px;object-fit:contain;background:transparent;display:block;filter:drop-shadow(0 8px 12px rgba(0,0,0,.22))}.brand-mini strong{font-size:1.62rem;display:block;letter-spacing:.02em}.brand-mini span{font-size:.86rem;opacity:.80}
    .nav-link,.logout{display:flex;align-items:center;gap:10px;padding:12px 14px;margin:6px 0;border-radius:12px;background:transparent;transition:.2s;color:#f4fbfd}.nav-link:hover,.nav-link.active,.logout:hover{background:rgba(0,0,0,.24);transform:translateX(3px)}.logout{margin-top:auto;background:rgba(0,0,0,.16)}.nav-section{margin:18px 0 8px;font-size:.76rem;text-transform:uppercase;letter-spacing:.11em;opacity:.72;color:#d8eef5}
    .content{padding:30px;max-width:1360px;width:100%;margin:0 auto}.topbar{display:flex;justify-content:space-between;align-items:center;gap:16px;margin-bottom:24px}.topbar h1{margin:0;font-size:2.08rem;letter-spacing:-.03em}.topbar-actions{display:flex;align-items:center;gap:12px;flex-wrap:wrap}.user-badge{display:flex;align-items:center;gap:11px;padding:9px 12px;border-radius:999px;background:rgba(var(--primary-rgb),.12);border:1px solid var(--border-color)}.user-badge img,.user-badge .mini-avatar{width:38px;height:38px;border-radius:50%;object-fit:cover;background:var(--secondary-color);border:2px solid rgba(255,255,255,.08)}.user-badge strong{display:block;line-height:1.1}.user-badge span{display:block;color:var(--muted-color);font-size:.82rem}.theme-change{border-radius:999px;padding:.72rem 1rem;border:1px solid var(--border-color);cursor:pointer;color:#fff;background:var(--primary-color)}
    .grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(270px,1fr));gap:18px}.card{background:linear-gradient(180deg,var(--panel-color),var(--card-color));border:1px solid var(--border-color);border-radius:var(--radius);padding:22px;margin-bottom:18px;box-shadow:var(--shadow)}.card h2,.card h3{margin-top:0;letter-spacing:-.02em}.card p{margin-bottom:1rem}.muted{color:var(--muted-color)}.chip{display:inline-block;padding:5px 10px;border-radius:999px;background:rgba(var(--primary-rgb),.88);color:#fff;font-size:.82rem;margin:3px}.avatar{width:104px;height:104px;border-radius:50%;object-fit:cover;background:linear-gradient(135deg,var(--secondary-color),var(--primary-color));border:3px solid var(--border-color);box-shadow:0 10px 24px rgba(0,0,0,.22)}.post-img{max-width:100%;border-radius:14px;margin-top:12px;border:1px solid var(--border-color);box-shadow:0 8px 18px rgba(0,0,0,.18)}
    label{display:block;margin-top:12px;margin-bottom:6px;font-weight:700}input,textarea,select{width:100%;padding:12px 13px;border-radius:12px;border:1px solid var(--border-color);background:var(--card-color);color:var(--text-color);outline:none;transition:border-color .18s,box-shadow .18s}input:focus,textarea:focus,select:focus{border-color:var(--accent-color);box-shadow:0 0 0 3px rgba(var(--primary-rgb),.20)}textarea{min-height:120px;resize:vertical}input[type=file]{padding:10px;background:transparent}
    button,.button{display:inline-block;border:0;border-radius:12px;background:linear-gradient(135deg,var(--primary-color),var(--accent-color));color:#fff;font-weight:800;padding:12px 16px;cursor:pointer;margin-top:14px;box-shadow:0 8px 18px rgba(0,0,0,.18);transition:.18s}button:hover,.button:hover{transform:translateY(-1px);filter:brightness(1.06)}.button.secondary,button.secondary{background:var(--secondary-color)}button.danger,.button.danger{background:var(--danger-color)}.actions{display:flex;gap:10px;flex-wrap:wrap;align-items:center}.notice{border-radius:14px;padding:15px;margin-bottom:18px;border:1px solid var(--border-color);background:var(--panel-color)}.notice pre{white-space:pre-wrap;margin:10px 0 0}.notice.success{border-color:var(--success-color)}.notice.error{border-color:var(--danger-color)}.muted-box{opacity:.85}
    .table-wrap{overflow:auto;border:1px solid var(--border-color);border-radius:14px}.table{width:100%;border-collapse:collapse;background:var(--panel-color)}.table th,.table td{text-align:left;border-bottom:1px solid var(--border-color);padding:12px;vertical-align:top}.table th{font-size:.82rem;color:var(--muted-color);text-transform:uppercase;letter-spacing:.06em}.table tr:last-child td{border-bottom:0}.table tr:hover td{background:rgba(var(--primary-rgb),.06)}.inline-form{display:inline}.empty{padding:30px;text-align:center;border:1px dashed var(--border-color);border-radius:14px;color:var(--muted-color)}.hero{display:grid;grid-template-columns:auto minmax(0,1fr);gap:20px;align-items:center}.hero h2{margin:0}.section-title{margin:26px 0 12px}.login-body{display:flex;align-items:center;justify-content:center;min-height:100vh}.login-container{display:flex;flex-direction:column;justify-content:center;align-items:center;width:100%;padding:24px}.UTMS-logo{font-size:5rem;margin-bottom:1rem;color:var(--text-color);letter-spacing:.03em}.login-box{width:min(430px,92vw);padding:2.15rem;display:flex;border-radius:18px;align-items:center;flex-direction:column;justify-content:center;background:var(--panel-color);border:1px solid var(--border-color);box-shadow:var(--shadow)}.login-box input{margin-top:1rem}.login-box button{width:100%;font-size:1rem}.login-theme{position:fixed;top:1rem;right:1rem;border-radius:999px;padding:.65rem 1rem;border:1px solid var(--border-color);cursor:pointer;color:#fff;background:var(--primary-color)}
    @media(max-width:900px){.app{grid-template-columns:1fr}.sidebar{position:relative;height:auto}.content{padding:18px}.topbar{align-items:flex-start;flex-direction:column}.topbar-actions{width:100%;justify-content:space-between}.UTMS-logo{font-size:3.6rem}.hero{grid-template-columns:1fr}.brand-mini img{width:52px;height:52px}}
    )CSS";
}

string page_shell(Instruction_Handler *ih, const string &title, const string &active, const string &content)
{
    WEB_USER_VIEW user_view;
    ih->web_user_view(ih->current_user_id(), &user_view);
    string photo = public_asset_src(user_view.profile_photo_address);

    string html;
    html += "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>" + html_escape(title) + "</title><style>" + base_styles() + "</style></head><body>";
    html += "<div class='app'>" + sidebar(ih, active) + "<main class='content'>";
    html += "<div class='topbar'><div><h1>" + html_escape(title) + "</h1><div class='muted'>University Teaching Management System</div></div>";
    html += "<div class='topbar-actions'><div class='user-badge'>";
    if (!photo.empty())
        html += "<img src='" + attr_escape(photo) + "' alt='Profile photo'>";
    else
        html += "<div class='mini-avatar'></div>";
    html += "<div><strong>" + html_escape(ih->current_user_name()) + "</strong><span>#" + html_escape(ih->current_user_id()) + " · " + html_escape(ih->current_user_type_label()) + "</span></div></div>";
    html += "<button type='button' class='theme-change'>Change Theme</button></div></div>";
    html += content;
    html += "</main></div><script>const savedTheme=localStorage.getItem('utms-theme');if(savedTheme==='light')document.body.classList.add('theme');document.querySelector('.theme-change')?.addEventListener('click',()=>{document.body.classList.toggle('theme');localStorage.setItem('utms-theme',document.body.classList.contains('theme')?'light':'dark');});</script></body></html>";
    return html;
}

Response *html_response(const string &body)
{
    Response *res = new Response();
    res->setHeader("Content-Type", "text/html; charset=utf-8");
    res->setBody(body);
    return res;
}

Response *render_page(Instruction_Handler *ih, const string &title, const string &active, const string &content)
{
    return html_response(page_shell(ih, title, active, content));
}

Response *render_message_page(Instruction_Handler *ih, const string &title, const string &active, ANSWER answer, const string &back_href, const string &back_label)
{
    string content = result_banner(answer);
    content += "<div class='actions'><a class='button secondary' href='" + back_href + "'>" + html_escape(back_label) + "</a><a class='button' href='/home'>Dashboard</a></div>";
    return render_page(ih, title, active, content);
}

string login_document(const string &error_message = EMPTYSTRING)
{
    string html;
    html += "<!doctype html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>UTMS Login</title><style>" + base_styles() + "</style></head>";
    html += "<body class='login-body'><div class='login-container'><img src='/UTMS.png' alt='Home Logo' style='width:23vh'><div class='UTMS-logo'>UTMS</div>";
    html += "<form class='login-box' action='/' method='post'><h1>LOGIN</h1>";
    if (!error_message.empty())
        html += "<div class='notice error' style='width:100%'><strong>" + html_escape(error_message) + "</strong></div>";
    html += "<input name='username' type='text' placeholder='Username' required><input name='password' type='password' placeholder='Password' required><button type='submit'>Login</button>";
    html += "</form></div><button class='login-theme'>Change Theme</button>";
    html += "<script>const savedTheme=localStorage.getItem('utms-theme');if(savedTheme==='light')document.body.classList.add('theme');document.querySelector('.login-theme').addEventListener('click',()=>{document.body.classList.toggle('theme');localStorage.setItem('utms-theme',document.body.classList.contains('theme')?'light':'dark');});</script></body></html>";
    return html;
}

string class_table(const vector<WEB_CLASS_VIEW> &classes, bool include_channel_link = true)
{
    if (classes.empty())
        return "<div class='empty'>No course offerings are available.</div>";
    string html = "<div class='table-wrap'><table class='table'><thead><tr><th>ID</th><th>Course</th><th>Capacity</th><th>Professor</th><th>Time</th><th>Exam</th><th>Class</th>";
    if (include_channel_link)
        html += "<th>Channel</th>";
    html += "</tr></thead><tbody>";
    for (const auto &c : classes)
    {
        html += "<tr><td>" + html_escape(c.id) + "</td><td>" + html_escape(c.name) + "</td><td>" + html_escape(c.capacity) + "</td><td>" + html_escape(c.professor_name) + "</td><td>" + html_escape(c.time) + "</td><td>" + html_escape(c.exam_date) + "</td><td>" + html_escape(c.class_number) + "</td>";
        if (include_channel_link)
            html += "<td><a class='button secondary' href='/course_channel?id=" + attr_escape(c.id) + "'>Open</a></td>";
        html += "</tr>";
    }
    html += "</tbody></table></div>";
    return html;
}

string course_definitions_table(const vector<WEB_COURSE_DEF_VIEW> &courses)
{
    string html = "<div class='table-wrap'><table class='table'><thead><tr><th>ID</th><th>Name</th><th>Credit</th><th>Prerequisite Semester</th><th>Majors</th></tr></thead><tbody>";
    for (const auto &c : courses)
        html += "<tr><td>" + html_escape(c.id) + "</td><td>" + html_escape(c.name) + "</td><td>" + int_to_str(c.credit) + "</td><td>" + int_to_str(c.prerequisite) + "</td><td>" + html_escape(join_strings(c.major_ids, ", ")) + "</td></tr>";
    html += "</tbody></table></div>";
    return html;
}

string professors_table(const vector<WEB_USER_MINI_VIEW> &professors)
{
    string html = "<div class='table-wrap'><table class='table'><thead><tr><th>ID</th><th>Name</th></tr></thead><tbody>";
    for (const auto &p : professors)
        html += "<tr><td>" + html_escape(p.id) + "</td><td>" + html_escape(p.name) + "</td></tr>";
    html += "</tbody></table></div>";
    return html;
}

string user_cards(const vector<WEB_USER_MINI_VIEW> &users)
{
    string html = "<div class='grid'>";
    for (const auto &u : users)
    {
        html += "<div class='card'><h3>" + html_escape(u.name) + "</h3><p class='muted'>#" + html_escape(u.id) + " · " + html_escape(u.type_label) + "</p>";
        html += "<div class='actions'><a class='button secondary' href='/personal_page?id=" + attr_escape(u.id) + "'>View Page</a></div></div>";
    }
    html += "</div>";
    return html;
}

string role_gate_message(const string &role)
{
    return "<div class='notice error'><strong>Permission Denied</strong><p>This page is available only for " + html_escape(role) + ".</p></div>";
}

bool is_natural_number(const string &input)
{
    if (input.empty() || input == "0")
        return false;
    for (char ch : input)
        if (!isdigit((unsigned char)ch))
            return false;
    return true;
}

string render_home_content(Instruction_Handler *ih)
{
    WEB_USER_VIEW user_view;
    ih->web_user_view(ih->current_user_id(), &user_view);
    string img = public_asset_src(user_view.profile_photo_address);
    string content;
    content += "<div class='card hero'>";
    if (!img.empty())
        content += "<img class='avatar' src='" + attr_escape(img) + "' alt='Profile photo'>";
    else
        content += "<div class='avatar'></div>";
    content += "<div><h2>" + html_escape(user_view.name) + "</h2><p class='muted'>#" + html_escape(user_view.id) + " · " + html_escape(user_view.type_label) + "</p>";
    if (!user_view.major.empty())
        content += "<span class='chip'>" + html_escape(user_view.major) + "</span>";
    if (!user_view.semester.empty())
        content += "<span class='chip'>Semester " + html_escape(user_view.semester) + "</span>";
    if (!user_view.position.empty())
        content += "<span class='chip'>" + html_escape(user_view.position) + "</span>";
    if (!user_view.course_names.empty())
        content += "<p>Courses: " + html_escape(join_strings(user_view.course_names, ", ")) + "</p>";
    content += "</div></div>";

    content += "<div class='grid'>";
    content += "<div class='card'><h3>Create Social Post</h3><p>Share a new post with title, message and an optional PNG image.</p><a class='button' href='/post/new'>New Post</a></div>";
    if (ih->current_user_is_student())
    {
        content += "<div class='card'><h3>Student Registration</h3><p>Enroll in offered courses, drop courses and view your registered courses.</p><a class='button' href='/my_courses'>My Courses</a></div>";
        content += "<div class='card'><h3>Teaching Assistant</h3><p>Browse open TA forms and submit a request.</p><a class='button' href='/ta_request'>TA Requests</a></div>";
    }
    if (ih->current_user_is_professor())
    {
        content += "<div class='card'><h3>Course Channels</h3><p>Open course channels, post announcements and manage TA forms.</p><a class='button' href='/course_channel'>Course Channels</a></div>";
        content += "<div class='card'><h3>TA Forms</h3><p>Create and close teaching assistant recruitment forms.</p><a class='button' href='/ta_form/new'>New TA Form</a></div>";
    }
    if (ih->current_user_is_system_manager())
    {
        content += "<div class='card'><h3>Course Offering</h3><p>Create a new course offering for a professor.</p><a class='button' href='/courses/add'>Offer Course</a></div>";
    }
    if (!ih->current_user_is_system_manager())
        content += "<div class='card'><h3>Social Network</h3><p>View personal pages, connect to users and read notifications.</p><a class='button' href='/personal_page'>Browse Users</a></div>";
    content += "</div>";
    return content;
}

string render_post_form(const string &action, const string &course_id = EMPTYSTRING)
{
    string html = "<div class='card'><form method='post' action='" + action + "' enctype='multipart/form-data'>";
    if (!course_id.empty())
        html += "<input type='hidden' name='id' value='" + attr_escape(course_id) + "'>";
    else if (action == "/course_post/new")
        html += "<label>Course Offering ID</label><input name='id' required placeholder='7'>";
    html += "<label>Title</label><input name='title' required placeholder='Announcement title'>";
    html += "<label>Message</label><textarea name='message' required placeholder='Write your message'></textarea>";
    html += "<label>PNG Image (optional)</label><input type='file' name='image' accept='image/png'>";
    html += "<button type='submit'>Publish</button></form></div>";
    return html;
}

string render_personal_page_content(Instruction_Handler *ih, Request *req)
{
    string target_id = req->getQueryParam("id");
    string content;
    content += "<div class='card'><form method='get' action='/personal_page'><label>User ID</label><input name='id' placeholder='810102612 or 0' value='" + attr_escape(target_id) + "'><button type='submit'>Search</button></form></div>";
    if (target_id.empty())
    {
        content += "<h2>Users</h2>" + user_cards(ih->web_all_users());
        return content;
    }

    ANSWER permission = run_checked(ih, question(Get_Type, Personal_Page_Input, {ID, target_id}));
    if (permission.output != JustInformation)
    {
        content += result_banner(permission);
        return content;
    }

    WEB_USER_VIEW view;
    if (!ih->web_user_view(target_id, &view))
    {
        content += result_banner(make_answer(NotFound));
        return content;
    }
    string img = public_asset_src(view.profile_photo_address);
    content += "<div class='card hero'>";
    if (!img.empty())
        content += "<img class='avatar' src='" + attr_escape(img) + "' alt='Profile photo'>";
    else
        content += "<div class='avatar'></div>";
    content += "<div><h2>" + html_escape(view.name) + "</h2><p class='muted'>#" + html_escape(view.id) + " · " + html_escape(view.type_label) + "</p>";
    if (!view.major.empty())
        content += "<span class='chip'>" + html_escape(view.major) + "</span>";
    if (!view.semester.empty())
        content += "<span class='chip'>Semester " + html_escape(view.semester) + "</span>";
    if (!view.position.empty())
        content += "<span class='chip'>" + html_escape(view.position) + "</span>";
    if (!view.course_names.empty())
        content += "<p>Courses: " + html_escape(join_strings(view.course_names, ", ")) + "</p>";
    content += "</div></div>";

    content += "<h2>Posts</h2>";
    if (view.posts.empty())
    {
        content += "<div class='empty'>No posts yet.</div>";
        return content;
    }
    content += "<div class='grid'>";
    for (int i = (int)view.posts.size() - 1; i >= 0; i--)
    {
        const POST &post = view.posts[i];
        content += "<div class='card'><h3>";
        if (post.is_ta_form)
            content += "TA form for " + html_escape(post.ta_course_name) + " course";
        else
            content += html_escape(post.title);
        content += "</h3><p class='muted'>Post #" + int_to_str(post.id) + "</p>";
        if (!post.image_address.empty())
            content += "<img class='post-img' src='" + attr_escape(public_asset_src(post.image_address)) + "' alt='Post image'>";
        content += "<div class='actions'><a class='button secondary' href='/post?user_id=" + attr_escape(view.id) + "&post_id=" + int_to_str(post.id) + "'>Details</a>";
        if (view.id == ih->current_user_id() && !post.is_ta_form)
        {
            content += "<form class='inline-form' method='post' action='/post/delete'><input type='hidden' name='id' value='" + int_to_str(post.id) + "'><button class='danger' type='submit'>Delete</button></form>";
        }
        content += "</div></div>";
    }
    content += "</div>";
    return content;
}

string render_user_post_content(Instruction_Handler *ih, Request *req)
{
    string user_id = req->getQueryParam("user_id");
    string post_id = req->getQueryParam("post_id");
    ANSWER permission = run_checked(ih, question(Get_Type, Post_Input, {ID, user_id, POST_ID, post_id}));
    if (permission.output != JustInformation)
        return result_banner(permission);
    WEB_USER_VIEW view;
    if (!ih->web_user_view(user_id, &view))
        return result_banner(make_answer(NotFound));
    for (const auto &post : view.posts)
        if (int_to_str(post.id) == post_id)
        {
            string html = "<div class='card'><p class='muted'>" + html_escape(view.name) + " · #" + html_escape(view.id) + "</p>";
            if (post.is_ta_form)
                html += "<h2>TA form for " + html_escape(post.ta_course_name) + " course</h2>";
            else
                html += "<h2>" + html_escape(post.title) + "</h2>";
            html += "<p>" + html_escape(post.message) + "</p>";
            if (!post.image_address.empty())
                html += "<img class='post-img' src='" + attr_escape(public_asset_src(post.image_address)) + "' alt='Post image'>";
            html += "</div><a class='button secondary' href='/personal_page?id=" + attr_escape(user_id) + "'>Back to Personal Page</a>";
            return html;
        }
    return result_banner(make_answer(NotFound));
}

string render_course_channel_content(Instruction_Handler *ih, Request *req)
{
    string target_id = req->getQueryParam("id");
    string content;
    content += "<div class='card'><form method='get' action='/course_channel'><label>Course Offering ID</label><input name='id' placeholder='7' value='" + attr_escape(target_id) + "'><button type='submit'>Open Channel</button></form></div>";
    if (target_id.empty())
    {
        content += "<h2>Course Offerings</h2>" + class_table(ih->web_all_classes());
        return content;
    }

    ANSWER permission = run_checked(ih, question(Get_Type, Course_Channel_Input, {ID, target_id}));
    if (permission.output != JustInformation)
    {
        content += result_banner(permission);
        return content;
    }
    WEB_CLASS_VIEW view;
    if (!ih->web_class_view(target_id, &view))
    {
        content += result_banner(make_answer(NotFound));
        return content;
    }
    content += "<div class='card'><h2>" + html_escape(view.name) + "</h2><p class='muted'>#" + html_escape(view.id) + " · Professor: " + html_escape(view.professor_name) + "</p>";
    content += "<span class='chip'>Capacity " + html_escape(view.capacity) + "</span><span class='chip'>" + html_escape(view.time) + "</span><span class='chip'>Exam " + html_escape(view.exam_date) + "</span><span class='chip'>Class " + html_escape(view.class_number) + "</span></div>";
    if (view.current_user_can_post)
    {
        content += "<h2>Publish in Channel</h2>" + render_post_form("/course_post/new", view.id);
    }
    content += "<h2>Channel Posts</h2>";
    if (view.channel_posts.empty())
        content += "<div class='empty'>No posts in this course channel yet.</div>";
    else
    {
        content += "<div class='grid'>";
        for (int i = (int)view.channel_posts.size() - 1; i >= 0; i--)
        {
            const auto &post = view.channel_posts[i];
            content += "<div class='card'><h3>" + html_escape(post.title) + "</h3><p class='muted'>#" + int_to_str(post.id) + " · " + html_escape(post.author_name) + "</p>";
            if (!post.image_address.empty())
                content += "<img class='post-img' src='" + attr_escape(public_asset_src(post.image_address)) + "' alt='Post image'>";
            content += "<a class='button secondary' href='/course_post?id=" + attr_escape(view.id) + "&post_id=" + int_to_str(post.id) + "'>Details</a></div>";
        }
        content += "</div>";
    }
    return content;
}

string render_course_post_content(Instruction_Handler *ih, Request *req)
{
    string course_id = req->getQueryParam("id");
    string post_id = req->getQueryParam("post_id");
    ANSWER permission = run_checked(ih, question(Get_Type, Course_Post_Input, {ID, course_id, POST_ID, post_id}));
    if (permission.output != JustInformation)
        return result_banner(permission);
    WEB_CLASS_VIEW view;
    if (!ih->web_class_view(course_id, &view))
        return result_banner(make_answer(NotFound));
    for (const auto &post : view.channel_posts)
        if (int_to_str(post.id) == post_id)
        {
            string html = "<div class='card'><h2>" + html_escape(view.name) + "</h2><p class='muted'>#" + html_escape(view.id) + " · " + html_escape(view.professor_name) + " · " + html_escape(view.time) + "</p></div>";
            html += "<div class='card'><p class='muted'>" + html_escape(post.author_name) + " · Post #" + int_to_str(post.id) + "</p><h2>" + html_escape(post.title) + "</h2><p>" + html_escape(post.message) + "</p>";
            if (!post.image_address.empty())
                html += "<img class='post-img' src='" + attr_escape(public_asset_src(post.image_address)) + "' alt='Post image'>";
            html += "</div><a class='button secondary' href='/course_channel?id=" + attr_escape(course_id) + "'>Back to Channel</a>";
            return html;
        }
    return result_banner(make_answer(NotFound));
}

string render_ta_request_content(Instruction_Handler *ih)
{
    string content;
    vector<WEB_TA_FORM_VIEW> forms = ih->web_all_ta_forms();
    if (forms.empty())
    {
        content += "<div class='empty'>There are no open TA forms.</div>";
        return content;
    }
    content += "<div class='grid'>";
    for (const auto &form : forms)
    {
        content += "<div class='card'><h3>" + html_escape(form.course_name) + "</h3><p class='muted'>Form #" + int_to_str(form.id) + " · Professor: " + html_escape(form.professor_name) + " (#" + html_escape(form.professor_id) + ")</p>";
        content += "<p>" + html_escape(form.message) + "</p>";
        content += "<form method='post' action='/ta_request'><input type='hidden' name='professor_id' value='" + attr_escape(form.professor_id) + "'><input type='hidden' name='form_id' value='" + int_to_str(form.id) + "'><button type='submit'>Request TA Position</button></form></div>";
    }
    content += "</div>";
    return content;
}

string render_close_ta_form_content(Instruction_Handler *ih, Request *req)
{
    string target_id = req->getQueryParam("id");
    vector<WEB_TA_FORM_VIEW> forms = ih->web_my_ta_forms();
    if (forms.empty())
        return "<div class='empty'>You have no open TA forms.</div>";
    if (target_id.empty())
    {
        string html = "<div class='grid'>";
        for (const auto &form : forms)
            html += "<div class='card'><h3>" + html_escape(form.course_name) + "</h3><p class='muted'>Form #" + int_to_str(form.id) + " · " + int_to_str((int)form.requests.size()) + " request(s)</p><p>" + html_escape(form.message) + "</p><a class='button' href='/ta_form/close?id=" + int_to_str(form.id) + "'>Review Requests</a></div>";
        html += "</div>";
        return html;
    }
    WEB_TA_FORM_VIEW selected;
    bool found = false;
    for (const auto &form : forms)
        if (int_to_str(form.id) == target_id)
        {
            selected = form;
            found = true;
        }
    if (!found)
        return result_banner(make_answer(NotFound));
    string html = "<div class='card'><h2>" + html_escape(selected.course_name) + "</h2><p class='muted'>Form #" + int_to_str(selected.id) + "</p><p>" + html_escape(selected.message) + "</p></div>";
    html += "<div class='card'><form method='post' action='/ta_form/close'><input type='hidden' name='id' value='" + int_to_str(selected.id) + "'>";
    if (selected.requests.empty())
        html += "<p class='muted'>No requests have been submitted. Closing the form will remove it from your personal page.</p>";
    else
    {
        html += "<div class='table-wrap'><table class='table'><thead><tr><th>Student ID</th><th>Name</th><th>Semester</th><th>Decision</th></tr></thead><tbody>";
        for (const auto &r : selected.requests)
        {
            string key = "decision_" + r.student_id;
            html += "<tr><td>" + html_escape(r.student_id) + "</td><td>" + html_escape(r.student_name) + "</td><td>" + int_to_str(r.semester) + "</td><td>";
            html += "<label style='display:inline;font-weight:400'><input style='width:auto' type='radio' name='" + attr_escape(key) + "' value='accept'> accept</label> ";
            html += "<label style='display:inline;font-weight:400'><input style='width:auto' type='radio' name='" + attr_escape(key) + "' value='reject' checked> reject</label>";
            html += "</td></tr>";
        }
        html += "</tbody></table></div>";
    }
    html += "<button type='submit'>Close Form</button></form></div>";
    return html;
}

} // namespace

Response *LoginPageHandler::callback(Request *)
{
    return html_response(login_document());
}

LoginHandler::LoginHandler(Instruction_Handler *ih) : ih_(ih) {}
Response *LoginHandler::callback(Request *req)
{
    string username = req->getBodyParam("username");
    string password = req->getBodyParam("password");
    ANSWER answer = run_checked(ih_, question(Post_Type, Login_Input, {ID, username, PASSWORD, password}));
    if (answer.output == Ok)
    {
        Response *res = Response::redirect("/home");
        res->setSessionId(username);
        return res;
    }
    return html_response(login_document(answer_to_string(answer)));
}

LogoutHandler::LogoutHandler(Instruction_Handler *ih) : ih_(ih) {}
Response *LogoutHandler::callback(Request *req)
{
    if (!has_current_session(ih_, req))
        return Response::redirect("/");
    run_checked(ih_, question(Post_Type, Logout_Input, {EMPTYSTRING}));
    Response *res = Response::redirect("/");
    res->setSessionId(EMPTYSTRING);
    return res;
}

WebPageHandler::WebPageHandler(Instruction_Handler *ih, WebPage page) : ih_(ih), page_(page) {}
Response *WebPageHandler::callback(Request *req)
{
    if (!has_current_session(ih_, req))
        return Response::redirect("/");

    switch (page_)
    {
    case WebPage::Home:
        return render_page(ih_, "Dashboard", "home", render_home_content(ih_));
    case WebPage::NewPost:
        return render_page(ih_, "New Post", "new-post", render_post_form("/post/new"));
    case WebPage::ProfilePhoto:
    {
        if (ih_->current_user_is_system_manager())
            return render_page(ih_, "Profile Photo", "profile-photo", role_gate_message("students and professors"));
        WEB_USER_VIEW view;
        ih_->web_user_view(ih_->current_user_id(), &view);
        string content;
        string img = public_asset_src(view.profile_photo_address);
        content += "<div class='card'>";
        if (!img.empty())
            content += "<img class='avatar' src='" + attr_escape(img) + "' alt='Profile photo'><p class='muted'>Current profile photo</p>";
        else
            content += "<div class='empty'>No profile photo has been set.</div>";
        content += "</div><div class='card'><form method='post' action='/profile_photo' enctype='multipart/form-data'><label>New PNG Profile Photo</label><input type='file' name='photo' accept='image/png' required><button type='submit'>Save Photo</button></form>";
        content += "<form method='post' action='/profile_photo/delete'><button class='danger' type='submit'>Delete Profile Photo</button></form></div>";
        return render_page(ih_, "Profile Photo", "profile-photo", content);
    }
    case WebPage::Courses:
    {
        if (ih_->current_user_is_system_manager())
            return render_page(ih_, "Course Offerings", "courses", role_gate_message("students and professors"));
        return render_page(ih_, "Course Offerings", "courses", class_table(ih_->web_all_classes()));
    }
    case WebPage::PersonalPage:
    {
        if (ih_->current_user_is_system_manager())
            return render_page(ih_, "Personal Pages", "personal-page", role_gate_message("students and professors"));
        return render_page(ih_, "Personal Pages", "personal-page", render_personal_page_content(ih_, req));
    }
    case WebPage::UserPost:
    {
        if (ih_->current_user_is_system_manager())
            return render_page(ih_, "Post Details", "personal-page", role_gate_message("students and professors"));
        return render_page(ih_, "Post Details", "personal-page", render_user_post_content(ih_, req));
    }
    case WebPage::Connect:
    {
        if (ih_->current_user_is_system_manager())
            return render_page(ih_, "Connect", "connect", role_gate_message("students and professors"));
        string content = "<div class='card'><form method='post' action='/connect'><label>User ID</label><input name='id' required placeholder='810102612'><button type='submit'>Connect</button></form></div><h2>Users</h2>" + user_cards(ih_->web_all_users());
        return render_page(ih_, "Connect", "connect", content);
    }
    case WebPage::Notifications:
    {
        if (ih_->current_user_is_system_manager())
            return render_page(ih_, "Notifications", "notifications", role_gate_message("students and professors"));
        ANSWER answer = run_checked(ih_, question(Get_Type, Notification_Input, {EMPTYSTRING}));
        return render_page(ih_, "Notifications", "notifications", result_banner(answer));
    }
    case WebPage::MyCourses:
    {
        if (!ih_->current_user_is_student())
            return render_page(ih_, "My Courses", "my-courses", role_gate_message("students"));
        return render_page(ih_, "My Courses", "my-courses", class_table(ih_->web_my_student_classes()));
    }
    case WebPage::EnrollCourse:
    {
        if (!ih_->current_user_is_student())
            return render_page(ih_, "Enroll Course", "enroll", role_gate_message("students"));
        string content = "<div class='card'><form method='post' action='/my_courses/add'><label>Course Offering ID</label><input name='id' required placeholder='7'><button type='submit'>Enroll</button></form></div><h2>Available Offerings</h2>" + class_table(ih_->web_all_classes(), false);
        return render_page(ih_, "Enroll Course", "enroll", content);
    }
    case WebPage::DropCourse:
    {
        if (!ih_->current_user_is_student())
            return render_page(ih_, "Drop Course", "drop", role_gate_message("students"));
        string content = "<div class='card'><form method='post' action='/my_courses/delete'><label>Course Offering ID</label><input name='id' required placeholder='7'><button class='danger' type='submit'>Drop</button></form></div><h2>Your Courses</h2>" + class_table(ih_->web_my_student_classes(), false);
        return render_page(ih_, "Drop Course", "drop", content);
    }
    case WebPage::CourseOffer:
    {
        if (!ih_->current_user_is_system_manager())
            return render_page(ih_, "Offer Course", "course-offer", role_gate_message("system manager"));
        string content = "<div class='card'><form method='post' action='/courses/add'><label>Course ID</label><input name='course_id' required><label>Professor ID</label><input name='professor_id' required><label>Capacity</label><input name='capacity' required><label>Time</label><input name='time' required placeholder='Sunday:13-15'><label>Exam Date</label><input name='exam_date' required placeholder='1403/4/4'><label>Class Number</label><input name='class_number' required><button type='submit'>Create Offering</button></form></div>";
        content += "<div class='grid'><div class='card'><h2>Course Definitions</h2>" + course_definitions_table(ih_->web_course_definitions()) + "</div><div class='card'><h2>Professors</h2>" + professors_table(ih_->web_professors()) + "</div></div>";
        return render_page(ih_, "Offer Course", "course-offer", content);
    }
    case WebPage::CourseChannel:
    {
        if (ih_->current_user_is_system_manager())
            return render_page(ih_, "Course Channel", "course-channel", role_gate_message("students and professors"));
        return render_page(ih_, "Course Channel", "course-channel", render_course_channel_content(ih_, req));
    }
    case WebPage::CoursePost:
    {
        if (ih_->current_user_is_system_manager())
            return render_page(ih_, "Course Post", "course-channel", role_gate_message("students and professors"));
        return render_page(ih_, "Course Post", "course-channel", render_course_post_content(ih_, req));
    }
    case WebPage::NewCoursePost:
    {
        if (!ih_->current_user_is_professor() && !ih_->current_user_is_student())
            return render_page(ih_, "New Course Post", "new-course-post", role_gate_message("professors or teaching assistants"));
        return render_page(ih_, "New Course Post", "new-course-post", render_post_form("/course_post/new"));
    }
    case WebPage::NewTaForm:
    {
        if (!ih_->current_user_is_professor())
            return render_page(ih_, "New TA Form", "new-ta-form", role_gate_message("professors"));
        string content = "<div class='card'><form method='post' action='/ta_form/new'><label>Course Offering ID</label><input name='course_id' required><label>Message</label><textarea name='message' required></textarea><button type='submit'>Publish TA Form</button></form></div><h2>Your Course Offerings</h2>";
        vector<WEB_CLASS_VIEW> owned;
        for (auto c : ih_->web_all_classes())
            if (c.professor_id == ih_->current_user_id())
                owned.push_back(c);
        content += class_table(owned, false);
        return render_page(ih_, "New TA Form", "new-ta-form", content);
    }
    case WebPage::TaRequest:
    {
        if (!ih_->current_user_is_student())
            return render_page(ih_, "TA Requests", "ta-request", role_gate_message("students"));
        return render_page(ih_, "TA Requests", "ta-request", render_ta_request_content(ih_));
    }
    case WebPage::CloseTaForm:
    {
        if (!ih_->current_user_is_professor())
            return render_page(ih_, "Close TA Form", "close-ta-form", role_gate_message("professors"));
        return render_page(ih_, "Close TA Form", "close-ta-form", render_close_ta_form_content(ih_, req));
    }
    }
    return render_page(ih_, "Not Found", "home", result_banner(make_answer(NotFound)));
}

WebActionHandler::WebActionHandler(Instruction_Handler *ih, WebAction action) : ih_(ih), action_(action) {}
Response *WebActionHandler::callback(Request *req)
{
    if (!has_current_session(ih_, req))
        return Response::redirect("/");

    switch (action_)
    {
    case WebAction::NewPost:
    {
        string image = save_uploaded_png(req, "image", "post");
        if (image == INVALID_UPLOAD)
            return render_message_page(ih_, "New Post", "new-post", make_answer(BadRequest), "/post/new", "Create Another Post");
        ANSWER answer = run_checked(ih_, question(Post_Type, Post_Input, {TITLE, req->getBodyParam("title"), MESSAGE, req->getBodyParam("message"), IMAGE, image}));
        if (answer.output != Ok)
            remove_managed_upload(image);
        return render_message_page(ih_, "New Post", "new-post", answer, "/post/new", "Create Another Post");
    }
    case WebAction::DeletePost:
    {
        string image_to_delete;
        WEB_USER_VIEW view;
        if (ih_->web_user_view(ih_->current_user_id(), &view))
            for (const auto &post : view.posts)
                if (int_to_str(post.id) == req->getBodyParam("id") && !post.is_ta_form)
                    image_to_delete = post.image_address;
        ANSWER answer = run_checked(ih_, question(Delete_Type, Post_Input, {ID, req->getBodyParam("id")}));
        if (answer.output == Ok)
            remove_managed_upload(image_to_delete);
        return render_message_page(ih_, "Delete Post", "personal-page", answer, "/personal_page?id=" + ih_->current_user_id(), "Back to My Page");
    }
    case WebAction::SaveProfilePhoto:
    {
        WEB_USER_VIEW view;
        ih_->web_user_view(ih_->current_user_id(), &view);
        string old_photo = view.profile_photo_address;
        string photo = save_uploaded_png(req, "photo", "profile");
        ANSWER answer = (photo.empty() || photo == INVALID_UPLOAD) ? make_answer(BadRequest) : run_checked(ih_, question(Post_Type, Profile_Photo_Input, {PHOTO, photo}));
        if (answer.output == Ok)
            remove_managed_upload(old_photo);
        else
            remove_managed_upload(photo);
        return render_message_page(ih_, "Profile Photo", "profile-photo", answer, "/profile_photo", "Back to Profile Photo");
    }
    case WebAction::DeleteProfilePhoto:
    {
        WEB_USER_VIEW view;
        ih_->web_user_view(ih_->current_user_id(), &view);
        remove_managed_upload(view.profile_photo_address);
        ANSWER answer = run_checked(ih_, question(Post_Type, Profile_Photo_Input, {PHOTO, EMPTYSTRING}));
        return render_message_page(ih_, "Profile Photo", "profile-photo", answer, "/profile_photo", "Back to Profile Photo");
    }
    case WebAction::Connect:
    {
        ANSWER answer = run_checked(ih_, question(Post_Type, Connect_Input, {ID, req->getBodyParam("id")}));
        return render_message_page(ih_, "Connect", "connect", answer, "/connect", "Back to Connect");
    }
    case WebAction::EnrollCourse:
    {
        ANSWER answer = run_checked(ih_, question(Put_Type, My_Courses_Input, {ID, req->getBodyParam("id")}));
        return render_message_page(ih_, "Enroll Course", "enroll", answer, "/my_courses/add", "Back to Enroll");
    }
    case WebAction::DropCourse:
    {
        ANSWER answer = run_checked(ih_, question(Delete_Type, My_Courses_Input, {ID, req->getBodyParam("id")}));
        return render_message_page(ih_, "Drop Course", "drop", answer, "/my_courses/delete", "Back to Drop");
    }
    case WebAction::CourseOffer:
    {
        ANSWER answer = run_checked(ih_, question(Post_Type, Course_Offer_Input, {COURSE_ID, req->getBodyParam("course_id"), PROFESSOR_ID, req->getBodyParam("professor_id"), CAPACITY, req->getBodyParam("capacity"), CLASS_TIME, req->getBodyParam("time"), EXAM_DATE, req->getBodyParam("exam_date"), CLASS_NUMBER, req->getBodyParam("class_number")}));
        return render_message_page(ih_, "Offer Course", "course-offer", answer, "/courses/add", "Back to Course Offering");
    }
    case WebAction::NewCoursePost:
    {
        string image = save_uploaded_png(req, "image", "course_post");
        string back = req->getBodyParam("id").empty() ? "/course_post/new" : "/course_channel?id=" + req->getBodyParam("id");
        if (image == INVALID_UPLOAD)
            return render_message_page(ih_, "New Course Post", "new-course-post", make_answer(BadRequest), back, "Back to Channel");
        ANSWER answer = run_checked(ih_, question(Post_Type, Course_Post_Input, {ID, req->getBodyParam("id"), TITLE, req->getBodyParam("title"), MESSAGE, req->getBodyParam("message"), IMAGE, image}));
        if (answer.output != Ok)
            remove_managed_upload(image);
        return render_message_page(ih_, "New Course Post", "new-course-post", answer, back, "Back to Channel");
    }
    case WebAction::NewTaForm:
    {
        ANSWER answer = run_checked(ih_, question(Post_Type, Ta_Form_Input, {COURSE_ID, req->getBodyParam("course_id"), MESSAGE, req->getBodyParam("message")}));
        return render_message_page(ih_, "New TA Form", "new-ta-form", answer, "/ta_form/new", "Back to TA Form");
    }
    case WebAction::TaRequest:
    {
        ANSWER answer = run_checked(ih_, question(Post_Type, Ta_Request_Input, {PROFESSOR_ID, req->getBodyParam("professor_id"), FORM_ID, req->getBodyParam("form_id")}));
        return render_message_page(ih_, "TA Request", "ta-request", answer, "/ta_request", "Back to TA Requests");
    }
    case WebAction::CloseTaForm:
    {
        string form_id = req->getBodyParam("id");
        if (!is_natural_number(form_id))
            return render_message_page(ih_, "Close TA Form", "close-ta-form", make_answer(BadRequest), "/ta_form/close", "Back to Forms");
        map<string, string> decisions;
        for (const auto &form : ih_->web_my_ta_forms())
            if (int_to_str(form.id) == form_id)
                for (const auto &request : form.requests)
                {
                    string value = req->getBodyParam("decision_" + request.student_id);
                    decisions[request.student_id] = value == "accept" ? "accept" : "reject";
                }
        ANSWER answer = ih_->web_close_ta_form(str_to_int(form_id), decisions);
        return render_message_page(ih_, "Close TA Form", "close-ta-form", answer, "/ta_form/close", "Back to Forms");
    }
    }
    return render_message_page(ih_, "Not Found", "home", make_answer(NotFound), "/home", "Dashboard");
}

Response *UploadedAssetHandler::callback(Request *req)
{
    string file = basename_only(req->getQueryParam("file"));
    if (file.empty())
        return new Response(Response::Status::notFound);
    string path = "web/uploads/" + file;
    string data = utils::readFile(path);
    if (data.empty())
        return new Response(Response::Status::notFound);
    Response *res = new Response();
    res->setHeader("Content-Type", "image/png");
    res->setBody(data);
    return res;
}
