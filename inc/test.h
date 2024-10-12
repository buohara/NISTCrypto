#pragma once

#include "commoninc.h"
#include "utils.h"
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

TestResult TestSHA3224Short();
TestResult TestSHA3224Long();
TestResult TestSHA3224Monte();

TestResult TestSHA3256Short();
TestResult TestSHA3256Long();
TestResult TestSHA3256Monte();

TestResult TestSHA3384Short();
TestResult TestSHA3384Long();
TestResult TestSHA3384Monte();

TestResult TestSHA3512Short();
TestResult TestSHA3512Long();
TestResult TestSHA3512Monte();

TestResult TestAESEncrypt256ECB();
TestResult TestAESDecrypt256ECB();