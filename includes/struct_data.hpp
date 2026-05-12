#pragma once

#include "global.hpp"

#define SYSTEM_MANAGER_ID "0"
#define SYSTEM_MANAGER_MAJOR_ID ""
#define SYSTEM_MANAGER_NAME "admin"
#define SYSTEM_MANAGER_PASSWORD "UT_account"

enum pos
{
    instructor,
    assistant_professor,
    associate_professor,
    professor
};

struct MAJOR
{
    string id;
    string name;
};
struct COURSE
{
    string id;
    string name;
    int credit;
    int prerequisite;
    vector<string> major_ids;
};

struct USER
{
    string id;
    string name;
    string major_id;
    string password;
};
struct STUDENT
{
    USER info;
    int semester;
};
struct PROFESSOR
{
    USER info;
    pos position;
};

struct DATA
{
    USER system_manager;
    vector<MAJOR> majors;
    vector<COURSE> courses;
    vector<STUDENT> students;
    vector<PROFESSOR> professors;
};

struct POST
{
    int id;
    string title;
    string message;
    string image_address;
};
struct CHANNEL_POST
{
    int id;
    string author_name;
    string title;
    string message;
    string image_address;
};
struct NOTIFICATION
{
    string id;
    string name;
    string subject;
};

enum day
{
    Saturday,
    Sunday,
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday
};
struct TIME
{
    day week;
    int start;
    int end;
};
struct DATE
{
    int day;
    int month;
    int year;
};

int str_to_int(string input);
string int_to_str(int input);

pos str_to_pos(string input);
string pos_to_str(pos input);

day str_to_day(string input);
string day_to_str(day input);

TIME string_to_time(string input);

DATE string_to_date(string input);