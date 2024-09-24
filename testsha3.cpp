#include "test.h"
#include "hash.h"

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
    vector<vector<uint8_t>> hashes;

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
    if (!(vecs.msgs.size() == 0 && vecs.hashes.size() == 0))
        throw invalid_argument("Expected test vector output list to be empty when loading SHA3 vectors from file.");

    FILE* pFile = fopen(file.c_str(), "r");

    if (pFile == nullptr)
        throw invalid_argument("Invalid file path given when loading SHA3 test vectors.");

    string patternMsg       = "^Msg = ([a-fA-F0-9]+)$";
    string patternMD        = "^MD = ([a-fA-F0-9]+)$";
    string patternSeed      = "^Seed = ([a-fA-F0-9]+)$";
    string patternCount     = "^COUNT = ([a-fA-F0-9]+)$";

    regex reMsg(patternMsg);
    regex reMD(patternMD);
    regex reSeed(patternSeed);
    regex reCount(patternCount);

    char buf[512];
    smatch match;

    while (fgets(buf, sizeof(buf), pFile) != NULL)
    {
        string line = string(buf);

        if (regex_search(line, match, reMsg))
        {
            vecs.mode = MSG;
            vector<uint8_t> msg;

            StringToHexArray(match[1], msg);
            vecs.msgs.push_back(msg);
            continue;
        }

        if (regex_search(line, match, reMD))
        {
            vector<uint8_t> md;

            StringToHexArray(match[1], md);
            vecs.hashes.push_back(md);
            continue;
        }

        if (regex_search(line, match, reSeed))
        {
            vecs.mode = MONTE;
            vector<uint8_t> seed;

            StringToHexArray(match[1], seed);
            vecs.msgs.push_back(seed);
            continue;
        }

        if (regex_search(line, match, reCount))
        {
            continue;
        }
    }

    if (vecs.mode == MSG && (vecs.msgs.size() != vecs.hashes.size()))
        throw length_error("Mismatched number of messages and hashes loading SHA3 test vec file");
}

/**
 * TestSHA3Theta - Test SHA3 Theta step function with known inputs
 * and outputs.
 *
 * @return  Pass if known transformation matches, fail otherwise.
 */

TestResult TestSHA3Theta()
{
    SHA3 sha3;

    uint8_t col1    = rand() % 0x20;
    uint8_t col2    = rand() % 0x20;

    uint8_t xRand   = rand() % STATE_W;
    uint8_t yRand   = rand() % STATE_H;
    uint8_t zRand   = rand() % sha3.params.w;

    sha3.SetBit(xRand, yRand, zRand, 1);
    sha3.SetColumn(xRand == 0 ? STATE_W - 1 : xRand - 1, zRand, col1);
    sha3.SetColumn(xRand == STATE_W - 1 ? 0 : xRand + 1, zRand == 0 ? sha3.params.w - 1 : zRand - 1, col2);



    sha3.Theta();
}

/**
 * TestSHA3Rho - Test Rho Theta step function with known inputs
 * and outputs.
 *
 * @return  Pass if known transformation matches, fail otherwise.
 */

TestResult TestSHA3Rho()
{

}

/**
 * TestSHA3Pi - Test SHA3 Pi step function with known inputs
 * and outputs.
 *
 * @return  Pass if known transformation matches, fail otherwise.
 */

TestResult TestSHA3Pi()
{

}

/**
 * TestSHA3Chi - Test SHA3 Chi step function with known inputs
 * and outputs.
 *
 * @return  Pass if known transformation matches, fail otherwise.
 */

TestResult TestSHA3Chi()
{

}

/**
 * TestSHA3Iota - Test SHA3 Iota step function with known inputs
 * and outputs.
 *
 * @return  Pass if known transformation matches, fail otherwise.
 */

TestResult TestSHA3Iota()
{

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
        SHA3 sha3;
        vector<uint8_t> hashOut;
        sha3.Hash(vecs.msgs[i], hashOut);

        if (hashOut.size() != vecs.hashes[i][0] ||
            (memcmp(&hashOut[0], &vecs.hashes[i][0], hashOut.size()) != 0)) 
        {
            char msg[256];

            string hashOutStr;
            HexArrayToString(hashOut, hashOutStr);

            string hashExpStr;
            HexArrayToString(vecs.hashes[i], hashExpStr);

            sprintf(
                msg,
                "SHA3-512 long message test failed. Expected hash = %s, actual hash = %s.",
                hashExpStr.c_str(),
                hashOutStr.c_str()
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
        SHA3 sha3;
        vector<uint8_t> hashOut;
        sha3.Hash(vecs.msgs[i], hashOut);

        if (hashOut.size() != vecs.hashes[i][0] ||
            (memcmp(&hashOut[0], &vecs.hashes[i][0], hashOut.size()) != 0))
        {
            char msg[256];

            string hashOutStr;
            HexArrayToString(hashOut, hashOutStr);

            string hashExpStr;
            HexArrayToString(vecs.hashes[i], hashExpStr);

            sprintf(
                msg,
                "SHA3-512 short message test failed. Expected hash = %s, actual hash = %s.",
                hashExpStr.c_str(),
                hashOutStr.c_str()
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

    SHA3 sha3;
    vector<uint8_t> msg = vecs.msgs[0];

    for (uint64_t i = 0; i < vecs.hashes.size(); i++)
    {
        vector<uint8_t> hashOut;
        sha3.Hash(msg, hashOut);

        if (hashOut.size() != vecs.hashes[i][0] ||
            (memcmp(&hashOut[0], &vecs.hashes[i][0], hashOut.size()) != 0))
        {
            char msg[256];

            string hashOutStr;
            HexArrayToString(hashOut, hashOutStr);

            string hashExpStr;
            HexArrayToString(vecs.hashes[i], hashExpStr);

            sprintf(
                msg,
                "SHA3-512 Monte test failed. Expected hash = %s, actual hash = %s.",
                hashExpStr.c_str(),
                hashOutStr.c_str()
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