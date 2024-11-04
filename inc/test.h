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

TestResult TestAESEncrypt128ECB();
TestResult TestAESDecrypt128ECB();
TestResult TestAESEncrypt192ECB();
TestResult TestAESDecrypt192ECB();
TestResult TestAESEncrypt256ECB();
TestResult TestAESDecrypt256ECB();

TestResult TestAESEncrypt128CBC();
TestResult TestAESEncrypt192CBC();
TestResult TestAESEncrypt256CBC();
TestResult TestAESDecrypt128CBC();
TestResult TestAESDecrypt192CBC();
TestResult TestAESDecrypt256CBC();

TestResult TestAESEncrypt128CFB1();
TestResult TestAESEncrypt128CFB8();
TestResult TestAESEncrypt128CFB128();

TestResult TestAESEncrypt192CFB1();
TestResult TestAESEncrypt192CFB8();
TestResult TestAESEncrypt192CFB128();

TestResult TestAESEncrypt256CFB1();
TestResult TestAESEncrypt256CFB8();
TestResult TestAESEncrypt256CFB128();

TestResult TestAESDecrypt128CFB1();
TestResult TestAESDecrypt128CFB8();
TestResult TestAESDecrypt128CFB128();

TestResult TestAESDecrypt192CFB1();
TestResult TestAESDecrypt192CFB8();
TestResult TestAESDecrypt192CFB128();

TestResult TestAESDecrypt256CFB1();
TestResult TestAESDecrypt256CFB8();
TestResult TestAESDecrypt256CFB128();

TestResult TestAESEncrypt256OFB();