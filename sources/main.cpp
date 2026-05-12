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
    server.get("/home", new HomeHandler(ih));
    server.get("/console", new ConsolePageHandler());
    server.post("/console", new ConsoleCommandHandler(ih));
    server.get("/UTMS.png", new ShowImage("web/UTMS.png"));
    server.get("/up", new ShowPage("web/upload_form.html"));
    server.post("/up", new UploadHandler(ih));
    server.get("/colors", new ColorHandler("web/colors.html"));
    server.get("/music", new ShowPage("web/music.html"));
    server.get("/music/moonlight.mp3", new ShowFile("web/moonlight.mp3", "audio/mpeg"));
    server.get("/logout", new LogoutHandler(ih));
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
