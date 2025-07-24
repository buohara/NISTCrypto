#include "test.h"

static const uint64_t nSizes = 10;
static const uint64_t maxShift = 16;
static const uint64_t testIntBitSizes[nSizes] = { 7, 32, 41, 64, 103, 128, 211, 256, 388, 512 };
static const uint64_t numCasesPerSize = 10;

/**
 * ReverseString - Reverse a string. Helper function since BigInts store LSBs
 * in low indices, while integer strings store them in high indices.
 *
 * @param in    [in]        String to reverse.
 *
 * @return      Reversed string.
 */

static string ReverseString(const string& in)
{
    if (in == "")
        return string("");

    string out;
    const uint64_t len = in.length();
    out.resize(len);

    for (uint64_t i = 0; i < len; i++)
        out[len - i - 1] = in[i];

    return out;
}

/**
 * AddBinaryStrings - Add two binary strings and store sum in an output byte
 * array.
 *
 * @param u1    [in]        First addend string.
 * @param u2    [in]        Second addend string.
 * @param out   [in/out]    Output sum as a byte array.
 */

static void AddBinaryStrings(const string& u1, const string& u2, vector<uint8_t>& out)
{
    const uint64_t maxLen   = max(u1.length(), u2.length()) + 1;
    const uint64_t l1       = u1.length();
    const uint64_t l2       = u2.length();
    uint8_t carry           = 0;
    uint8_t bitCounter      = 0;
    uint8_t sum             = 0;

    const string u1Rev      = ReverseString(u1);
    const string u2Rev      = ReverseString(u2);

    for (uint64_t i = 0; i < maxLen; i++)
    {
        uint8_t d1  = (i < l1) ? (u1Rev[i] - '0') : 0;
        uint8_t d2  = (i < l2) ? (u2Rev[i] - '0') : 0;

        uint8_t dig = d1 ^ d2 ^ carry;
        carry       = (d1 & d2) | (carry & (d1 ^ d2));

        sum         |= (dig << (i % 8));
        bitCounter++;

        if (bitCounter == 8)
        {
            bitCounter = 0;
            out.push_back(sum);
            sum = 0;
        }
    }

    if (sum)
    {
        if (carry)
            sum |= (1 << bitCounter);

        out.push_back(sum);
        sum     = 0;
        carry   = 0;
    }

    if (carry)
        out.push_back(1);
}

/**
 * AddBinaryStrings - Add two binary strings and store sum in an
 * output string.
 *
 * @param u1    [in]        First addend string.
 * @param u2    [in]        Second addend string.
 * @param out   [in/out]    Output sum as string.
 */

static void AddBinaryStrings(const string& u1, const string& u2, string& out)
{
    const uint64_t maxLen   = max(u1.length(), u2.length());
    const uint64_t l1       = u1.length();
    const uint64_t l2       = u2.length();
    uint8_t carry           = 0;

    const string u1Rev      = ReverseString(u1);
    const string u2Rev      = ReverseString(u2);

    for (uint64_t i = 0; i < maxLen; i++)
    {
        uint8_t d1  = (i < l1) ? (u1Rev[i] - '0') : 0;
        uint8_t d2  = (i < l2) ? (u2Rev[i] - '0') : 0;

        uint8_t dig = d1 ^ d2 ^ carry;
        carry       = (d1 & d2) | (carry & (d1 ^ d2));

        out.insert(out.begin(), dig + '0');
    }

    if (carry)
        out.insert(out.begin(), '1');
}

/**
 * SubtractBinaryStrings - Subtract two binary strings and store sum in an output byte
 * array.
 *
 * @param u1        [in] Integer to be subtracted from.
 * @param u2        [in] Integer to subtract.
 * @param out       [in/out] Output difference as a byte array.
 */

static void SubtractBinaryStrings(const string& u1, const string& u2, vector<uint8_t>& out)
{
    if (out.size() != 0)
        throw invalid_argument("Expected output byte array to be empty in input when adding binary strings");

    if (u2.length() > u1.length() || (u1.length() == u2.length() && u1 <= u2))
    {
        out.push_back(0);
        return;
    }

    const uint64_t maxLen   = max(u1.length(), u2.length()) + 1;
    const uint64_t l1       = u1.length();
    const uint64_t l2       = u2.length();
    uint8_t bitCounter      = 0;
    uint8_t diff            = 0;
    uint8_t borrow          = 0;

    const string u1Rev      = ReverseString(u1);
    const string u2Rev      = ReverseString(u2);

    for (uint64_t i = 0; i < maxLen; i++)
    {
        uint8_t d1  = (i < l1) ? (u1Rev[i] - '0') : 0;
        uint8_t d2  = (i < l2) ? (u2Rev[i] - '0') : 0;
        uint8_t dig = 0;

        if (borrow && d1)
        {
            d1--;
            borrow = 0;
        }
        else if (borrow)
            d1 = 1;

        if (d1 == 1 && d2 == 0)
            dig = 1;

        if (d1 == 1 && d2 == 1)
            dig = 0;

        if (d1 == 0 && d2 == 0)
            dig = 0;

        if (d1 == 0 && d2 == 1)
        {
            dig = 1;
            borrow = 1;
        }

        bitCounter++;
        diff |= (dig << (i % 8));

        if (bitCounter == 8)
        {
            bitCounter = 0;
            out.push_back(diff);
            diff = 0;
        }
    }

    if (diff)
    {
        out.push_back(diff);
        diff = 0;
    }
}

/**
 * MultiplyBinaryStrings - Multiply two binary strings and store product in an output byte
 * array.
 *
 * @param u1        [in] First integer to multiply.
 * @param u2        [in] Second integer to multiply.
 * @param prod      [in/out] Output product as a byte array.
 */

static void MultiplyBinaryStrings(const string& u1, const string& u2, string& prod)
{
    string sum = "0";

    for (uint64_t i = u1.length(); i-- > 0;)
    {
        string out;

        if ((u1[i] - '0'))
        {
            string tmp = u2 + string(u1.length() - i - 1, '0');
            AddBinaryStrings(sum, tmp, out);
            sum = out;
        }
    }

    prod = sum;
}

/**
 * TestAssignBigInt - Test big integer assignment. Generate a random batch of big integers,
 * assign them to other big integers, test equality.
 *
 * @return  Pass if all cases match, fail otherwise.
 */

TestResult TestAssignBigInt()
{
    const uint64_t nCases = 100;
    TestResult res;

    for (uint64_t i = 0; i < nCases; i++)
    {
        uint64_t val = (uint64_t)rand();
        BigInt a(val);
        BigInt b = a;

        if (!(a == b))
        {
            char msg[256];
            assert(false);

            sprintf(
                msg,
                "BigInt assignment operator failed with a = %s, b = %s.",
                a.GetBinaryString().c_str(),
                b.GetBinaryString().c_str()
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
 * TestCmpLShiftBigIntCorrect - Test big integer left shift. Compare
 * against bit string manipulation version of shift.
 *
 * @return  Pass if shifts match expected patterns, fail otherwise.
 */

TestResult TestCmpLShiftBigIntCorrect()
{
    TestResult res;

    for (uint64_t i = 0; i < nSizes; i++)
    {
        for (uint64_t j = 0; j < numCasesPerSize; j++)
        {
            BigInt randInt;
            BigIntRand(testIntBitSizes[i], randInt);

            string binString    = randInt.GetBinaryString();
            uint64_t shift      = rand() % (maxShift - 1) + 1;
            randInt             <<= shift;

            for (uint64_t i = 0; i < shift; i++)
                binString = binString + '0';

            BigInt str2Int(binString, 2);

            if (!(randInt == str2Int))
            {
                char msg[256];
                assert(false);

                sprintf(
                    msg,
                    "BigInt left shift failed. Expected = %s, actual = %s",
                    str2Int.GetHexString().c_str(),
                    randInt.GetHexString().c_str()
                );

                res.caseResults.push_back({ FAIL, string(msg) });
            }
            else
            {
                res.caseResults.push_back({ PASS, "" });
            }
        }
    }

    return res;
}

/**
 * TestCmpRShiftBigIntCorrect - Test big integer right shift. Compare
 * against bit string manipulation version of shift.
 *
 * @return  Pass if shifts match expected patterns, fail otherwise.
 */

TestResult TestCmpRShiftBigIntCorrect()
{
    TestResult res;

    for (uint64_t i = 0; i < nSizes; i++)
    {
        for (uint64_t j = 0; j < numCasesPerSize; j++)
        {
            BigInt randInt;
            BigIntRand(testIntBitSizes[i], randInt);

            string binString    = randInt.GetBinaryString();
            uint64_t shift      = rand() % (maxShift - 1) + 1;
            randInt             >>= shift;

            string shiftString;

            if (shift >= binString.length())
                shiftString = "";
            else
                shiftString = binString.substr(0, binString.length() - shift);

            BigInt str2Int(shiftString, 2);

            if (!(randInt == str2Int))
            {
                char msg[256];
                assert(false);

                sprintf(
                    msg,
                    "BigInt right shift failed. Expected = %s, actual = %s",
                    str2Int.GetHexString().c_str(),
                    randInt.GetHexString().c_str()
                );

                res.caseResults.push_back({ FAIL, string(msg) });
            }
            else
            {
                res.caseResults.push_back({ PASS, "" });
            }
        }
    }


    return res;
}

/**
 * TestCmpAddBigIntCorrect - Test big integer addition. Compare
 * against bit string manipulation version of addition.
 *
 * @return  Pass if additions match expected patterns, fail otherwise.
 */

TestResult TestCmpAddBigIntCorrect()
{
    TestResult res;

    for (uint64_t i = 0; i < nSizes; i++)
    {
        for (uint64_t j = 0; j < numCasesPerSize; j++)
        {
            BigInt a;
            BigIntRand(testIntBitSizes[i], a);
            string aBin = a.GetBinaryString();

            BigInt b;
            BigIntRand(testIntBitSizes[i], b);
            string bBin = b.GetBinaryString();

            a += b;

            vector<uint8_t> binOut;
            AddBinaryStrings(aBin, bBin, binOut);

            BigInt aStrToInt    = BigInt(binOut);
            string outStr       = aStrToInt.GetBinaryString();

            if (!(a == aStrToInt))
            {
                char msg[256];
                assert(false);

                sprintf(
                    msg,
                    "BigInt compound add failed. Expected = %s, actual = %s",
                    a.GetHexString().c_str(),
                    aStrToInt.GetHexString().c_str()
                );

                res.caseResults.push_back({ FAIL, string(msg) });
            }
            else
            {
                res.caseResults.push_back({ PASS, "" });
            }
        }
    }


    return res;
}

/**
 * TestCmpSubBigIntCorrect - Test big integer subtraction. Compare
 * against bit string manipulation version of subtraction.
 *
 * @return  Pass if subtractions match expected patterns, fail otherwise.
 */

TestResult TestCmpSubBigIntCorrect()
{
    TestResult res;

    for (uint64_t i = 0; i < nSizes; i++)
    {
        for (uint64_t j = 0; j < numCasesPerSize; j++)
        {
            BigInt a;
            BigIntRand(testIntBitSizes[i], a);
            string aBin = a.GetBinaryString();

            BigInt b;
            BigIntRand(testIntBitSizes[i], b);
            string bBin = b.GetBinaryString();

            a -= b;

            vector<uint8_t> binOut;
            SubtractBinaryStrings(aBin, bBin, binOut);

            BigInt aStrToInt    = BigInt(binOut);
            string outStr       = aStrToInt.GetBinaryString();

            if (!(a == aStrToInt))
            {
                char msg[256];
                assert(false);

                sprintf(
                    msg,
                    "BigInt compound subtract failed. Expected = %s, actual = %s",
                    a.GetHexString().c_str(),
                    aStrToInt.GetHexString().c_str()
                );

                res.caseResults.push_back({ FAIL, string(msg) });
            }
            else
            {
                res.caseResults.push_back({ PASS, "" });
            }
        }
    }


    return res;
}

/**
 * TestCmpMulBigIntCorrect - Test big integer multiplication. Simulate large
 * integer multiplication using large double floats for validation.
 *
 * @return  Pass if multiplications match expected patterns, fail otherwise.
 */

TestResult TestCmpMulBigIntCorrect()
{
    TestResult res;

    for (uint64_t i = 0; i < nSizes; i++)
    {
        for (uint64_t j = 0; j < numCasesPerSize; j++)
        {
            BigInt a;
            BigIntRand(testIntBitSizes[i], a);
            string aBin = a.GetBinaryString();

            BigInt b;
            BigIntRand(testIntBitSizes[i], b);
            string bBin = b.GetBinaryString();

            a *= b;
            string prod;
            MultiplyBinaryStrings(aBin, bBin, prod);

            BigInt strToInt = BigInt(prod, 2);

            if (!(a == strToInt))
            {
                char msg[256];
                assert(false);

                sprintf(
                    msg,
                    "BigInt compound multiply failed. Expected = %s, actual = %s",
                    a.GetHexString().c_str(),
                    strToInt.GetHexString().c_str()
                );

                res.caseResults.push_back({ FAIL, string(msg) });
            }
            else
            {
                res.caseResults.push_back({ PASS, "" });
            }
        }
    }


    return res;
}

/**
 * TestCmpDivBigIntCorrect - Test big integer division. Generate
 * two random big ints, multiply, then divide again. Check result matches
 * beginning values.
 *
 * @return  Pass if divisions match expected patterns, fail otherwise.
 */

TestResult TestCmpDivBigIntCorrect()
{
    TestResult res;

    for (uint64_t i = 1; i < nSizes; i++)
    {
        for (uint64_t j = 0; j < numCasesPerSize; j++)
        {
            BigInt a;
            BigIntRand(testIntBitSizes[i], a);

            BigInt b;
            BigIntRand(testIntBitSizes[i - 1], b);

            BigInt tmp  = a;
            tmp         *= b;
            tmp         /= b;

            if (!(a == tmp))
            {

#ifdef _DEBUG
                __debugbreak();

                tmp = a;
                tmp *= b;
                tmp /= b;
#endif

                char msg[512];

                sprintf(
                    msg,
                    "BigInt compound divide failed. Expected = %s, actual = %s",
                    a.GetHexString().c_str(),
                    tmp.GetHexString().c_str()
                );

                res.caseResults.push_back({ FAIL, string(msg) });
            }
            else
            {
                res.caseResults.push_back({ PASS, "" });
            }
        }
    }


    return res;
}

/**
 * TestCmpModBigIntCorrect - Test big integer modulus. Generate values
 * a and b, compute q = a / b and r = a % b, and check a = b * q + r. 
 *
 * @return  Pass if divisions match expected patterns, fail otherwise.
 */

TestResult TestCmpModBigIntCorrect()
{
    TestResult res;

    for (uint64_t i = 1; i < nSizes; i++)
    {
        for (uint64_t j = 0; j < numCasesPerSize; j++)
        {
            BigInt a;
            BigIntRand(testIntBitSizes[i], a);

            BigInt b;
            BigIntRand(testIntBitSizes[i - 1], b);

            BigInt q = a;
            BigInt r = a;
            q       /= b;
            r       %= b;

            if (a != ((q * b) + r))
            {
#ifdef _DEBUG
                __debugbreak();

                q = a;
                r = a;
                q /= b;
                r %= b;
#endif
                char msg[256];

                sprintf(
                    msg,
                    "BigInt compound mod failed with a = %s, b = %s, r = %s",
                    a.GetHexString().c_str(),
                    b.GetHexString().c_str(),
                    r.GetHexString().c_str()
                );

                res.caseResults.push_back({ FAIL, string(msg) });
            }
            else
            {
                res.caseResults.push_back({ PASS, "" });
            }
        }
    }


    return res;
}

/**
 * TestSqrtBigIntCorrect - Test big integer square root function. Generate random large ints, square them,
 * then check Sqrt returns the original int.
 *
 * @return  Pass if squareroots match, fail otherwise.
 */

TestResult TestSqrtBigIntCorrect()
{
    TestResult res;

    for (uint64_t i = 0; i < nSizes; i++)
    {
        for (uint64_t j = 0; j < numCasesPerSize; j++)
        {
            BigInt a;
            BigIntRand(testIntBitSizes[i], a);

            BigInt square   = a;
            square          *= a;

            BigInt root = square.Sqrt();

            if (!(a == root))
            {
                char msg[256];
                assert(false);

                sprintf(
                    msg,
                    "BigInt square root failed. Expected = %s, actual = %s",
                    a.GetHexString().c_str(),
                    root.GetHexString().c_str()
                );

                res.caseResults.push_back({ FAIL, string(msg) });
            }
            else
            {
                res.caseResults.push_back({ PASS, "" });
            }
        }
    }


    return res;
}

/**
 * TestGetModInverseBigInt - Test big integer modular inverses. Generate random BigInt pairs (n, k)
 * of various known sizes, check if they're coprime and if so, compute k^-1 mod n. Check
 * k * k^-1 = 1 mod n.
 *
 * @return  Pass if squareroots match, fail otherwise.
 */

TestResult TestGetModInverseBigInt()
{
    TestResult res;

    for (uint64_t i = 1; i < nSizes; i++)
    {
        for (uint64_t j = 0; j < numCasesPerSize; j++)
        {
            //BigInt n;
            //BigInt k;

            BigInt n(4096300225);
            BigInt k(48);

            while (1)
            {
                //BigIntRand(testIntBitSizes[i], n);
                //BigIntRand(testIntBitSizes[i - 1], k);

                BigInt gcd = GetGCD(n, k);

                if (gcd == 1)
                    break;
            }

            BigInt kInv = GetModInverse(k, n);
            BigInt prod = k * kInv;

            if (!((prod % n) == 1))
            {
#ifdef _DEBUG
                __debugbreak();
                
                kInv = GetModInverse(k, n);
                prod = k * kInv;
                prod %= n;
#endif

                char msg[256];

                sprintf(
                    msg,
                    "BigInt modular inverse failed with n = %s, k = %s",
                    n.GetHexString().c_str(),
                    k.GetHexString().c_str()
                );

                res.caseResults.push_back({ FAIL, string(msg) });
            }
            else
            {
                res.caseResults.push_back({ PASS, "" });
            }
        }
    }


    return res;
}