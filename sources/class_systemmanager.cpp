#include "class_systemmanager.hpp"

System_Manager::System_Manager(USER input_info)
    : User(input_info)
{

}
System_Manager::System_Manager() {}

response System_Manager::show_info(vector<string> *output, vector<MAJOR> *majors)
{
    output->push_back(NICK_NMAE);
    output->push_back(ENTER);
    return JustInformation;
}