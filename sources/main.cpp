#include "global.hpp"
#include "io_csv.hpp"
#include "io_terminal.hpp"
#include "handler_instruction.hpp"
#include "server.hpp"
#include "web_handlers.hpp"

#ifndef _WIN32
#include <unistd.h>
#endif

void mapServerPaths(Instruction_Handler *ih, Server &server)
{
    server.setNotFoundErrPage("web/404.html");
    server.get("/", new LoginPageHandler());
    server.post("/", new LoginHandler(ih));
    server.get("/logout", new LogoutHandler(ih));
    server.get("/UTMS.png", new ShowImage("web/UTMS.png"));
    server.get("/asset", new UploadedAssetHandler());

    server.get("/home", new WebPageHandler(ih, WebPage::Home));
    server.get("/post/new", new WebPageHandler(ih, WebPage::NewPost));
    server.post("/post/new", new WebActionHandler(ih, WebAction::NewPost));
    server.get("/profile_photo", new WebPageHandler(ih, WebPage::ProfilePhoto));
    server.post("/profile_photo", new WebActionHandler(ih, WebAction::SaveProfilePhoto));
    server.post("/profile_photo/delete", new WebActionHandler(ih, WebAction::DeleteProfilePhoto));

    server.get("/courses", new WebPageHandler(ih, WebPage::Courses));
    server.get("/courses/add", new WebPageHandler(ih, WebPage::CourseOffer));
    server.post("/courses/add", new WebActionHandler(ih, WebAction::CourseOffer));

    server.get("/personal_page", new WebPageHandler(ih, WebPage::PersonalPage));
    server.get("/post", new WebPageHandler(ih, WebPage::UserPost));
    server.post("/post/delete", new WebActionHandler(ih, WebAction::DeletePost));
    server.get("/connect", new WebPageHandler(ih, WebPage::Connect));
    server.post("/connect", new WebActionHandler(ih, WebAction::Connect));
    server.get("/notifications", new WebPageHandler(ih, WebPage::Notifications));

    server.get("/my_courses", new WebPageHandler(ih, WebPage::MyCourses));
    server.get("/my_courses/add", new WebPageHandler(ih, WebPage::EnrollCourse));
    server.post("/my_courses/add", new WebActionHandler(ih, WebAction::EnrollCourse));
    server.get("/my_courses/delete", new WebPageHandler(ih, WebPage::DropCourse));
    server.post("/my_courses/delete", new WebActionHandler(ih, WebAction::DropCourse));

    server.get("/course_channel", new WebPageHandler(ih, WebPage::CourseChannel));
    server.get("/course_post", new WebPageHandler(ih, WebPage::CoursePost));
    server.get("/course_post/new", new WebPageHandler(ih, WebPage::NewCoursePost));
    server.post("/course_post/new", new WebActionHandler(ih, WebAction::NewCoursePost));

    server.get("/ta_form/new", new WebPageHandler(ih, WebPage::NewTaForm));
    server.post("/ta_form/new", new WebActionHandler(ih, WebAction::NewTaForm));
    server.get("/ta_request", new WebPageHandler(ih, WebPage::TaRequest));
    server.post("/ta_request", new WebActionHandler(ih, WebAction::TaRequest));
    server.get("/ta_form/close", new WebPageHandler(ih, WebPage::CloseTaForm));
    server.post("/ta_form/close", new WebActionHandler(ih, WebAction::CloseTaForm));
}

void run_cli(Instruction_Handler &instruction_handler)
{
    string line;
    while (getline(cin, line))
    {
        if (line.empty())
            continue;
        TERMINAL_PARSE_RESULT parsed = parse_terminal_command(line);
        if (!parsed.should_run)
        {
            ANSWER parse_answer;
            parse_answer.output = parsed.parse_error;
            write_terminal(parse_answer);
            continue;
        }
        write_terminal(instruction_handler.run(parsed.question));
    }
}

bool should_run_cli(int argc, char *argv[])
{
    if (argc >= 6)
    {
        string mode = argv[5];
        if (mode == "--cli")
            return true;
        if (mode == "--web")
            return false;
    }
#ifndef _WIN32
    return !isatty(STDIN_FILENO);
#else
    return false;
#endif
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        cerr << "Usage: " << argv[0] << " <majors.csv> <students.csv> <courses.csv> <professors.csv> [--cli|--web]" << endl;
        return 1;
    }

    Instruction_Handler instruction_handler(read_csv(argv[1], argv[2], argv[3], argv[4]));
    if (should_run_cli(argc, argv))
    {
        run_cli(instruction_handler);
        return 0;
    }

    try
    {
        int port = 5000;
        Server server(port);
        mapServerPaths(&instruction_handler, server);
        cout << "Server running on port: " << port << endl;
        server.run();
    }
    catch (const invalid_argument &e)
    {
        cerr << e.what() << endl;
    }
    catch (const Server::Exception &e)
    {
        cerr << e.getMessage() << endl;
    }
    return 0;
}
