#ifndef HANDLERS_HPP_INCLUDE
#define HANDLERS_HPP_INCLUDE

#include <map>
#include <string>

#include "server.hpp"
#include "struct_question.hpp"
#include "struct_answer.hpp"
#include "handler_instruction.hpp"
#include "io_terminal.hpp"

class LoginPageHandler : public RequestHandler
{
public:
    Response *callback(Request *) override;
};

class HomeHandler : public RequestHandler
{
private:
    Instruction_Handler *ih_;

public:
    HomeHandler(Instruction_Handler *ih);
    Response *callback(Request *) override;
};

class ConsolePageHandler : public RequestHandler
{
public:
    Response *callback(Request *) override;
};

class ConsoleCommandHandler : public RequestHandler
{
private:
    Instruction_Handler *ih_;

public:
    ConsoleCommandHandler(Instruction_Handler *ih);
    Response *callback(Request *) override;
};

class RandomNumberHandler : public RequestHandler
{
public:
    Response *callback(Request *) override;
};

class LoginHandler : public RequestHandler
{
private:
    Instruction_Handler *ih_;

public:
    LoginHandler(Instruction_Handler *ih);
    Response *callback(Request *) override;
};

class LogoutHandler : public RequestHandler
{
private:
    Instruction_Handler *ih_;

public:
    LogoutHandler(Instruction_Handler *ih);
    Response *callback(Request *) override;
};

class UploadHandler : public RequestHandler
{
private:
    Instruction_Handler *ih_;

public:
    UploadHandler(Instruction_Handler *ih);
    Response *callback(Request *) override;
};

class ColorHandler : public TemplateHandler
{
public:
    ColorHandler(const std::string &filePath);
    std::map<std::string, std::string> handle(Request *req) override;
};

#endif // HANDLERS_HPP_INCLUDE
