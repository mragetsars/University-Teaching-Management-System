#ifndef HANDLERS_HPP_INCLUDE
#define HANDLERS_HPP_INCLUDE

#include <map>
#include <string>

#include "server.hpp"
#include "struct_question.hpp"
#include "struct_answer.hpp"
#include "handler_instruction.hpp"

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
