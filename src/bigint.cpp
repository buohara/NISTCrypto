#include "bigint.h"

static const uint64_t nSizes                    = 10;
static const uint64_t maxShift                  = 16;
static const uint64_t testIntBitSizes[nSizes]   = { 7, 32, 41, 64, 103, 128, 211, 256, 388, 512 };
static const uint64_t numCasesPerSize           = 10;

/**
 * BigIntRand - Generate a random big integer with specified number of bits.
 *
 * @param val   [in] Input integer value array to convert to double.
 */

void BigIntRand(uint64_t nBits, BigInt &bigInt)
{
    uint64_t nBytes = BYTES(nBits);
    vector<uint8_t> vals(nBytes);

    for (uint64_t i = 0; i < nBytes; i++)
        vals[i] = rand() % 256;

    uint64_t msByteBits = nBits % 8;

    if (msByteBits)
        vals[nBytes - 1] = rand() % (1 << msByteBits);

    bigInt = BigInt(vals);
}

/**
 * HexArrayToDouble - For testing large integers, use 64-bit float arithmetic
 * for comparison. Here, convert a byte array to a double.
 *
 * @param val   [in] Input integer value array to convert to double.
 * 
 * @return      Double representation of input integer.
 */

double BigIntToDouble(const BigInt& val)
{
    double result   = 0.0;
    double pow      = 1.0;

    for (uint64_t i = 0; i < val.data.size(); i++)
    {
        if ((double)val.data[i] > DBL_MAX / pow || result > DBL_MAX - ((double)val.data[i]) * pow)
            throw overflow_error("Double overflow converting big integer to double.");

        result  += ((double)val.data[i]) * pow;
        pow     *= 16.0;
    }

    return result;
}

/**
 * DoubleToBigInt - Convert an integer in double to an integer as an array
 * of hex values.
 *
 * @param val   [in] Input double to convert to hex array.
 * @param out   [in/out] Array to populate with hex values for output integer.
 */

void DoubleToBigInt(const double val, BigInt &out)
{
    if (val == 0.0)
    {
        out.data.resize(0);
        out.data[0]         = 0;
        out.nBits           = 1;
        return;
    }

    double tmp              = val;
    uint64_t outBytes       = 1;
    double base             = 256.0;
    double pow              = 256.0;

    while (pow < val)
    {
        pow *= base;
        outBytes++;
    }

    vector<uint8_t> outVals(outBytes);
    
    while (tmp > 0.0)
    {
        uint64_t outIdx     = 0;
        pow                 = 1.0;

        while (pow * base < tmp)
        {
            pow *= base;
            outIdx++;
        }

        double rem          = fmod(tmp, pow);
        double quot         = (tmp - rem) / pow;
        outVals[outIdx]     = (uint8_t)quot;
        tmp                 -= pow * quot;
    }

    out = BigInt(outVals);
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
            res.code = FAIL;

            sprintf(
                msg,
                "BigInt assignment operator failed with a = %s, b = %s.",
                a.GetBinaryString().c_str(),
                b.GetBinaryString().c_str()
            );

            res.msg = string(msg);
            return res;
        }
    }

    res.code = PASS;
    return res;
}

/**
 * TestLeftShiftBigInt - Test big integer left shift. Simulate large
 * integer shifting using large double floats, i.e., shifting an int
 * A via A << k is the same as multiplying an equivalent double B = A
 * via B * 2.0^k.
 *
 * @return  Pass if shifts match expected patterns, fail otherwise.
 */

TestResult TestLeftShiftBigInt()
{
    TestResult res;

    for (uint64_t i = 0; i < nSizes; i++)
    {
        for (uint64_t j = 0; j < numCasesPerSize; j++)
        {
            BigInt randInt;
            BigIntRand(testIntBitSizes[i], randInt);

            double randDbl  = BigIntToDouble(randInt);
            uint64_t shift  = (rand() + 1) % maxShift;
            randInt         <<= shift;

            for (uint64_t k = 0; k < shift; k++)
                randDbl *= 2.0;

            BigInt dbl2Int;
            DoubleToBigInt(randDbl, dbl2Int);

            if (!(randInt == dbl2Int))
            {
                char msg[256];
                res.code = FAIL;
                
                sprintf(
                    msg, 
                    "BigInt left shift failed. Expected = %s, actual = %s",
                    dbl2Int.GetHexString().c_str(),
                    randInt.GetHexString().c_str()
                );

                res.msg = string(msg);
            }
        }
    }

    res.code    = PASS;
    res.msg     = "";

    return res;
}

/**
 * TestRightShiftBigInt - Test big integer right shift. Simulate large
 * integer shifting using large double floats, i.e., shifting an int
 * A via A >> k is the same as dividing an equivalent double B = A
 * via B / 2.0^k. Compare doubles against bigints for validation.
 *
 * @return  Pass if shifts match expected patterns, fail otherwise.
 */

TestResult TestRightShiftBigInt()
{
    TestResult res;

    for (uint64_t i = 0; i < nSizes; i++)
    {
        for (uint64_t j = 0; j < numCasesPerSize; j++)
        {
            BigInt randInt;
            BigIntRand(testIntBitSizes[i], randInt);

            double randDbl  = BigIntToDouble(randInt);
            uint64_t shift  = rand() % maxShift;
            randInt         >>= shift;

            for (uint64_t k = 0; k < shift; k++)
            {
                randDbl -= fmod(randDbl, 2.0);
                randDbl /= 2.0;
            }

            BigInt dbl2Int;
            DoubleToBigInt(randDbl, dbl2Int);

            if (!(randInt == dbl2Int))
            {
                char msg[256];
                res.code = FAIL;
                
                sprintf(
                    msg, 
                    "BigInt left shift failed. Expected = %s, actual = %s",
                    dbl2Int.GetHexString().c_str(),
                    randInt.GetHexString().c_str()
                );

                res.msg = string(msg);
            }
        }
    }

    res.code    = PASS;
    res.msg     = "";

    return res;
}

/**
 * TestCompoundAddBigInt - Test big integer addition. Simulate large
 * integer addition using large double floats for validation.
 *
 * @return  Pass if additions match expected patterns, fail otherwise.
 */

TestResult TestCompoundAddBigInt()
{
    TestResult res;

    for (uint64_t i = 0; i < nSizes; i++)
    {
        for (uint64_t j = 0; j < numCasesPerSize; j++)
        {
            BigInt a;
            BigIntRand(testIntBitSizes[i], a);

            BigInt b;
            BigIntRand(testIntBitSizes[i], b);

            double aDbl = BigIntToDouble(a);
            double bDbl = BigIntToDouble(b);
            a           += b;
            aDbl        += bDbl;

            BigInt aDblToInt;
            DoubleToBigInt(aDbl, aDblToInt);

            if (!(a == aDblToInt))
            {
                char msg[256];
                res.code = FAIL;

                sprintf(
                    msg,
                    "BigInt compound failed. Expected = %s, actual = %s",
                    a.GetHexString().c_str(),
                    aDblToInt.GetHexString().c_str()
                );

                res.msg = string(msg);
            }
        }
    }

    res.code    = PASS;
    res.msg     = "";

    return res;
}

/**
 * TestCompoundSubtractBigInt - Test big integer subtraction. Simulate large
 * integer subtraction using large double floats for validation.
 *
 * @return  Pass if subtractions match expected patterns, fail otherwise.
 */

TestResult TestCompoundSubtractBigInt()
{
    TestResult res;

    for (uint64_t i = 0; i < nSizes; i++)
    {
        for (uint64_t j = 0; j < numCasesPerSize; j++)
        {
            BigInt a;
            BigIntRand(testIntBitSizes[i], a);

            BigInt b;
            BigIntRand(testIntBitSizes[i], b);

            double aDbl = BigIntToDouble(a);
            double bDbl = BigIntToDouble(b);
            a           -= b;
            aDbl        -= bDbl;

            BigInt aDblToInt;
            DoubleToBigInt(aDbl, aDblToInt);

            if (!(a == aDblToInt))
            {
                char msg[256];
                res.code = FAIL;

                sprintf(
                    msg,
                    "BigInt left shift failed. Expected = %s, actual = %s",
                    a.GetHexString().c_str(),
                    aDblToInt.GetHexString().c_str()
                );

                res.msg = string(msg);
            }
        }
    }

    res.code    = PASS;
    res.msg     = "";

    return res;
}

/**
 * TestCompoundMultiplyBigInt - Test big integer multiplication. Simulate large
 * integer multiplication using large double floats for validation.
 *
 * @return  Pass if multiplications match expected patterns, fail otherwise.
 */

TestResult TestCompoundMultiplyBigInt()
{
    TestResult res;

    for (uint64_t i = 0; i < nSizes; i++)
    {
        for (uint64_t j = 0; j < numCasesPerSize; j++)
        {
            BigInt a;
            BigIntRand(testIntBitSizes[i], a);
            double aDbl = BigIntToDouble(a);

            BigInt b;
            double bDbl;

            while (1) 
            {
                BigIntRand(testIntBitSizes[i], b);
                bDbl = BigIntToDouble(b);

                if (DBL_MAX / bDbl >= aDbl)
                    break;
            }

            a           *= b;
            aDbl        *= bDbl;

            BigInt aDblToInt;
            DoubleToBigInt(aDbl, aDblToInt);

            if (!(a == aDblToInt))
            {
                char msg[256];
                res.code = FAIL;

                sprintf(
                    msg,
                    "BigInt left shift failed. Expected = %s, actual = %s",
                    a.GetHexString().c_str(),
                    aDblToInt.GetHexString().c_str()
                );

                res.msg = string(msg);
            }
        }
    }

    res.code    = PASS;
    res.msg     = "";

    return res;
}

/**
 * TestCompoundDivideBigInt - Test big integer division. Simulate large
 * integer division using large double floats for validation.
 *
 * @return  Pass if divisions match expected patterns, fail otherwise.
 */

TestResult TestCompoundDivideBigInt()
{
    TestResult res;

    for (uint64_t i = 0; i < nSizes; i++)
    {
        for (uint64_t j = 0; j < numCasesPerSize; j++)
        {
            BigInt a;
            BigIntRand(testIntBitSizes[i], a);

            BigInt b;
            BigIntRand(testIntBitSizes[i], b);

            double aDbl     = BigIntToDouble(a);
            double bDbl     = BigIntToDouble(b);
            a               /= b;
            aDbl            /= bDbl;

            BigInt aDblToInt;
            DoubleToBigInt(aDbl, aDblToInt);

            if (!(a == aDblToInt))
            {
                char msg[256];
                res.code = FAIL;

                sprintf(
                    msg,
                    "BigInt left shift failed. Expected = %s, actual = %s",
                    a.GetHexString().c_str(),
                    aDblToInt.GetHexString().c_str()
                );

                res.msg = string(msg);
            }
        }
    }

    res.code    = PASS;
    res.msg     = "";

    return res;
}

void DivideU8(BigInt& dividend, uint8_t divisor, BigInt& quotient, uint8_t &rem)
{
    vector<uint8_t> quot;
    uint16_t remainder  = 0;
    uint16_t cur        = 0;

    for (uint64_t i = dividend.data.size(); i-- > 0;)
    {
        remainder   <<= 8;
        cur         = remainder + dividend.data[i];
        remainder   = cur % divisor;

        if (i == dividend.data.size() - 1 && cur / divisor == 0)
            continue;

        quot.insert(quot.begin(), cur / divisor);
    }

    rem         = (uint8_t)remainder;
    quotient    = BigInt(quot);
}