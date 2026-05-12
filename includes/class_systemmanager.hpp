#pragma once

#include "global.hpp"
#include "class_user.hpp"

#define NICK_NMAE "UT_account"

class System_Manager : public User
{
private:
public:
    System_Manager(USER input_info);
    System_Manager();
    virtual response show_info(vector<string> *output, vector<MAJOR> *majors) override;
};