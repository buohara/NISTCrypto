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
    TestResult res;
    SHA3 sha3;

    for (uint64_t i = 0; i < numCases; i++)
    {
        sha3.ClearState();

        const uint8_t col1      = rand() % 0x20;
        const uint8_t col2      = rand() % 0x20;
        const uint8_t arrayVal  = rand() % 2;

        const uint8_t xRand     = rand() % STATE_W;
        const uint8_t yRand     = rand() % STATE_H;
        const uint8_t zRand     = rand() % sha3.params.w;

        if (arrayVal)
            sha3.SetBit(xRand, yRand, zRand);

        sha3.SetColumn(xRand == 0 ? STATE_W - 1 : xRand - 1, zRand, col1);
        sha3.SetColumn(xRand == STATE_W - 1 ? 0 : xRand + 1, zRand == 0 ? sha3.params.w - 1 : zRand - 1, col2);
        sha3.Theta();

        uint8_t p1                  = Parity(col1);
        uint8_t p2                  = Parity(col2);
        uint8_t parityExp           = arrayVal ^ p1 ^ p2;

        uint8_t thetaOut = sha3.GetBit(xRand, yRand, zRand);

        if (parityExp != thetaOut)
        {
            char msg[256];

            sprintf(
                msg,
                "SHA3 Theta parity transformation check failed at (%lu, %lu, %lu). Expected = %lu, actual = %lu.",
                xRand,
                yRand,
                zRand,
                parityExp,
                thetaOut
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
 * TestSHA3Rho - Test Rho Theta step function with known inputs
 * and outputs.
 *
 * @return  Pass if known transformation matches, fail otherwise.
 */

TestResult TestSHA3Rho()
{
    TestResult res;
    SHA3 sha3;

    const uint64_t laneOffsets[5][5] =
    {
        0,    1, 190,  28,  91,
        36, 300,   6,  55, 276,
        3,   10, 171, 153, 231,
        105, 45,  15,  21, 136,
        210, 66, 253, 120,  78
    };

    for (uint64_t i = 0; i < numCases; i++)
    {
        sha3.ClearState();

        const uint8_t xRand = rand() % STATE_W;
        const uint8_t yRand = rand() % STATE_H;
        const uint8_t zRand = rand() % STATE_L;

        sha3.SetBit(xRand, yRand, zRand);
        sha3.Rho();

        uint64_t laneOffset = laneOffsets[xRand][yRand];
        uint8_t zExp        = (zRand + laneOffset) % (STATE_L);
        uint8_t out         = sha3.GetBit(xRand, yRand, zExp);

        if (!out)
        {
            char msg[256];

            sprintf(
                msg,
                "SHA3 Rho lane offset transformation check failed at (%lu, %lu, %lu)." \
                    "Expected 1 at z = % lu.",
                xRand,
                yRand,
                zRand,
                zExp
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
 * TestSHA3Pi - Test SHA3 Pi step function with known inputs
 * and outputs.
 *
 * @return  Pass if known transformation matches, fail otherwise.
 */

TestResult TestSHA3Pi()
{
    TestResult res;
    SHA3 sha3;

    for (uint64_t i = 0; i < numCases; i++)
    {
        sha3.ClearState();

        const uint8_t xRand = rand() % STATE_W;
        const uint8_t yRand = rand() % STATE_H;

        vector<uint8_t> laneVal(sha3.params.w / 8, 0xA5);
        sha3.SetLane(xRand, yRand, laneVal);
        sha3.Pi();

        uint8_t xExp        = (xRand + 3 * yRand) % STATE_W;
        uint8_t yExp        = xRand;

        vector<uint8_t> laneAct;
        sha3.GetLane(xExp, yExp, laneAct);

        if (memcmp(&laneVal[0], &laneAct[0], laneVal.size()))
        {
            char msg[256];

            sprintf(
                msg,
                "SHA3 Pi lane rotation transformation check failed at (%lu, %lu). Expected" \
                    "0xA5A5A5A5A5A5A5A5 at (%lu, %lu).",
                xRand,
                yRand,
                xExp,
                yExp
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
 * TestSHA3Chi - Test SHA3 Chi step function with known inputs
 * and outputs.
 *
 * @return  Pass if known transformation matches, fail otherwise.
 */

TestResult TestSHA3Chi()
{
    TestResult res;
    SHA3 sha3;

    for (uint64_t i = 0; i < numCases; i++)
    {
        sha3.ClearState();

        const uint8_t yRand     = rand() % STATE_W;
        const uint8_t zRand     = rand() % STATE_H;
        const uint8_t randRow   = rand() % 0x20;

        sha3.SetRow(yRand, zRand, randRow);
        sha3.Chi();

        uint8_t rowAct          = sha3.GetRow(yRand, zRand);
        uint8_t rowExp          = 0;

        for (uint64_t j = 0; j < STATE_W; j++)
        {
            uint8_t s0  = j;
            uint8_t s1  = (j + 1) % STATE_W;
            uint8_t s2  = (j + 2) % STATE_W;

            uint8_t b0  = (randRow & (1 << s0)) >> s0;
            uint8_t b1  = (randRow & (1 << s1)) >> s1;
            uint8_t b2  = (randRow & (1 << s2)) >> s2;

            uint8_t bit  = (b0 ^ b1) * b2;
            rowExp      |= (bit << j);
        }

        if (rowExp != rowAct)
        {
            char msg[256];

            sprintf(
                msg,
                "SHA3 Chi row transformation check failed at (%lu, %lu). Expected = " \
                "%lu, actual = %lu.",
                yRand,
                zRand,
                rowAct,
                rowExp
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
 * RC - Round constant (RC) function used in Iota step transformation.
 *
 * @param t    [in] Input parameter for RC.
 *
 * @return RC(t).
 */

static uint8_t RC(const uint64_t t)
{
    if (!(t % 255))
        return 1;

    vector<uint8_t> r = { 1, 0, 0, 0, 0, 0, 0, 0 };

    for (uint64_t i = 0; i < t % 255; i++)
    {
        r.insert(r.begin(), 0);

        r[0] = r[0] ^ r[8];
        r[4] = r[4] ^ r[8];
        r[5] = r[5] ^ r[8];
        r[6] = r[6] ^ r[8];

        r.resize(8);
    }

    return r[0];
}

/**
 * TestSHA3Iota - Test SHA3 Iota step function with known inputs
 * and outputs.
 *
 * @return  Pass if known transformation matches, fail otherwise.
 */

TestResult TestSHA3Iota()
{
    TestResult res;
    SHA3 sha3;

    for (uint64_t i = 0; i < numCases; i++)
    {
        sha3.ClearState();

        const uint8_t indices[7]    = { 0, 1, 3, 7, 15, 31, 63 };
        uint8_t rcs[64]             = { 0 };
        const uint8_t round         = rand() % NUM_SHA3_ROUNDS;

        vector<uint8_t> lane(8, 0);

        for (uint64_t j = 0; j < lane.size(); j++)
            lane[j] = rand() % 256;

        for (uint64_t j = 0; j < 7; j++)
            rcs[indices[j]] = RC(round);

        vector<uint8_t> laneExp(8, 0);

        for (uint64_t j = 0; j < sha3.params.w; j++)
        {
            uint8_t byte    = j / 8;
            uint8_t bit     = j % 8;
            laneExp[byte]   |= ((lane[byte] & (1 << bit)) ^ rcs[j]) << bit;
        }

        sha3.Iota(round);
        vector<uint8_t> laneAct(8, 0);
        sha3.GetLane(0, 0, laneAct);

        if (memcmp(&laneExp[0], &laneAct[0], laneExp.size()))
        {
            char msg[256];
            sprintf(msg, "SHA3 Iota RC transformation check failed.");
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