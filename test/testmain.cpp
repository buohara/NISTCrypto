#include "commoninc.h"
#include "test.h"

#include "hash.h"

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
    vector<uint64_t> testIDs;
};

map<ResultCode, string> ResultCodeStrings =
{
    { PASS, "PASS" },
    { FAIL, "FAIL" },
    { UNKNOWN, "UNKNOWN" }
};

TestGroupList testGroups =
{
    {
        { 
            "BigInt",
            "Arbitrary size integer unit tests."
        },
        {
            { "TestAssignBigInt",           TestAssignBigInt },
            { "TestCmpLShiftBigIntCorrect", TestCmpLShiftBigIntCorrect },
            { "TestCmpRShiftBigIntCorrect", TestCmpRShiftBigIntCorrect },
            { "TestCmpAddBigIntCorrect",    TestCmpAddBigIntCorrect },
            { "TestCmpSubBigIntCorrect",    TestCmpSubBigIntCorrect },
            { "TestCmpMulBigIntCorrect",    TestCmpMulBigIntCorrect },
            { "TestCmpDivBigIntCorrect",    TestCmpDivBigIntCorrect },
            { "TestSqrtBigIntCorrect",      TestSqrtBigIntCorrect }
        }
    },

    {
        { 
            "Diff",
            "File version diff and patch unit tests."
        },
        {
            { "TestDiff",                   TestDiff },
            { "TestPatch",                  TestPatch },
        }
    },

    {
        { 
            "Hash",
            "Hash algorithm unit tests."
        },
        {
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

    {
        { 
            "Compress",
            "Compression algorithm unit tests."
        },
        {
        }
    },

    {
        { 
            "Block Cipher",
            "Block cipher unit tests for AES."
        },
        {
            /*{ "TestAESEncrypt256ECB",       TestAESEncrypt256ECB },
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
            { "TestAESEncrypt256CFB128",    TestAESEncrypt256CFB128 },*/
            { "TestAESDecrypt128CFB1",      TestAESDecrypt128CFB1 },
            { "TestAESDecrypt128CFB8",      TestAESDecrypt128CFB8 },
            { "TestAESDecrypt128CFB128",    TestAESDecrypt128CFB128 },
            { "TestAESDecrypt192CFB1",      TestAESDecrypt192CFB1 },
            { "TestAESDecrypt192CFB8",      TestAESDecrypt192CFB8 },
            { "TestAESDecrypt192CFB128",    TestAESDecrypt192CFB128 },
            { "TestAESDecrypt256CFB1",      TestAESDecrypt256CFB1 },
            { "TestAESDecrypt256CFB8",      TestAESDecrypt256CFB8 },
            { "TestAESDecrypt256CFB128",    TestAESDecrypt256CFB128 }
        }
    }
};

/**
 * PrintHelp - Print off usage and list of available tests.
 */

void PrintHelp()
{
    printf("Handshake Test Suite Version %lu.%lu\n\n", VERSION_MAJOR, VERSION_MINOR);

    printf("Usage: handshaketest.exe [-r <x,y,z...>] [-o <path>]\n\n");
    printf("    -r    Run a list of numbered test groups, e.g., '-r 1, 3, 4'. See test groups below.\n");
    printf("    -o    Output test log file.\n\n");

    printf("Available Test Groups:\n\n");

    for (uint64_t i = 0; i < testGroups.size(); i++)
        printf("    %llu - %s: %s\n", i + 1, testGroups[i].first.first.c_str(), testGroups[i].first.second.c_str());
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

static bool IsWhitespace(const char inChar)
{
    if (inChar == 32 || inChar == 9 || inChar == 10 ||
        inChar == 13 || inChar == 12 || inChar == 11)
        return true;
    
    return false;
}

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

            string testString = args[i];

            for (uint64_t j = 0; j < testString.length(); j++)
            {
                if (IsNumeric(testString[j]))
                {
                    argsOut.testIDs.push_back(testString[j] - '0');
                    continue;
                }

                if (IsWhitespace(testString[j]))
                    continue;

                if (testString[j] == ',')
                    continue;

                throw invalid_argument("Unexpected character processing -r flag in Handshake tests: " +
                    testString[j]);
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
 * LogMessage - Logging helper. Print messages to a log file or console
 * depending on log file config.
 *
 * @param format        [in]  Log message format.
 * @param args (...)    [in]  Variadic argument list.
 */

static void LogMessage(const char* format, ...)
{
    const uint64_t bufSize = 512;
    char msgBuf[bufSize];

    va_list args;
    va_start(args, format);

    vsnprintf(msgBuf, sizeof(msgBuf), format, args);
    string msgStr = string(msgBuf);

    if (gbLogToFile)
        fwrite(msgStr.c_str(), sizeof(char), msgStr.size(), gpLogFile);
    else
        printf("%s\n", msgBuf);

    va_end(args);
}

/**
 * main - Print off usage and list of available tests.
 *
 * @param argc  [in]    Number of command line arguments.
 * @param argv  [in]    List of argument strings.
 *
 * @return Return 0 on exit.
 */

void RunTests(TestArgs& args)
{
    InitLogFile();
    srand(time(nullptr));

    for (uint64_t i = 0; i < args.testIDs.size(); i++)
    {
        uint64_t groupID        = args.testIDs[i] - 1;
        string groupName        = testGroups[groupID].first.first;
        TestGroupCases& cases   = testGroups[groupID].second;

        LogMessage("Beginning test group %s...\n\n", groupName.c_str());

        if (args.testIDs[i] > testGroups.size())
            throw invalid_argument("Invalid test group ID encountered running Handshake tests.");

        for (uint64_t j = 0; j < cases.size(); j++)
        {
            LogMessage("Beginning test %s...\n\n", cases[j].first.c_str());
            pfnTestFunc pfnTest = cases[j].second;

            try
            {
                TestResult res = pfnTest();

                for (uint64_t k = 0; k < res.caseResults.size(); k++)
                {
                    LogMessage("Case %llu %s %s\n", k, ResultCodeStrings[res.caseResults[k].first].c_str(),
                        res.caseResults[k].second.c_str());
                }
            }
            catch (exception& e)
            {
                LogMessage("Exception encountered in Handshake tests: '%s'. Continuing to next case.\n",
                    e.what());
            }

            LogMessage("\n");
        }

        LogMessage("\n");
    }

    CloseLogFile();
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

static uint8_t GFMult(uint8_t a, uint8_t b)
{
    uint8_t tmpA            = a;
    uint8_t tmpB            = b;
    const uint8_t bitHi     = 0x80;
    const uint8_t mod       = 0x1B;
    uint8_t xTimes[8]       = { 0 };
    uint8_t e               = 0;
    uint8_t out             = 0;
    xTimes[0]               = tmpA;

    for (uint8_t i = 1; i < 8; i++)
    {
        if (tmpA & bitHi)
            tmpA = (tmpA << 1) ^ mod;
        else
            tmpA <<= 1;

        xTimes[i] = tmpA;
    }

    while (tmpB)
    {
        if (tmpB & 0x1)
            out ^= xTimes[e];

        e++;
        tmpB >>= 1;
    }

    return out;
}

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