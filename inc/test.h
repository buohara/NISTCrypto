#pragma once

#include "commoninc.h"
#include "bigint.h"

enum ResultCode
{
    PASS,
    FAIL,
    UNKNOWN
};

struct TestResult
{
    vector<pair<ResultCode, string>> caseResults;
};

typedef TestResult(*pfnTestFunc)(void);

TestResult TestAssignBigInt();
TestResult TestCmpLShiftBigIntCorrect();
TestResult TestCmpRShiftBigIntCorrect();
TestResult TestCmpAddBigIntCorrect();
TestResult TestCmpSubBigIntCorrect();
TestResult TestCmpMulBigIntCorrect();
TestResult TestCmpDivBigIntCorrect();
TestResult TestSqrtBigIntCorrect();

TestResult TestDiff();
TestResult TestPatch();

TestResult TestSHA3();