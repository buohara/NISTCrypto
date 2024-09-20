#include "commoninc.h"
#include "test.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 0

struct TestArgs
{
    vector<uint64_t> testIDs;
    string logFile;
};

map<ResultCode, string> ResultCodeStrings =
{
    { PASS, "PASS" },
    { FAIL, "FAIL" },
    { UNKNOWN, "UNKNOWN" }
};

vector<pair<pair<string, string>, vector<pair<string, pfnTestFunc>>>> testGroups =
{
    {
        { "BigInt", "Arbitrary size integer unit tests." },
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
        { "Diff", "File version diff and patch unit tests."},
        {
            { "TestDiff",                   TestDiff },
            { "TestPatch",                  TestPatch },
        }
    },

    {
        { "Hash", "Hash algorithm unit tests." },
        {
            { "TestSHA3512Long",            TestSHA3512Long },
            { "TestSHA3512Short",           TestSHA3512Short },
            { "TestSHA3512Monte",           TestSHA3512Monte },
        }
    },

    {
        { "Comopress", "Compression algorithm unit tests." },
        {
        }
    },

    {
        { "Encrypt", "Encryption algorithm unit tests." },
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
            string logFile = args[i];
            continue;
        }
    }
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
    bool bResultsToLog      = false;
    FILE* pFile;
    const uint64_t bufSize  = 512;

    pFile = fopen(args.logFile.c_str(), "w");

    if (pFile == nullptr)
        throw invalid_argument("Failed to write Handshake test log file " + args.logFile);
    else
        bResultsToLog = true;

    for (uint64_t i = 0; i < args.testIDs.size(); i++)
    {
        if (args.testIDs[i] > testGroups.size())
            throw invalid_argument("Invalid test group ID encountered running Handshake tests.");

        if (bResultsToLog)
        {
            char msgBuf[bufSize];
            sprintf(msgBuf, "Beginning %s test group.\n", testGroups[i].first.first.c_str());
            fwrite(msgBuf, sizeof(char), bufSize, pFile);
        }
        {
            printf("Beginning %s test group.\n", testGroups[i].first.first.c_str());
        }

        for (uint64_t j = 0; j < testGroups[i].second.size(); j++)
        {
            pfnTestFunc pfnTest = testGroups[i].second[j].second;

            try
            {
                TestResult res = pfnTest();

                for (uint64_t k = 0; k < res.caseResults.size(); k++)
                {
                    if (bResultsToLog)
                    {
                        char msgBuf[bufSize];
                        sprintf(msgBuf, "Case %llu %s %s\n", k, ResultCodeStrings[res.caseResults[k].first].c_str(),
                            res.caseResults[k].second.c_str());

                        fwrite(msgBuf, sizeof(char), bufSize, pFile);
                    }
                    else
                    {
                        printf("Case %llu %s %s\n", k, ResultCodeStrings[res.caseResults[k].first].c_str(),
                            res.caseResults[k].second.c_str());
                    }
                }
            }
            catch (exception& e)
            {
                
            }
        }
    }
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
    uint64_t vals[3] = { 1, 0, 0 };

    for (int64_t t = 0; t < 24; t++)
    {
        int64_t t0  = vals[1];
        int64_t t1  = (2 * vals[0] + 3 * vals[1]) % 5;
        int64_t t2  = ((t + 1) * (t + 2)) / 2;

        printf("[%lld %lld %lld]\n", vals[0], vals[1], t2);

        vals[0]     = t0;
        vals[1]     = t1;
        vals[2]     = t2;
    }

    return 0;

    vector<string> args(argv + 1, argv + argc);
    TestArgs argsOut;

    ParseCommandLine(args, argsOut);   
    RunTests(argsOut);

    return 0;
}