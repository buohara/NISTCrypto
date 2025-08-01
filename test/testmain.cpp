#include "commoninc.h"
#include "test.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 0

typedef pair<string, string> TestGroupDesc;
typedef pair<string, pfnTestFunc> TestCase;
typedef vector<TestCase> TestGroupCases;
typedef pair<TestGroupDesc, TestGroupCases> TestGroup;
typedef vector<TestGroup> TestGroupList;

static bool gbLogToFile         = false;
static string gLogFilePath      = "";
static FILE *gpLogFile          = nullptr;

struct TestArgs
{
    vector<uint64_t> testGroupIDs;
};

map<ResultCode, string> ResultCodeStrings =
{
    { PASS,     "PASS" },
    { FAIL,     "FAIL" },
    { UNKNOWN,  "UNKNOWN" }
};

TestGroupList testGroups =
{
    {
        { 
            "BigInt",
            "Arbitrary size integer unit tests."
        },
        {
            {"TestAssignBigInt",           TestAssignBigInt},
            { "TestCmpLShiftBigIntCorrect", TestCmpLShiftBigIntCorrect },
            { "TestCmpRShiftBigIntCorrect", TestCmpRShiftBigIntCorrect },
            { "TestCmpAddBigIntCorrect",    TestCmpAddBigIntCorrect },
            { "TestCmpSubBigIntCorrect",    TestCmpSubBigIntCorrect },
            { "TestCmpMulBigIntCorrect",    TestCmpMulBigIntCorrect },
            { "TestCmpDivBigIntCorrect",    TestCmpDivBigIntCorrect },
            { "TestCmpModBigIntCorrect",    TestCmpModBigIntCorrect },
            { "TestSqrtBigIntCorrect",      TestSqrtBigIntCorrect }
            //{ "TestGetModInverseBigInt",    TestGetModInverseBigInt }
        }
    },

    // {
    //     {
    //         "Number",
    //         "Number Experiments"
    //     },
    //     {
    //         { "GenerateNearestSquareFactors", GetNearestSquareFactors }
    //     },
    // },

    // {
    //     { 
    //         "Diff",
    //         "File version diff and patch unit tests."
    //     },
    //     {
    //         { "TestDiff",                   TestDiff },
    //         { "TestPatch",                  TestPatch },
    //     }
    // },

    {
        { 
            "Hash",
            "SHA224/256/384/512 and SHA3 tests."
        },
        {
            { "TestSHA224Short",            TestSHA224Short },
            { "TestSHA256Short",            TestSHA256Short },
            { "TestSHA384Short",            TestSHA384Short },
            { "TestSHA512Short",            TestSHA512Short },
            { "TestSHA3224Short",           TestSHA3224Short },
            { "TestSHA3224Long",            TestSHA3224Long },
            { "TestSHA3224Monte",           TestSHA3224Monte },
            { "TestSHA3256Short",           TestSHA3256Short },
            { "TestSHA3256Long",            TestSHA3256Long },
            { "TestSHA3256Monte",           TestSHA3256Monte },
            { "TestSHA3384Short",           TestSHA3384Short },
            { "TestSHA3384Long",            TestSHA3384Long },
            { "TestSHA3384Monte",           TestSHA3384Monte },
            { "TestSHA3512Short",           TestSHA3512Short },
            { "TestSHA3512Long",            TestSHA3512Long },
            { "TestSHA3512Monte",           TestSHA3512Monte }
        }
    },

    // {
    //     { 
    //         "Compress",
    //         "Compression algorithm unit tests."
    //     },
    //     {
    //     }
    // },

    {
        { 
            "Block Cipher",
            "AES tests."
        },
        {
            { "TestAESEncrypt256ECB",       TestAESEncrypt256ECB },
            { "TestAESDecrypt256ECB",       TestAESDecrypt256ECB },
            { "TestAESEncrypt192ECB",       TestAESEncrypt192ECB },
            { "TestAESDecrypt192ECB",       TestAESDecrypt192ECB },
            { "TestAESEncrypt128ECB",       TestAESEncrypt128ECB },
            { "TestAESDecrypt128ECB",       TestAESDecrypt128ECB },
            { "TestAESEncrypt128CBC",       TestAESEncrypt128CBC },
            { "TestAESEncrypt192CBC",       TestAESEncrypt192CBC },
            { "TestAESEncrypt256CBC",       TestAESEncrypt256CBC },
            { "TestAESDecrypt128CBC",       TestAESDecrypt128CBC },
            { "TestAESDecrypt192CBC",       TestAESDecrypt192CBC },
            { "TestAESDecrypt256CBC",       TestAESDecrypt256CBC },
            { "TestAESEncrypt128CFB1",      TestAESEncrypt128CFB1 },
            { "TestAESEncrypt128CFB8",      TestAESEncrypt128CFB8 },
            { "TestAESEncrypt128CFB128",    TestAESEncrypt128CFB128 },
            { "TestAESEncrypt192CFB1",      TestAESEncrypt192CFB1 },
            { "TestAESEncrypt192CFB8",      TestAESEncrypt192CFB8 },
            { "TestAESEncrypt192CFB128",    TestAESEncrypt192CFB128 },
            { "TestAESEncrypt256CFB1",      TestAESEncrypt256CFB1 },
            { "TestAESEncrypt256CFB8",      TestAESEncrypt256CFB8 },
            { "TestAESEncrypt256CFB128",    TestAESEncrypt256CFB128 },
            { "TestAESDecrypt128CFB1",      TestAESDecrypt128CFB1 },
            { "TestAESDecrypt128CFB8",      TestAESDecrypt128CFB8 },
            { "TestAESDecrypt128CFB128",    TestAESDecrypt128CFB128 },
            { "TestAESDecrypt192CFB1",      TestAESDecrypt192CFB1 },
            { "TestAESDecrypt192CFB8",      TestAESDecrypt192CFB8 },
            { "TestAESDecrypt192CFB128",    TestAESDecrypt192CFB128 },
            { "TestAESDecrypt256CFB1",      TestAESDecrypt256CFB1 },
            { "TestAESDecrypt256CFB8",      TestAESDecrypt256CFB8 },
            { "TestAESDecrypt256CFB128",    TestAESDecrypt256CFB128 },
            { "TestAESEncrypt128OFB",       TestAESEncrypt128OFB },
            { "TestAESEncrypt192OFB",       TestAESEncrypt192OFB },
            { "TestAESEncrypt256OFB",       TestAESEncrypt256OFB },
            { "TestAESDecrypt128OFB",       TestAESDecrypt128OFB },
            { "TestAESDecrypt192OFB",       TestAESDecrypt192OFB },
            { "TestAESDecrypt256OFB",       TestAESDecrypt256OFB }
        }
    },

    // {
    //     {
    //         "DSA",
    //             "Digital signature tests."
    //     },
    //     {
    //         { "TestSigGen", TestSigGen }
    //     }
    // },
};

/**
 * PrintHelp - Print off usage and list of available tests.
 */

void PrintHelp()
{
    printf("NISTCrypto Version %d.%d\n\n", VERSION_MAJOR, VERSION_MINOR);

    printf("Usage: ./NISTCrypto [-r <x,y,z...>] [-o <path>]\n\n");
    printf("    -r    Run a comma-separated list of numbered test groups, e.g., '-r 1, 3, 4'. See test groups below.\n");
    printf("    -o    Output test log file.\n\n");

    printf("Available Test Groups:\n\n");

    for (uint64_t i = 0; i < testGroups.size(); i++)
        printf("    %lu - %s: %s\n", i + 1, testGroups[i].first.first.c_str(), testGroups[i].first.second.c_str());

    printf("\n");
}

/**
 * IsNumeric - Check if an input character is a number.
 * 
 * @paran inChar    [in] Check if this character is a number.
 * 
 * @return True if input is a number.
 */

static bool IsNumeric(const char inChar)
{
    if (inChar >= '0' && inChar <= '9')
        return true;

    return false;
}

/**
 * IsWhitespace - Check if an input character is whitespace.
 *
 * @paran inChar    [in] Check if this character is whitespace.
 *
 * @return True if input is whitespace.
 */

// static bool IsWhitespace(const char inChar)
// {
//     if (inChar == 32 || inChar == 9 || inChar == 10 ||
//         inChar == 13 || inChar == 12 || inChar == 11)
//         return true;
    
//     return false;
// }

/**
 * PrintHelp - Print off usage and list of available tests.
 */

void ParseCommandLine(vector<string> &args, TestArgs &argsOut)
{
    if (args.size() == 0)
    {
        PrintHelp();
        exit(0);
    }

    for (uint64_t i = 0; i < args.size(); i++)
    {
        if (args[i] == "-h" || args[i] == "/?" || args[i] == "-help")
        {
            PrintHelp();
            exit(0);
        }

        if (args[i] == "-r")
        {
            i++;

            if (i >= args.size())
            {
                printf("Error: Expected test group IDs after -r flag.\n");
                exit(1);
            }

            string testString = args[i];

            for (uint64_t j = 0; j < testString.length(); j++)
            {
                if (IsNumeric(testString[j]))
                {
                    argsOut.testGroupIDs.push_back(testString[j] - '0');
                    continue;
                }
                else if (testString[j] == ',')
                {
                    continue;
                }
                else
                {
                    printf("Error: Unexpected character '%c' processing -r flag in Handshake tests.\n", testString[j]);
                    exit(1);
                }
            }

            continue;
        }

        if (args[i] == "-o")
        {
            i++;
            gbLogToFile     = true;
            gLogFilePath    = args[i];
            continue;
        }
    }
}

/**
 * InitLogFile - If a log file path was specified on the command line,
 * try opening it here.
 */

static void InitLogFile()
{
    if (gbLogToFile)
    {
        gpLogFile = fopen(gLogFilePath.c_str(), "w");

        if (gpLogFile == nullptr)
        {
            throw invalid_argument("Failed to write Handshake test log file " + gLogFilePath);
            gbLogToFile = false;
        }
    }
}

/**
 * CloseLogFile - Close log file if specified on the command line.
 */

static void CloseLogFile()
{
    if (gbLogToFile)
        fclose(gpLogFile);
}

/**
 * PrintStats - Print a summary of test group pass/fail rates and total pass/fail rates.
 *
 * @param runStats        [in]  List of test group pass/fail rates.
 */

static void PrintStats(vector<TestStats>& runStats)
{
    uint64_t runPass        = 0;
    uint64_t runFail        = 0;
    uint64_t runExecErr     = 0;
    uint64_t runCases       = 0;

    for (uint64_t i = 0; i < runStats.size(); i++)
    {
        runPass     += runStats[i].numPass;
        runFail     += runStats[i].numFail;
        runExecErr  += runStats[i].numExecErr;
        runCases    += runStats[i].numCases;
    }

    printf("\nSummary:\n\n");
    printf("Total Groups: %lu\n", runStats.size());
    printf("Total Cases: %lu\n", runCases);
    printf("Pass: %lu/%lu (%3.2f%%)\n", runPass, runCases, 100.0 * ((double)runPass/(double)runCases));
    printf("Fail: %lu/%lu (%3.2f%%)\n", runFail, runCases, 100.0 * ((double)runFail / (double)runCases));
    printf("Execution Errors: %lu/%lu (%3.2f%%)\n\n", runExecErr, runCases, 100.0 * ((double)runExecErr / (double)runCases));

    printf("Test Group Summary:\n\n");

    for (uint64_t i = 0; i < runStats.size(); i++)
    {
        printf("'%s'\n", runStats[i].groupName.c_str());
        printf("Pass: %lu/%lu (%3.2f%%)\n", runStats[i].numPass, runStats[i].numCases, runStats[i].GetPassPct());
        printf("Fail: %lu/%lu (%3.2f%%)\n", runStats[i].numFail, runStats[i].numCases, runStats[i].GetFailPct());
        printf("Execution Errors: %lu/%lu (%3.2f%%)\n\n", runStats[i].numExecErr, runStats[i].numCases, runStats[i].GetExecErrPct());
    }
}

/**
 * LogToFile - Write test results to a log file if specified on the command line.
 *
 * @param testResults   [in] List of test results to log.
 */

static void LogToFile(vector<vector<TestResult>>& testResults, vector<TestStats>& runStats)
{
    InitLogFile();

    for (uint64_t i = 0; i < runStats.size(); i++)
    {
        fprintf(gpLogFile, "Test Group: %s\n", runStats[i].groupName.c_str());
        fprintf(gpLogFile, "Pass: %lu/%lu (%3.2f%%)\n", runStats[i].numPass, runStats[i].numCases, runStats[i].GetPassPct());
        fprintf(gpLogFile, "Fail: %lu/%lu (%3.2f%%)\n", runStats[i].numFail, runStats[i].numCases, runStats[i].GetFailPct());
        fprintf(gpLogFile, "Execution Errors: %lu/%lu (%3.2f%%)\n\n", runStats[i].numExecErr, runStats[i].numCases, runStats[i].GetExecErrPct());
    }

    const uint32_t nGroups = static_cast<uint32_t>(testResults.size());

    for (uint64_t i = 0; i < nGroups; i++)
    {
        const uint32_t nCases = static_cast<uint32_t>(testResults[i].size());

        for (uint32_t j = 0; j < nCases; j++)
        {
            const uint32_t nCaseResults = static_cast<uint32_t>(testResults[i][j].caseResults.size());


            for (uint32_t k = 0; k < nCaseResults; k++)
            {
                ResultCode resCode = testResults[i][j].caseResults[k].first;
                string resMsg      = testResults[i][j].caseResults[k].second;

                fprintf(gpLogFile, "Test Group %lu, Test Case %u: %s\n", i + 1, j + 1, ResultCodeStrings[resCode].c_str());

                if (resMsg.length() > 0)
                    fprintf(gpLogFile, "Message: %s\n", resMsg.c_str());
            }
        }
    }

    CloseLogFile();
}

/**
 * RunTests - Run a set of tests based on the command line arguments.
 *
 * @param args [in] List of tests groups to run.
 */

void RunTests(TestArgs& args)
{
    srand(time(nullptr));

    const uint64_t numTestGroups = args.testGroupIDs.size();
    vector<TestStats> runStats;
    vector<vector<TestResult>> testResults;

    for (uint64_t i = 0; i < numTestGroups; i++)
    {
        uint64_t groupID                = args.testGroupIDs[i] - 1;
        string groupName                = testGroups[groupID].first.first;
        TestGroupCases& groupCases      = testGroups[groupID].second;
        const uint64_t numGroupCases    = groupCases.size();

        testResults.resize(numGroupCases);
        TestStats groupStats(groupName);

        if (groupID > testGroups.size())
            throw invalid_argument("Invalid test group ID encountered running Handshake tests.");

        for (uint64_t j = 0; j < numGroupCases; j++)
        {
            string caseName     = groupCases[j].first.c_str();
            pfnTestFunc pfnCase = groupCases[j].second;

            printf("Executing test group '%s' (%lu/%lu) - Test case '%s' (%lu/%lu)\n",
                groupName.c_str(),
                i + 1,
                numTestGroups,
                caseName.c_str(),
                j + 1,
                numGroupCases
            );

            try
            {
                TestResult res = pfnCase();
                testResults[i].push_back(res);

                for (uint64_t k = 0; k < res.caseResults.size(); k++)
                {
                    if (res.caseResults[k].first == PASS)
                        groupStats.IncPass();
                    else
                        groupStats.IncFail();
                }
            }
            catch (exception& e)
            {
                printf("Exception encountered during Handshake RunTests with message '%s'." \
                    " Continuing.",
                    e.what()
                );

                groupStats.IncExecErr();
            }
        }

        runStats.push_back(groupStats);
    }

    if (gbLogToFile)
        LogToFile(testResults, runStats);
    else
        PrintStats(runStats);
}

/**
 * GFMult - Multiply two 8-bit numbers as elements of GF(2^8) (see FIPS 197). Multiply
 * the two 8-bit values as polynomials mod 2, then reduce by the irreducible/prime polynomial
 * 0b100011011. This follows the iterative XTimes method outlined in FIPS 197.
 *
 * @param a [in] First factor.
 * @param b [in] Second factor.
 *
 * @return Product of a and b over GF(2^8).
 */

// static uint8_t GFMult(uint8_t a, uint8_t b)
// {
//     uint8_t tmpA            = a;
//     uint8_t tmpB            = b;
//     const uint8_t bitHi     = 0x80;
//     const uint8_t mod       = 0x1B;
//     uint8_t xTimes[8]       = { 0 };
//     uint8_t e               = 0;
//     uint8_t out             = 0;
//     xTimes[0]               = tmpA;

//     for (uint8_t i = 1; i < 8; i++)
//     {
//         if (tmpA & bitHi)
//             tmpA = (tmpA << 1) ^ mod;
//         else
//             tmpA <<= 1;

//         xTimes[i] = tmpA;
//     }

//     while (tmpB)
//     {
//         if (tmpB & 0x1)
//             out ^= xTimes[e];

//         e++;
//         tmpB >>= 1;
//     }

//     return out;
// }

/**
 * main - Print off usage and list of available tests.
 * 
 * @param argc  [in]    Number of command line arguments.
 * @param argv  [in]    List of argument strings.
 * 
 * @return Return 0 on exit.
 */

int main(int argc, char** argv)
{
    vector<string> args(argv + 1, argv + argc);
    TestArgs argsOut;

    ParseCommandLine(args, argsOut);
    RunTests(argsOut);

    return 0;
}