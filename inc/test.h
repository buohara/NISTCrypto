#pragma once

#include "commoninc.h"
#include "utils.h"
#include "bigint.h"

enum ResultCode
{
    PASS,
    FAIL,
    EXECUTION_ERROR,
    UNKNOWN
};

struct TestStats
{
    string groupName;

    TestStats(string groupNameIn) : groupName(groupNameIn), numPass(0),
        numFail(0), numExecErr(0), numCases(0) {}
    
    void IncPass()
    {
        numPass++;
        numCases++;
    }

    void IncFail()
    {
        numFail++;
        numCases++;
    }

    void IncExecErr()
    {
        numExecErr++;
        numCases++;
    }

    double GetPassPct() { return 100.0 * (double)numPass / (double)numCases; }
    double GetFailPct() { return 100.0 * (double)numFail / (double)numCases; }
    double GetExecErrPct() { return 100.0 * (double)numExecErr / (double)numCases; }

    uint64_t numPass;
    uint64_t numFail;
    uint64_t numExecErr;
    uint64_t numCases;

    double passPct;
    double failPct;
    double execErrPct;
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
TestResult TestCmpModBigIntCorrect();
TestResult TestSqrtBigIntCorrect();
TestResult TestGetModInverseBigInt();

TestResult GetNearestSquareFactors();

TestResult TestDiff();
TestResult TestPatch();

TestResult TestSHA224Short();
TestResult TestSHA256Short();
TestResult TestSHA384Short();
TestResult TestSHA512Short();

TestResult TestSHA224Long();
TestResult TestSHA256Long();
TestResult TestSHA384Long();
TestResult TestSHA512Long();

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

TestResult TestAESEncrypt128OFB();
TestResult TestAESEncrypt192OFB();
TestResult TestAESEncrypt256OFB();

TestResult TestAESDecrypt128OFB();
TestResult TestAESDecrypt192OFB();
TestResult TestAESDecrypt256OFB();

TestResult TestSigGen();