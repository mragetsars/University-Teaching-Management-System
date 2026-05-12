#include "global.hpp"
#include "io_csv.hpp"
#include "io_terminal.hpp"
#include "handler_instruction.hpp"
#include "server.hpp"
#include "web_handlers.hpp"

void mapServerPaths(Instruction_Handler *ih, Server &server)
{
    server.setNotFoundErrPage("web/404.html");
    server.get("/", new ShowPage("web/login.html"));
    server.post("/", new LoginHandler(ih));
    server.get("/home", new ShowPage("web/home.html"));
    server.get("/UTMS.png", new ShowImage("web/UTMS.png"));
    server.get("/rand", new RandomNumberHandler());
    server.get("/up", new ShowPage("web/upload_form.html"));
    server.post("/up", new UploadHandler(ih));
    server.get("/colors", new ColorHandler("web/colors.html"));
    server.get("/music", new ShowPage("web/music.html"));
    server.get("/music/moonlight.mp3", new ShowFile("web/moonlight.mp3", "audio/mpeg"));
    server.get("/logout", new LogoutHandler(ih));
}

int main(int argc, char *argv[])
{
    Instruction_Handler instruction_handler(read_csv(argv[1], argv[2], argv[3], argv[4]));
    try
    {
        int port = 5000;
        Server server(port);
        mapServerPaths(&instruction_handler, server);
        std::cout << "Server running on port: " << port << std::endl;
        server.run();
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (const Server::Exception &e)
    {
        std::cerr << e.getMessage() << std::endl;
    }
    return 0;
}