#include "test.h"
#include "hash.h"

const uint64_t numCases = 100;

enum SHA3_TEST_MODE
{
    MSG,
    MONTE,
    NUM_MODES
};

struct SHA3TestVecs
{
    SHA3_TEST_MODE mode;

    vector<vector<uint8_t>> msgs;
    vector<vector<uint8_t>> mds;

    SHA3TestVecs() : mode(MSG) {}
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

void LoadTestVecsFromFile(const string file, SHA3TestVecs &vecs)
{
    assert(vecs.msgs.size() == 0 && vecs.mds.size() == 0);
    
    FILE* pFile = fopen(file.c_str(), "r");
    assert(pFile != nullptr);

    string patternMsg       = "^Msg = ([a-fA-F0-9]+)$";
    string patternMD        = "^MD = ([a-fA-F0-9]+)$";
    string patternSeed      = "^Seed = ([a-fA-F0-9]+)$";
    string patternCount     = "^COUNT = ([a-fA-F0-9]+)$";

    regex reMsg(patternMsg);
    regex reMD(patternMD);
    regex reSeed(patternSeed);
    regex reCount(patternCount);

    char buf[32768];
    smatch match;

    while (fgets(buf, sizeof(buf), pFile) != NULL)
    {
        string line = string(buf);

        if (regex_search(line, match, reMsg))
        {
            vecs.mode = MSG;
            vector<uint8_t> msg;
            StringToHexArray(match[1], msg, false);

            while (msg.size() && msg[0] == 0)
                msg.erase(msg.begin());

            vecs.msgs.push_back(msg);
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
 * TestSHA3512Long - Compute a SHA3 hash for a long input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA3512Long()
{
    TestResult res;
    SHA3TestVecs vecs;

    LoadTestVecsFromFile("test/sha-3bytetestvectors/SHA3_512LongMsg.rsp", vecs);

    for (uint64_t i = 0; i < vecs.msgs.size(); i++)
    {
        SHA3 sha3(SHA512);
        vector<uint8_t> mdOut;
        sha3.Hash(vecs.msgs[i], mdOut);

        bool bSameSize  = (mdOut.size() == vecs.mds[i].size());
        uint64_t mem    = memcmp(&mdOut[0], &vecs.mds[i][0], mdOut.size());

        if (mdOut.size() != vecs.mds[i].size() ||
            (memcmp(&mdOut[0], &vecs.mds[i][0], mdOut.size()) != 0))
        {
            char msg[256];

            string mdOutStr;
            HexArrayToString(mdOut, mdOutStr, false);

            string hashExpStr;
            HexArrayToString(vecs.mds[i], hashExpStr, false);

            sprintf(
                msg,
                "SHA3-512 long message test failed."
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
 * TestSHA3512Short - Compute a SHA3 hash for a short input and compare
 * against known value.
 *
 * @return  Pass if hash matches, fail otherwise.
 */

TestResult TestSHA3512Short()
{
    TestResult res;
    SHA3TestVecs vecs;

    LoadTestVecsFromFile("test/sha-3bytetestvectors/SHA3_512ShortMsg.rsp", vecs);

    for (uint64_t i = 0; i < vecs.msgs.size(); i++)
    {
        SHA3 sha3(SHA512);
        vector<uint8_t> mdOut;
        sha3.Hash(vecs.msgs[i], mdOut);

        if (mdOut.size() != vecs.mds[i].size() ||
            (memcmp(&mdOut[0], &vecs.mds[i][0], mdOut.size()) != 0))
        {
            char msg[256];

            string mdOutStr;
            HexArrayToString(mdOut, mdOutStr);

            string hashExpStr;
            HexArrayToString(vecs.mds[i], hashExpStr);

            sprintf(
                msg,
                "SHA3-512 short message test failed."
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
 * TestSHA3512Monte - Iteratively compute SHA3 hashes starting from an initial seed
 * and repeating for 100 rounds.
 *
 * @return  Pass if hashes match, fail otherwise.
 */

TestResult TestSHA3512Monte()
{
    TestResult res;
    SHA3TestVecs vecs;

    LoadTestVecsFromFile("test/sha-3bytetestvectors/SHA3_512Monte.rsp", vecs);

    SHA3 sha3(SHA512);
    vector<uint8_t> msg = vecs.msgs[0];
    vector<uint8_t> mdOut;

    for (uint64_t i = 0; i < vecs.mds.size(); i++)
    {
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

            string mdOutStr;
            HexArrayToString(mdOut, mdOutStr);

            string hashExpStr;
            HexArrayToString(vecs.mds[i], hashExpStr);

            sprintf(
                msg,
                "SHA3-512 Monte test failed."
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