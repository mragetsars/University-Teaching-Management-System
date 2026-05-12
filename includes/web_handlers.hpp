#pragma once

#include <map>
#include <string>

#include "server.hpp"
#include "struct_question.hpp"
#include "struct_answer.hpp"
#include "handler_instruction.hpp"
#include "io_terminal.hpp"

enum class WebPage
{
    Home,
    NewPost,
    ProfilePhoto,
    Courses,
    PersonalPage,
    UserPost,
    Connect,
    Notifications,
    MyCourses,
    EnrollCourse,
    DropCourse,
    CourseOffer,
    CourseChannel,
    CoursePost,
    NewCoursePost,
    NewTaForm,
    TaRequest,
    CloseTaForm
};

enum class WebAction
{
    NewPost,
    DeletePost,
    SaveProfilePhoto,
    DeleteProfilePhoto,
    Connect,
    EnrollCourse,
    DropCourse,
    CourseOffer,
    NewCoursePost,
    NewTaForm,
    TaRequest,
    CloseTaForm
};

class LoginPageHandler : public RequestHandler
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

class WebPageHandler : public RequestHandler
{
private:
    Instruction_Handler *ih_;
    WebPage page_;

public:
    WebPageHandler(Instruction_Handler *ih, WebPage page);
    Response *callback(Request *) override;
};

class WebActionHandler : public RequestHandler
{
private:
    Instruction_Handler *ih_;
    WebAction action_;

public:
    WebActionHandler(Instruction_Handler *ih, WebAction action);
    Response *callback(Request *) override;
};

class UploadedAssetHandler : public RequestHandler
{
public:
    Response *callback(Request *) override;
};
