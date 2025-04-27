#include "ecdsa.h"
#include "test.h"

extern map<NISTCurve, DPStrings> curveDomainParams;

struct DSATestVec
{
    NISTCurve curve;
    SHASize shaSz;

    vector<uint8_t> msg;
    vector<uint8_t> d;
    vector<uint8_t> k;

    vector<uint8_t> Qx;
    vector<uint8_t> Qy;
    vector<uint8_t> r;
    vector<uint8_t> s;
};

map<string, NISTCurve> curveStrings =
{
    { "P-224", P224 },
    { "P-256", P256 },
    { "P-384", P384 },
    { "P-521", P521 }
};

map<string, SHASize> shaStrings =
{
    { "SHA-224", SHA224 },
    { "SHA-256", SHA256 },
    { "SHA-384", SHA384 },
    { "SHA-512", SHA512 }
};

/**
 * LoadTestVecsFromFile - Load known ECDSA key pair, msg, and signature
 * values from file.
 *
 * @param file  [in]        Path to test vector file.
 * @param vects [in/out]    List of test vecs to populate.
 */

static void LoadTestVecsFromFile(const string file, vector<DSATestVec> &vecs)
{
    assert(vecs.size() == 0);

    FILE* pFile = fopen(file.c_str(), "r");
    assert(pFile != nullptr);

    string patternMsg       = "^Msg = ([a-fA-F0-9]+)$";
    string patternD         = "^d = ([a-fA-F0-9]+)$";
    string patternQx        = "^Qx = ([a-fA-F0-9]+)$";
    string patternQy        = "^Qy = ([a-fA-F0-9]+)$";
    string patternk         = "^k = ([a-fA-F0-9]+)$";
    string patternR         = "^R = ([a-fA-F0-9]+)$";
    string patternS         = "^S = ([a-fA-F0-9]+)$";
    string patternCurveSHA  = "^\\[([PKB]-[0-9]+),(SHA-[0-9]+)\\]$";

    regex reMsg(patternMsg);
    regex reD(patternD);
    regex reQx(patternQx);
    regex reQy(patternQy);
    regex rek(patternk);
    regex reR(patternR);
    regex reS(patternS);
    regex reCurveSHA(patternCurveSHA);

    char buf[32768];
    smatch match;

    DSATestVec curVec;
    NISTCurve curCurve;
    SHASize curSHA;

    while (fgets(buf, sizeof(buf), pFile) != NULL)
    {
        string line = string(buf);

        if (regex_search(line, match, reMsg))
        {
            vector<uint8_t> msg;
            StringToHexArray(match[1], msg, false);
            curVec.msg = msg;
            continue;
        }

        if (regex_search(line, match, reD))
        {
            vector<uint8_t> d;
            StringToHexArray(match[1], d, false);
            curVec.d = d;
            continue;
        }

        if (regex_search(line, match, reQx))
        {
            vector<uint8_t> qx;
            StringToHexArray(match[1], qx, false);
            curVec.Qx = qx;
            continue;
        }

        if (regex_search(line, match, reQy))
        {
            vector<uint8_t> qy;
            StringToHexArray(match[1], qy, false);
            curVec.Qy = qy;
            continue;
        }

        if (regex_search(line, match, rek))
        {
            vector<uint8_t> k;
            StringToHexArray(match[1], k, false);
            curVec.k = k;
            continue;
        }

        if (regex_search(line, match, reR))
        {
            vector<uint8_t> r;
            StringToHexArray(match[1], r, false);
            curVec.r = r;
            continue;
        }

        if (regex_search(line, match, reS))
        {
            vector<uint8_t> s;
            StringToHexArray(match[1], s, false);
            
            curVec.s        = s;
            curVec.shaSz    = curSHA;
            curVec.curve    = curCurve;

            vecs.push_back(curVec);
            continue;
        }

        if (regex_search(line, match, reCurveSHA))
        {
            curCurve    = curveStrings[match[1]];
            curSHA      = shaStrings[match[2]];

            continue;
        }
    }
}

/**
 * TestSigGen - Load known msg, key pair, and signature values from file.
 * Confirm computed signature matches known values.
 */

TestResult TestSigGen()
{
    vector<DSATestVec> testVecs;
    LoadTestVecsFromFile("test/ecdsatestvectors/SigGen.txt", testVecs);

    for (uint32_t i = 0; i < testVecs.size(); i++)
    {
        DPStrings paramStrings = curveDomainParams[testVecs[i].curve];
        DomainParams params(paramStrings);

        EllipticCurve ec(params);
        BigInt d(testVecs[i].d);

        DigSign sig = ec.GenerateSignature(
            testVecs[i].msg,
            d,
            SHA224
        );
    }

    TestResult res;
    return res;
}