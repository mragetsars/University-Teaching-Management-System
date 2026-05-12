#include "web_handlers.hpp"

#include <cstdlib>
#include <iostream>

static string html_escape(const string &input)
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
        else
            output += ch;
    }
    return output;
}

static string page_shell(const string &title, const string &content)
{
    string body;
    body += "<!doctype html><html lang='en'><head><meta charset='utf-8'>";
    body += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    body += "<title>" + html_escape(title) + "</title>";
    body += "<style>body{margin:0;font-family:Arial,sans-serif;background:#0f172a;color:#e2e8f0;}";
    body += "a{color:#fbbf24;text-decoration:none}.wrap{max-width:980px;margin:0 auto;padding:32px}.card{background:#111827;border:1px solid #334155;border-radius:18px;padding:24px;margin:16px 0;box-shadow:0 18px 50px #0005}.brand{display:flex;gap:16px;align-items:center}.brand img{width:72px;height:72px;object-fit:contain}.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(220px,1fr));gap:16px}input,textarea{width:100%;box-sizing:border-box;border-radius:12px;border:1px solid #475569;background:#020617;color:#e2e8f0;padding:12px;margin:8px 0}button{border:0;border-radius:12px;background:#f59e0b;color:#111827;font-weight:700;padding:12px 18px;cursor:pointer}pre{white-space:pre-wrap;background:#020617;border-radius:12px;padding:16px;border:1px solid #334155}.nav{display:flex;gap:12px;flex-wrap:wrap;margin:16px 0}.muted{color:#94a3b8}</style>";
    body += "</head><body><div class='wrap'>" + content + "</div></body></html>";
    return body;
}

static Response *html_response(const string &title, const string &content)
{
    Response *res = new Response();
    res->setHeader("Content-Type", "text/html");
    res->setBody(page_shell(title, content));
    return res;
}

Response *LoginPageHandler::callback(Request *)
{
    string content;
    content += "<div class='brand'><img src='/UTMS.png' alt='UTMS'><div><h1>UTMS</h1><p class='muted'>University Teaching Management System</p></div></div>";
    content += "<div class='card'><h2>Login</h2><form method='post' action='/'>";
    content += "<label>ID</label><input name='username' placeholder='810102612 or 0'>";
    content += "<label>Password</label><input name='password' type='password' placeholder='password'>";
    content += "<button type='submit'>Enter UTMS</button></form>";
    content += "<p class='muted'>Default system account: <code>0 / UT_account</code></p></div>";
    return html_response("UTMS Login", content);
}

HomeHandler::HomeHandler(Instruction_Handler *ih) : ih_(ih) {}
Response *HomeHandler::callback(Request *)
{
    if (!ih_->logged_in())
        return Response::redirect("/");
    string content;
    content += "<div class='brand'><img src='/UTMS.png' alt='UTMS'><div><h1>UTMS Dashboard</h1><p class='muted'>Signed in as " + html_escape(ih_->current_user_name()) + " (#" + html_escape(ih_->current_user_id()) + ")</p></div></div>";
    content += "<div class='nav'><a href='/console'>Command Console</a><a href='/up'>Upload</a><a href='/music'>Music</a><a href='/colors'>Colors</a><a href='/logout'>Logout</a></div>";
    content += "<div class='grid'><div class='card'><h3>CLI-compatible console</h3><p>Run any GET/POST/PUT/DELETE command from phases 1 and 2 through the web UI.</p><a href='/console'>Open console →</a></div>";
    content += "<div class='card'><h3>Examples</h3><pre>GET courses ?\nPOST post ? title \"Hello\" message \"Welcome to UTMS\"\nGET notification ?</pre></div></div>";
    return html_response("UTMS Home", content);
}

Response *ConsolePageHandler::callback(Request *)
{
    string content;
    content += "<h1>UTMS Command Console</h1><div class='nav'><a href='/home'>Home</a><a href='/logout'>Logout</a></div>";
    content += "<div class='card'><form method='post' action='/console'>";
    content += "<label>Command</label><textarea name='command' rows='4' placeholder='GET courses ?'></textarea>";
    content += "<button type='submit'>Run command</button></form></div>";
    return html_response("UTMS Console", content);
}

ConsoleCommandHandler::ConsoleCommandHandler(Instruction_Handler *ih) : ih_(ih) {}
Response *ConsoleCommandHandler::callback(Request *req)
{
    string command = req->getBodyParam("command");
    TERMINAL_PARSE_RESULT parsed = parse_terminal_command(command);
    ANSWER answer;
    if (!parsed.should_run)
        answer.output = parsed.parse_error;
    else
        answer = ih_->run(parsed.question);

    string content;
    content += "<h1>UTMS Command Console</h1><div class='nav'><a href='/home'>Home</a><a href='/console'>Run another command</a></div>";
    content += "<div class='card'><h3>Command</h3><pre>" + html_escape(command) + "</pre><h3>Output</h3><pre>" + html_escape(answer_to_string(answer)) + "</pre></div>";
    return html_response("UTMS Output", content);
}

Response *RandomNumberHandler::callback(Request *)
{
    return html_response("UTMS Random", "<div class='card'><h1>Random</h1><p>" + to_string(rand() % 10 + 1) + "</p></div>");
}

LoginHandler::LoginHandler(Instruction_Handler *ih) : ih_(ih) {}
Response *LoginHandler::callback(Request *req)
{
    string username = req->getBodyParam("username");
    string password = req->getBodyParam("password");
    QUESTION q;
    q.invalid = true;
    q.type = Post_Type;
    q.input = Login_Input;
    q.info = {ID, username, PASSWORD, password};
    ANSWER answer = ih_->run(q);
    if (answer.output == Ok)
    {
        Response *res = Response::redirect("/home");
        res->setSessionId(username);
        return res;
    }
    return html_response("UTMS Login Failed", "<div class='card'><h1>Login failed</h1><pre>" + html_escape(answer_to_string(answer)) + "</pre><a href='/'>Back to login</a></div>");
}

LogoutHandler::LogoutHandler(Instruction_Handler *ih) : ih_(ih) {}
Response *LogoutHandler::callback(Request *)
{
    QUESTION q;
    q.invalid = true;
    q.type = Post_Type;
    q.input = Logout_Input;
    q.info = {EMPTYSTRING};
    ih_->run(q);
    return Response::redirect("/");
}

UploadHandler::UploadHandler(Instruction_Handler *ih) : ih_(ih) {}
Response *UploadHandler::callback(Request *req)
{
    string name = ih_->current_user_id().empty() ? "upload.bin" : ih_->current_user_id();
    string file = req->getBodyParam("file");
    utils::writeToFile(file, name);
    return Response::redirect("/home");
}

ColorHandler::ColorHandler(const string &filePath) : TemplateHandler(filePath) {}

map<string, string> ColorHandler::handle(Request *req)
{
    map<string, string> context;
    context["name"] = "I am " + req->getQueryParam("name");
    context["color"] = req->getQueryParam("color");
    return context;
}
