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
            //{ "TestSHA3512Long",            TestSHA3512Long },
            { "TestSHA3512Short",           TestSHA3512Short },
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
            "Encrypt",
            "Encryption algorithm unit tests."
        },
        {
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