#pragma once

#include "global.hpp"

enum response
{
    Ok,
    Empty,
    NotFound,
    BadRequest,
    PermissionDenied,
    JustInformation
};

struct ANSWER
{
    response output;
    vector<string> info;
};