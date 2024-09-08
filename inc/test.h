#pragma once

#include "commoninc.h"

enum ResultCode
{
    PASS,
    FAIL,
    UNKNOWN
};

struct TestResult
{
    ResultCode code;
    string msg;

    TestResult() : code(UNKNOWN), msg("") {};
};