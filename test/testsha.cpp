#include "test.h"
#include "sha.h"

const uint64_t numCases = 100;

enum SHA_TEST_MODE
{
    MSG,
    MONTE,
    NUM_MODES
};

struct SHATestVecs
{
    SHA_TEST_MODE mode;

    vector<vector<uint8_t>> msgs;
    vector<vector<uint8_t>> mds;

    SHATestVecs() : mode(MSG) {}
};

/**
 * LoadTestVecsFromFile - Load known SHA3 messages and hash values
 * from a file.
 *
 * @param file  [in]        Path to test vector file.
 * @param vects [in/out]    List of test vecs to populate.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

static void LoadTestVecsFromFile(const string file, SHATestVecs& vecs)
{
    assert(vecs.msgs.size() == 0 && vecs.mds.size() == 0);

    FILE* pFile = fopen(file.c_str(), "r");
    assert(pFile != nullptr);

    string patternMsg   = "^Msg = ([a-fA-F0-9]+)$";
    string patternMD    = "^MD = ([a-fA-F0-9]+)$";
    string patternSeed  = "^Seed = ([a-fA-F0-9]+)$";
    string patternCount = "^COUNT = ([a-fA-F0-9]+)$";
    string patternLen   = "^Len = ([0-9]+)$";

    regex reMsg(patternMsg);
    regex reMD(patternMD);
    regex reSeed(patternSeed);
    regex reCount(patternCount);
    regex reLen(patternLen);

    char buf[32768];
    smatch match;
    uint64_t len;

    while (fgets(buf, sizeof(buf), pFile) != NULL)
    {
        string line = string(buf);

        if (regex_search(line, match, reMsg))
        {
            vecs.mode = MSG;
            vector<uint8_t> msg;
            StringToHexArray(match[1], msg, false);

            if (msg.size() != len / 8)
            {
                while (msg.size() && msg[0] == 0)
                    msg.erase(msg.begin());
            }

            assert(msg.size() == len / 8);
            len = 0;

            vecs.msgs.push_back(msg);
            continue;
        }

        if (regex_search(line, match, reLen))
        {
            len = stoull(match[1]);
            continue;
        }

        if (regex_search(line, match, reMD))
        {
            vector<uint8_t> md;

            StringToHexArray(match[1], md, false);
            vecs.mds.push_back(md);
            continue;
        }

        if (regex_search(line, match, reSeed))
        {
            vecs.mode = MONTE;
            vector<uint8_t> seed;

            StringToHexArray(match[1], seed, false);
            vecs.msgs.push_back(seed);
            continue;
        }

        if (regex_search(line, match, reCount))
        {
            continue;
        }
    }

    if (vecs.mode == MSG)
        assert(vecs.msgs.size() == vecs.mds.size());
}

/**
 * RunSHAMessageTest - Load test vectors and MDs from a test file. Hash the
 * test vectors and compare against expected.
 *
 * @param testVecFile   [in] Test vec file.
 *
 * @return List of test results.
 */

static TestResult RunSHAMessageTest(string testVecFile, SHASize sz)
{
    TestResult res;
    SHATestVecs vecs;

    LoadTestVecsFromFile(testVecFile, vecs);

    for (uint64_t i = 0; i < vecs.msgs.size(); i++)
    {
        SHA2 sha;
        vector<uint8_t> mdOut;
        sha.Hash(sz, vecs.msgs[i], mdOut);

        if (mdOut.size() != vecs.mds[i].size() ||
            (memcmp(&mdOut[0], &vecs.mds[i][0], mdOut.size()) != 0))
        {
            char msg[256];
            assert(false);

            string mdOutStr;
            HexArrayToString(mdOut, mdOutStr, false);

            string hashExpStr;
            HexArrayToString(vecs.mds[i], hashExpStr, false);

            sprintf(
                msg,
                "SHA3 message test failed. Input file = %s, test ID = %llu",
                testVecFile.c_str(),
                i
            );

            res.caseResults.push_back({ FAIL, string(msg) });
        }
        else
        {
            res.caseResults.push_back({ PASS, "" });
        }
    }

    return res;
}

/**
 * RunSHA3Monte - Load test vectors and MDs from a test file. Hash the
 * test vectors and compare against expected.
 *
 * @param testVecFile   [in] Test vec file.
 *
 * @return List of test results.
 */

static TestResult RunSHAMonte(string testVecFile, SHASize sz)
{
    assert(sz == SHA256 || sz == SHA512);

    TestResult res;
    SHATestVecs vecs;

    LoadTestVecsFromFile(testVecFile, vecs);

    SHA2 sha;
    vector<uint8_t> msg = vecs.msgs[0];
    vector<uint8_t> mdOut;

    for (uint64_t i = 0; i < vecs.mds.size(); i++)
    {
        mdOut.resize(0);

        for (uint64_t j = 0; j < 1000; j++)
        {
            sha.Hash(sz, vecs.msgs[i], mdOut);
            msg = mdOut;
            mdOut.resize(0);
        }

        mdOut = msg;

        if (mdOut.size() != vecs.mds[i].size() ||
            (memcmp(&mdOut[0], &vecs.mds[i][0], mdOut.size()) != 0))
        {
            assert(false);
            char msg[256];

            string mdOutStr;
            HexArrayToString(mdOut, mdOutStr);

            string hashExpStr;
            HexArrayToString(vecs.mds[i], hashExpStr);

            sprintf(
                msg,
                "SHA3 message test failed. Input file = %s, test ID = %llu",
                testVecFile.c_str(),
                i
            );

            res.caseResults.push_back({ FAIL, string(msg) });
        }
        else
        {
            res.caseResults.push_back({ PASS, "" });
        }
    }

    return res;
}

/**
 * RunSHA3MessageTest - Load test vectors and MDs from a test file. Hash the
 * test vectors and compare against expected.
 * 
 * @param testVecFile   [in] Test vec file.
 *
 * @return List of test results.
 */

static TestResult RunSHA3MessageTest(string testVecFile, SHASize sz)
{
    TestResult res;
    SHATestVecs vecs;

    LoadTestVecsFromFile(testVecFile, vecs);

    for (uint64_t i = 0; i < vecs.msgs.size(); i++)
    {
        SHA3 sha3(sz);
        vector<uint8_t> mdOut;
        sha3.Hash(vecs.msgs[i], mdOut);

        if (mdOut.size() != vecs.mds[i].size() ||
            (memcmp(&mdOut[0], &vecs.mds[i][0], mdOut.size()) != 0))
        {
            char msg[256];
            assert(false);

            string mdOutStr;
            HexArrayToString(mdOut, mdOutStr, false);

            string hashExpStr;
            HexArrayToString(vecs.mds[i], hashExpStr, false);

            sprintf(
                msg,
                "SHA3 message test failed. Input file = %s, test ID = %llu",
                testVecFile.c_str(),
                i
            );

            res.caseResults.push_back({ FAIL, string(msg) });
        }
        else
        {
            res.caseResults.push_back({ PASS, "" });
        }
    }

    return res;
}

/**
 * RunSHA3Monte - Load test vectors and MDs from a test file. Hash the
 * test vectors and compare against expected.
 *
 * @param testVecFile   [in] Test vec file.
 *
 * @return List of test results.
 */

static TestResult RunSHA3Monte(string testVecFile, SHASize sz)
{
    TestResult res;
    SHATestVecs vecs;

    LoadTestVecsFromFile(testVecFile, vecs);

    SHA3 sha3(sz);
    vector<uint8_t> msg = vecs.msgs[0];
    vector<uint8_t> mdOut;

    for (uint64_t i = 0; i < vecs.mds.size(); i++)
    {
        mdOut.resize(0);

        for (uint64_t j = 0; j < 1000; j++)
        {
            sha3.Hash(msg, mdOut);
            msg = mdOut;
            mdOut.resize(0);
        }

        mdOut = msg;

        if (mdOut.size() != vecs.mds[i].size() ||
            (memcmp(&mdOut[0], &vecs.mds[i][0], mdOut.size()) != 0))
        {
            char msg[256];
            assert(false);

            string mdOutStr;
            HexArrayToString(mdOut, mdOutStr);

            string hashExpStr;
            HexArrayToString(vecs.mds[i], hashExpStr);

            sprintf(
                msg,
                "SHA3 message test failed. Input file = %s, test ID = %llu",
                testVecFile.c_str(),
                i
            );

            res.caseResults.push_back({ FAIL, string(msg) });
        }
        else
        {
            res.caseResults.push_back({ PASS, "" });
        }
    }

    return res;
}

/**
 * TestSHA224Short - Compute a SHA224 hash for a short input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA224Short()
{
    SHATestVecs vecs;

    TestResult res = RunSHAMessageTest(
        "test/shabytetestvectors/SHA224ShortMsg.rsp",
        SHA224
    );

    return res;
}

/**
 * TestSHA256Short - Compute a SHA256 hash for a short input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA256Short()
{
    SHATestVecs vecs;

    TestResult res = RunSHAMessageTest(
        "test/shabytetestvectors/SHA256ShortMsg.rsp",
        SHA256
    );

    return res;
}

/**
 * TestSHA384Short - Compute a SHA384 hash for a short input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA384Short()
{
    SHATestVecs vecs;

    TestResult res = RunSHAMessageTest(
        "test/shabytetestvectors/SHA384ShortMsg.rsp",
        SHA384
    );

    return res;
}

/**
 * TestSHA512Short - Compute a SHA512 hash for a short input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA512Short()
{
    SHATestVecs vecs;

    TestResult res = RunSHAMessageTest(
        "test/shabytetestvectors/SHA512ShortMsg.rsp",
        SHA512
    );

    return res;
}

/**
 * TestSHA224Long - Compute a SHA224 hash for a long input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA224Long()
{
    SHATestVecs vecs;

    TestResult res = RunSHAMessageTest(
        "test/shabytetestvectors/SHA224LongMsg.rsp",
        SHA224
    );

    return res;
}

/**
 * TestSHA256Long - Compute a SHA256 hash for a long input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA256Long()
{
    SHATestVecs vecs;

    TestResult res = RunSHAMessageTest(
        "test/shabytetestvectors/SHA256LongMsg.rsp",
        SHA256
    );

    return res;
}

/**
 * TestSHA384Long - Compute a SHA384 hash for a long input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA384Long()
{
    SHATestVecs vecs;

    TestResult res = RunSHAMessageTest(
        "test/shabytetestvectors/SHA384LongMsg.rsp",
        SHA384
    );

    return res;
}

/**
 * TestSHA512Short - Compute a SHA512 hash for a long input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA512Long()
{
    SHATestVecs vecs;

    TestResult res = RunSHAMessageTest(
        "test/shabytetestvectors/SHA512LongMsg.rsp",
        SHA512
    );

    return res;
}

/**
 * TestSHA3224Short - Compute a SHA3 hash for a short input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA3224Short()
{
    SHATestVecs vecs;

    TestResult res = RunSHA3MessageTest(
        "test/sha-3bytetestvectors/SHA3_224ShortMsg.rsp",
        SHA224
    );

    return res;
}

/**
 * TestSHA3224Long - Compute a SHA3 hash for a long input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA3224Long()
{
    SHATestVecs vecs;

    TestResult res = RunSHA3MessageTest(
        "test/sha-3bytetestvectors/SHA3_224LongMsg.rsp",
        SHA224
    );

    return res;
}

/**
 * TestSHA3224Monte - Iteratively compute SHA3 hashes starting from an initial seed
 * and repeating for 100 rounds.
 *
 * @return  Pass if hashes match, fail otherwise.
 */

TestResult TestSHA3224Monte()
{
    SHATestVecs vecs;

    TestResult res = RunSHA3Monte(
        "test/sha-3bytetestvectors/SHA3_224Monte.rsp",
        SHA224
    );

    return res;
}

/**
 * TestSHA3256Short - Compute a SHA3 hash for a short input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA3256Short()
{
    SHATestVecs vecs;

    TestResult res = RunSHA3MessageTest(
        "test/sha-3bytetestvectors/SHA3_256ShortMsg.rsp",
        SHA256
    );

    return res;
}

/**
 * TestSHA3256Long - Compute a SHA3 hash for a long input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA3256Long()
{
    SHATestVecs vecs;

    TestResult res = RunSHA3MessageTest(
        "test/sha-3bytetestvectors/SHA3_256LongMsg.rsp",
        SHA256
    );

    return res;
}

/**
 * TestSHA3256Monte - Iteratively compute SHA3 hashes starting from an initial seed
 * and repeating for 100 rounds.
 *
 * @return  Pass if hashes match, fail otherwise.
 */

TestResult TestSHA3256Monte()
{
    SHATestVecs vecs;

    TestResult res = RunSHA3Monte(
        "test/sha-3bytetestvectors/SHA3_256Monte.rsp",
        SHA256
    );

    return res;
}

/**
 * TestSHA3384Short - Compute a SHA3 hash for a short input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA3384Short()
{
    SHATestVecs vecs;

    TestResult res = RunSHA3MessageTest(
        "test/sha-3bytetestvectors/SHA3_384ShortMsg.rsp",
        SHA384
    );

    return res;
}

/**
 * TestSHA3384Long - Compute a SHA3 hash for a long input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA3384Long()
{
    SHATestVecs vecs;

    TestResult res = RunSHA3MessageTest(
        "test/sha-3bytetestvectors/SHA3_384LongMsg.rsp",
        SHA384
    );

    return res;
}

/**
 * TestSHA3384Monte - Iteratively compute SHA3 hashes starting from an initial seed
 * and repeating for 100 rounds.
 *
 * @return  Pass if hashes match, fail otherwise.
 */

TestResult TestSHA3384Monte()
{
    SHATestVecs vecs;

    TestResult res = RunSHA3Monte(
        "test/sha-3bytetestvectors/SHA3_384Monte.rsp",
        SHA384
    );

    return res;
}

/**
 * TestSHA3512Short - Compute a SHA3 hash for a short input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA3512Short()
{
    SHATestVecs vecs;

    TestResult res = RunSHA3MessageTest(
        "test/sha-3bytetestvectors/SHA3_512ShortMsg.rsp",
        SHA512);

    return res;
}

/**
 * TestSHA3512Long - Compute a SHA3 hash for a long input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA3512Long()
{
    SHATestVecs vecs;

    TestResult res = RunSHA3MessageTest(
        "test/sha-3bytetestvectors/SHA3_512LongMsg.rsp",
        SHA512);

    return res;
}

/**
 * TestSHA3512Monte - Iteratively compute SHA3 hashes starting from an initial seed
 * and repeating for 100 rounds.
 *
 * @return  Pass if hashes match, fail otherwise.
 */

TestResult TestSHA3512Monte()
{
    SHATestVecs vecs;

    TestResult res = RunSHA3Monte(
        "test/sha-3bytetestvectors/SHA3_512Monte.rsp",
        SHA512);

    return res;
}