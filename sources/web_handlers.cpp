#include "web_handlers.hpp"

#include <cstdlib>
#include <iostream>

Response *RandomNumberHandler::callback(Request *req)
{
    Response *res = new Response();
    res->setHeader("Content-Type", "text/html");

    std::string randomNumber = std::to_string(std::rand() % 10 + 1);
    std::string body;

    body += "<html lang=\"en\">";

    body += "<head>";
    body += "<meta charset=\"UTF-8\">";
    body += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    body += "<title>UTMS - Random Number</title>";
    body += "<style>:root{--primary-color:#00455a;--secondary-color:#00455a7a;--text-color:#cccccc;--bg-color:#111111;--form-bg-color:#000000;}body{font-family:'Arial',sans-serif;color:var(--text-color);background-color:var(--bg-color);transition:background-color 0.3s;}.theme{--primary-color:#0096c4;--secondary-color:#0096c480;--text-color:#222222;--bg-color:#dddddd;--form-bg-color:#ffffff;}.UTMS-logo{font-size:5rem;margin-bottom:1rem;color:var(--text-color);transition:color 0.3s;}.container{display:flex;flex-direction:column;justify-content:center;align-items:center;height:100vh;}.box{width:30%;padding:2rem;display:flex;border-radius:10px;align-items:center;flex-direction:column;justify-content:center;box-shadow:0 4px 8px rgba(0,0,0,0.1);background:var(--form-bg-color);transition:background-color 0.3s;}.box img{border-radius:50%;width:150px;height:150px;object-fit:cover;margin-bottom:20px;}.box div{font-size:20rem;margin-bottom:1rem;color:var(--primary-color);transition:color 0.3s;}.box input{width:95%;padding:1rem;font-size:1rem;margin-top:1rem;border-radius:5px;border:none;cursor:pointer;color:var(--text-color);border:1px solid var(--bg-color);background-color:var(--bg-color);transition:background-color 0.3s;}.box button{width:100%;padding:1rem;font-size:1rem;margin-top:1rem;border-radius:5px;border:none;cursor:pointer;color:var(--text-color);background-color:var(--primary-color);transition:background-color 0.3s;}.box button:hover{background-color:var(--secondary-color);}.box a{width:100%;}.theme-change{position:fixed;top:1rem;right:1rem;border-radius:5px;padding:0.5rem 1rem;border:none;cursor:pointer;color:var(--text-color);background:var(--primary-color);transition:background-color 0.3s;}.theme-change:hover{background-color:var(--secondary-color);}</style>";
    body += "</head>";

    body += "<body>";
    body += "<div class=\"dashboard\">";

    body += "<div class=\"sidebar\">";

    body += "<div class=\"sidebar-option\" onclick=\"window.location.href='/home'\">Home</div>";
    body += "<div class=\"sidebar-option\" onclick=\"window.location.href='/rand'\">Random</div>";
    body += "<div class=\"sidebar-option\" onclick=\"window.location.href='/up'\">Upload</div>";
    body += "<div class=\"sidebar-option\" onclick=\"window.location.href='/colors'\">Colors</div>";
    body += "<div class=\"sidebar-option\" onclick=\"window.location.href='/music'\">Music</div>";
    body += "<div class=\"sidebar-option\" onclick=\"window.location.href='/logout'\">Logout</div>";
    
    body += "</div>";

    body += "<div class=\"box\">";
    body += "  <h1>Rand</h1>";
    body += "  <p>A random number in [1, 10] is: " + randomNumber + "</p>";
    body += "  <p>SessionId: " + req->getSessionId() + "</p>";
    body += "</div>";

    body += "</div>";

    body += "<button class=\"theme-change\">Change Theme</button>";

    body += "<script>";
    body += "const toggleThemeBtn = document.querySelector('.theme-change');";
    body += "toggleThemeBtn.addEventListener('click', () => {";
    body += "document.body.classList.toggle('theme');";
    body += "});";
    body += "</script>";
    body += "</body>";

    body += "</html>";
    
    res->setBody(body);
    return res;
}

LoginHandler::LoginHandler(Instruction_Handler *ih)
    : RequestHandler()
{
    ih_ = ih;
}
Response *LoginHandler::callback(Request *req)
{
    std::string username = req->getBodyParam("username");
    std::string password = req->getBodyParam("password");
    if (username == "root")
    {
        throw Server::Exception("Remote root access has been disabled.");
    }
    QUESTION q;
    q.invalid = true;
    q.type = Post_Type;
    q.input = Login_Input;
    q.info.push_back("u");
    q.info.push_back(username);
    q.info.push_back("p");
    q.info.push_back(password);
    cout << q.info[1] << q.info[3];
    if (ih_->run(q).output == Ok)
    {
        Response *res = Response::redirect("/home");
        res->setSessionId("SID");
        return res;
    }
    else
    {
        Response *res = Response::redirect("/");
        res->setSessionId("SID");
        return res;
    }
}

LogoutHandler::LogoutHandler(Instruction_Handler *ih)
    : RequestHandler()
{
    ih_ = ih;
}
Response *LogoutHandler::callback(Request *req)
{
    QUESTION q;
    q.invalid = true;
    q.type = Post_Type;
    q.input = Logout_Input;
    ih_->run(q);
    Response *res = Response::redirect("/");
    return res;
}

UploadHandler::UploadHandler(Instruction_Handler *ih)
    : RequestHandler()
{
    ih_ = ih;
}
Response *UploadHandler::callback(Request *req)
{
    std::string name = ih_->user->id;
    std::string file = req->getBodyParam("file");
    utils::writeToFile(file, name);
    Response *res = Response::redirect("/home");
    return res;
}

ColorHandler::ColorHandler(const std::string &filePath)
    : TemplateHandler(filePath) {}

std::map<std::string, std::string> ColorHandler::handle(Request *req)
{
    std::string newName = "I am " + req->getQueryParam("name");
    std::map<std::string, std::string> context;
    context["name"] = newName;
    context["color"] = req->getQueryParam("color");
    return context;
}
