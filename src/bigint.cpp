#include "bigint.h"

static const uint64_t nSizes                    = 10;
static const uint64_t maxShift                  = 16;
static const uint64_t testIntBitSizes[nSizes]   = { 7, 32, 41, 64, 103, 128, 211, 256, 388, 512 };
static const uint64_t numCasesPerSize           = 10;

static void DivideU8(const BigInt& dividend, const uint8_t divisor, BigInt& quotient, uint8_t& rem);
static void AddBinaryStrings(const string& u1, const string& u2, vector<uint8_t> &sum);
static void SubtractBinaryStrings(const string& u1, const string& u2, vector<uint8_t>& sum);
static void MultiplyBinaryStrings(const string& u1, const string& u2, string& prod);

/**
 * ParseHexString - Convert an integer represented as a hex string to a
 * binary integer stored as an array of bytes.
 *
 * @param val       [in] String val to convert to an integer.
 * @param bytes     [in/out] Array of bytes to populate with binary representation of integer.
 */

static void ParseHexString(const string val, vector<uint8_t>& bytes)
{
    if (bytes.size() != 0)
        throw invalid_argument("Expected input byte array to be empty when parsing BigInt hex string.");

    map<char, uint8_t> hexCharVals =
    {
        { '0', 0x0 },
        { '1', 0x1 },
        { '2', 0x2 },
        { '3', 0x3 },
        { '4', 0x4 },
        { '5', 0x5 },
        { '6', 0x6 },
        { '7', 0x7 },
        { '8', 0x8 },
        { '9', 0x9 },
        { 'A', 0xA },
        { 'B', 0xB },
        { 'C', 0xC },
        { 'D', 0xD },
        { 'E', 0xE },
        { 'F', 0xF },
        { 'a', 0xA },
        { 'b', 0xB },
        { 'c', 0xC },
        { 'd', 0xD },
        { 'e', 0xE },
        { 'f', 0xF }
    };

    for (uint64_t i = 0; i < val.length(); i++)
    {
        if (hexCharVals.count(val[i]) == 0)
        {
            throw invalid_argument("BigInt constructor encountered unexpected hex digit in input string %s");
            bytes.push_back(0);
            return;
        }
    }

    bytes.resize(BYTES(val.length() * 4));
    string valRev = val;

    for (uint64_t i = 0; i < val.length(); i++)
        valRev[i] = val[val.length() - i - 1];

    for (uint64_t i = 0; i < val.length(); i++)
    {
        uint64_t byteOut    = i / 2;
        bytes[byteOut]      |= (hexCharVals[val[i]] << 4 * (i % 2));
    }
}

/**
 * ParseBinaryString - Convert an integer represented as a binary string to a
 * binary integer stored as an array of bytes.
 *
 * @param val       [in] String val to convert to an integer.
 * @param bytes     [in/out] Array of bytes to populate with binary representation of integer.
 */

static void ParseBinaryString(const string val, vector<uint8_t>& bytes)
{
    if (bytes.size() != 0)
        throw invalid_argument("Expected input byte array to be empty when parsing BigInt binary string.");

    map<char, uint8_t> binCharVals =
    {
        { '0', 0 },
        { '1', 1 }
    };

    for (uint64_t i = 0; i < val.length(); i++)
    {
        if (binCharVals.count(val[i]) == 0)
        {
            throw invalid_argument("BigInt constructor encountered unexpected binary digit in input string %s");
            bytes.push_back(0);
            return;
        }
    }

    bytes.resize(BYTES(val.length()));
    string valRev = val;

    for (uint64_t i = 0; i < val.length(); i++)
        valRev[i] = val[val.length() - i - 1];

    for (uint64_t i = 0; i < val.length(); i++)
    {
        uint64_t byteOut    = i / 8;
        bytes[byteOut]      |= binCharVals[valRev[i]] << (i % 8);
    }
}

/**
 * ParseDecimalString - Convert an integer represented as a decimal string to a
 * binary integer stored as an array of bytes.
 *
 * @param val       [in] String val to convert to an integer.
 * @param bytes     [in/out] Array of bytes to populate with binary representation of integer.\
 */

static void ParseDecimalString(const string val, vector<uint8_t>& bytes)
{
    if (bytes.size() != 0)
        throw invalid_argument("Expected input byte array to be empty when parsing BigInt decimal string.");

    map<char, uint8_t> decCharVals =
    {
        { '0', 0 },
        { '1', 1 },
        { '2', 2 },
        { '3', 3 },
        { '4', 4 },
        { '5', 5 },
        { '6', 6 },
        { '7', 7 },
        { '8', 8 },
        { '9', 9 }
    };

    for (uint64_t i = 0; i < val.length(); i++)
    {
        if (decCharVals.count(val[i]) == 0)
        {
            throw invalid_argument("BigInt constructor encountered unexpected decimal digit in input string %s");
            bytes.push_back(0);
            return;
        }
    }

    string quot = val;

    while (quot != "")
    {
        string quotTmp;
        bool bLeadZeros     = true;
        uint16_t rem        = 0;

        for (uint64_t i = 0; i < quot.length(); i++)
        {
            rem *= 10;
            rem += quot[i] - '0';

            if (bLeadZeros && (rem / 256 == 0))
                continue;

            quotTmp     += (rem / 256) + '0';
            bLeadZeros  = false;
            rem         = rem % 256;
        }

        bytes.push_back((uint8_t)rem);
        quot = quotTmp;
    }
}

/**
 * BigIntRand - Generate a random big integer with specified number of bits.
 *
 * @param nBits     [in] Number of desired bits for generated int.
 * @param bigInt    [in/out] Big integer of specified bit length to create.
 */

void BigIntRand(const uint64_t nBits, BigInt &bigInt)
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
                res.code = FAIL;
                
                sprintf(
                    msg, 
                    "BigInt left shift failed. Expected = %s, actual = %s",
                    str2Int.GetHexString().c_str(),
                    randInt.GetHexString().c_str()
                );

                res.msg = string(msg);
                return res;
            }
        }
    }

    res.code    = PASS;
    res.msg     = "";

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
                res.code = FAIL;
                
                sprintf(
                    msg, 
                    "BigInt right shift failed. Expected = %s, actual = %s",
                    str2Int.GetHexString().c_str(),
                    randInt.GetHexString().c_str()
                );

                res.msg = string(msg);
                return res;
            }
        }
    }

    res.code    = PASS;
    res.msg     = "";

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
                res.code = FAIL;

                sprintf(
                    msg,
                    "BigInt compound add failed. Expected = %s, actual = %s",
                    a.GetHexString().c_str(),
                    aStrToInt.GetHexString().c_str()
                );

                res.msg = string(msg);
                return res;
            }
        }
    }

    res.code    = PASS;
    res.msg     = "";

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
                res.code = FAIL;

                sprintf(
                    msg,
                    "BigInt compound subtract failed. Expected = %s, actual = %s",
                    a.GetHexString().c_str(),
                    aStrToInt.GetHexString().c_str()
                );

                res.msg = string(msg);
                return res;
            }
        }
    }

    res.code    = PASS;
    res.msg     = "";

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
                res.code = FAIL;

                sprintf(
                    msg,
                    "BigInt compound multiply failed. Expected = %s, actual = %s",
                    a.GetHexString().c_str(),
                    strToInt.GetHexString().c_str()
                );

                res.msg = string(msg);
                return res;
            }
        }
    }

    res.code    = PASS;
    res.msg     = "";

    return res;
}

/**
 * TestCmpDivBigIntCorrect - Test big integer division. Simulate large
 * integer division using large double floats for validation.
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
            BigInt a("10110100111111101010001111011100110010001", 2);
            //BigIntRand(testIntBitSizes[i], a);

            BigInt b("10011000111011101010110000011111", 2);
            //BigIntRand(testIntBitSizes[i - 1], b);

            BigInt tmp = a;

            tmp *= b;
            tmp /= b;

            if (!(a == tmp))
            {
                char msg[256];
                res.code = FAIL;

                sprintf(
                    msg,
                    "BigInt compound divide failed. Expected = %s, actual = %s",
                    a.GetHexString().c_str(),
                    tmp.GetHexString().c_str()
                );

                res.msg = string(msg);
                return res;
            }
        }
    }

    res.code    = PASS;
    res.msg     = "";

    return res;
}

/**
 * TestSqrtCorrect - Test big integer square root function. Generate random large ints, square them,
 * then check Sqrt returns the original int.
 *
 * @return  Pass if squareroots match, fail otherwise.
 */

TestResult TestSqrtCorrect()
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
                res.code = FAIL;

                sprintf(
                    msg,
                    "BigInt square root failed. Expected = %s, actual = %s",
                    a.GetHexString().c_str(),
                    root.GetHexString().c_str()
                );

                res.msg = string(msg);
                return res;
            }
        }
    }

    res.code    = PASS;
    res.msg     = "";

    return res;
}

/**
 * BigInt - Default constructor. Initialize integer to zero.
 */

BigInt::BigInt() : nBits(1)
{
    data.resize(1);
    data[0] = 0;
};

/**
 * BigInt - Construct a big int from an input string and number base.
 *
 * @param val       [in] Input string value.
 * @param base      [in] What number base is the string. Only decimal, binary, hex supported.
 */

BigInt::BigInt(string val, uint64_t base)
{
    if (base != 10 && base != 2 && base != 16)
        throw invalid_argument("Big int string constructor expected base 10, 2, or 16.");

    if (val == "")
    {
        data.resize(1);
        data[0]     = 0;
        nBits       = 1;
        return;
    }

    vector<uint8_t> outBytes;

    if (base == 2)
        ParseBinaryString(val, outBytes);

    if (base == 10)
        ParseDecimalString(val, outBytes);

    if (base == 16)
        ParseHexString(val, outBytes);

    *this = BigInt(outBytes);
}

/**
 * BigInt - Construct a big int from a byte array.
 *
 * @param data       [in] Input integer bytes.
 */

BigInt::BigInt(vector<uint8_t>& dataIn)
{
    if (dataIn.size() == 0)
    {
        nBits   = 1;
        data.resize(1);
        data[0] = 0;
        return;
    }

    uint64_t nBytes = dataIn.size();

    for (uint64_t i = dataIn.size(); i-- > 1;)
    {
        if (dataIn[i] == 0)
        {
            nBytes--;
        }
        else
        {
            break;
        }
    }

    dataIn.resize(nBytes);
    data.resize(nBytes);

    if (dataIn.size() == 1 && dataIn[0] == 0)
    {
        nBits   = 1;
        data[0] = 0;
    }
    else
    {
        nBits = (dataIn.size() - 1) * 8 + (uint64_t)(log2(dataIn[dataIn.size() - 1]) + 1);
        memcpy(&this->data[0], &dataIn[0], data.size());
    }

}

/**
 * BigInt - Construct a big int from an input 64-bit unsigned integer.
 *
 * @param val       [in] Input integer value value.
 */

BigInt::BigInt(uint64_t val)
{
    nBits           = (uint64_t)log2(val) + 1;
    uint64_t bytes  = BYTES(nBits);

    data.resize(bytes);
    memcpy(&data[0], &val, bytes);
}

/**
 * GetDecimalString - Get a decimal string representation of this integer.
 *
 * @return Decimal string representation of this integer.
 */

string BigInt::GetDecimalString()
{
    uint8_t remainder   = 0;
    uint8_t divisor     = 0xA;
    BigInt quot;

    const char digitChars[10] =
    {
        '0', '1', '2', '3', '4',
        '5', '6', '7', '8', '9'
    };

    BigInt tmp = *this;
    string out = "";

    do
    {
        DivideU8(tmp, divisor, quot, remainder);
        out = string{ digitChars[remainder] } + out;
        tmp = quot;
    }
    while (quot.data.size() != 0 && quot.data[0] != 0);

    return out;
}

/**
 * GetBinaryString - Get a binary string representation of this integer.
 *
 * @return Binary string representation of this integer.
 */

string BigInt::GetBinaryString()
{
    string out = "";

    for (uint64_t i = 0; i < nBits; i++)
    {
        uint64_t byte   = i / 8;
        uint64_t bit    = i % 8;
        out             = (data[byte] & (1 << bit) ? "1" : "0") + out;
    }

    return out;
}

/**
 * GetHexString - Get a hex string representation of this integer.
 *
 * @return Hex string representation of this integer.
 */

string BigInt::GetHexString()
{
    char hexChars[16] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };

    string out = "";

    for (uint64_t i = 0; i < data.size(); i++)
    {
        uint8_t valLo = data[i] & 0xF;
        uint8_t valHi = (data[i] & 0xF0) >> 4;

        if (i == data.size() - 1 && valHi == 0)
            out = out + string{ hexChars[valLo] };
        else
            out = out + string{ hexChars[valLo] } + string{ hexChars[valHi] };
    }

    return "0x" + out;
}

/**
 * BigInt::operator== - Check if another integer equals this one.
 *
 * @param rhs       [in] Integer to check equality against.
 *
 * @return          True if integers are equal, false otherwise.
 */

bool BigInt::operator==(const BigInt& rhs) const
{
    if (&rhs == this)
        return true;

    if (nBits != rhs.nBits)
        return false;

    if (memcmp(&data[0], &rhs.data[0], data.size()) != 0)
        return false;

    return true;
}

/**
 * BigInt::operator== - Check if another 64-bit integer equals this one.
 *
 * @param rhs       [in] Integer to check equality against.
 *
 * @return          True if integers are equal, false otherwise.
 */

bool BigInt::operator==(const uint64_t rhs)
{
    uint64_t nBitsRhs = (uint64_t)(log2(rhs) + 1);
    uint64_t rhsBytes = BYTES(nBitsRhs);

    if (rhsBytes != data.size())
        return false;

    for (uint64_t i = 0; i < data.size(); i++)
        if (data[i] != (rhs & (0xFF << (8 * i))))
            return false;

    return true;
}

/**
 * BigInt::operator> - BigInt greater comparison operator.
 *
 * @param rhs       [in] BigInt to compare against.
 *
 * @return          True if this is greater than RHS, false otherwise.
 */

bool BigInt::operator>(const BigInt &rhs) const
{
    if (nBits > rhs.nBits)
        return true;

    if (nBits < rhs.nBits)
        return false;

    for (uint64_t i = data.size(); i-- > 0;)
        if (data[i] != rhs.data[i])
            if (data[i] > rhs.data[i])
                return true;
            else
                return false;

    return false;
}

/**
 * BigInt::operator< - BigInt less than comparison operator.
 *
 * @param rhs       [in] BigInt to compare against.
 *
 * @return          True if this is less than RHS, false otherwise.
 */

bool BigInt::operator<(const BigInt& rhs) const
{
    if (nBits < rhs.nBits)
        return true;

    if (nBits > rhs.nBits)
        return false;

    for (uint64_t i = data.size(); i-- > 0;)
        if (data[i] < rhs.data[i])
            return true;

    return false;
}

/**
 * BigInt::operator= - Assignment operator. Check for self-assignment,
 * copy contents from RHS and return reference to updated contents.
 *
 * @param rhs       [in] Int value to assign to this int.
 *
 * @return Reference to updated int.
 */

BigInt& BigInt::operator=(const BigInt& rhs)
{
    if (this != &rhs)
    {
        nBits = rhs.nBits;
        data.resize(rhs.data.size());
        memcpy(&data[0], &rhs.data[0], data.size());
    }

    return *this;
}

/**
 * BigInt::operator<<= - Compound left shift operator.
 *
 * @param shift     [in] How many bits to shift left.
 *
 * @return Reference to shifted int.
 */

BigInt& BigInt::operator<<=(uint64_t shift)
{
    if (shift == 0)
        return *this;

    uint8_t hiMasks[8]      = { 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
    uint8_t loMasks[8]      = { 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01 };
    uint8_t shiftCarry      = shift % 8;

    uint8_t maskHi          = hiMasks[shiftCarry];
    uint8_t maskLo          = loMasks[shiftCarry];

    vector<uint8_t> valsOut(BYTES(8 * data.size() + shift));
    uint64_t byteShift      = shift / 8;

    for (uint64_t i = 0; i < data.size(); i++)
    {
        uint64_t targetHi   = byteShift + i + ((shift % 8) ? 1 : 0);
        uint64_t targetLo   = byteShift + i;

        valsOut[targetHi]   |= ((data[i] & maskHi) >> (8 - shiftCarry));
        valsOut[targetLo]   |= (data[i] & maskLo) << shiftCarry;
    }

    *this = BigInt(valsOut);
    return *this;
}

/**
 * BigInt::operator>>= Compound right shift operator.
 *
 * @param shift     [in] How many bits to shift right.
 *
 * @return Reference to shifted int.
 */

BigInt& BigInt::operator>>=(uint64_t shift)
{
    if (shift == 0)
        return *this;

    if (shift >= nBits)
    {
        data.resize(1);
        data[0]     = 0;
        nBits       = 1;
        return *this;
    }

    uint8_t loMasks[8]  = { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
    uint8_t hiMasks[8]  = { 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80 };
    uint8_t shiftCarry  = shift % 8;

    uint8_t maskLo      = loMasks[shiftCarry];
    uint8_t maskHi      = hiMasks[shiftCarry];

    vector<uint8_t> valsOut(BYTES(8 * data.size() + shift));
    uint64_t byteShift  = shift / 8;

    for (uint64_t i = 0; i < data.size(); i++)
    {
        int64_t targetLo = i - byteShift - ((shift % 8) ? 1 : 0);
        int64_t targetHi = i - byteShift;

        if (targetHi >= 0)
            valsOut[targetHi] |= ((data[i] & maskHi) >> shiftCarry);

        if (targetLo >= 0)
            valsOut[targetLo] |= (data[i] & maskLo) << (8 - shiftCarry);
    }

    *this = BigInt(valsOut);
    return *this;
}

/**
 * BigInt::operator+= - Compound integer addition operator.
 *
 * @param rhs   [in] Int value to add to current int.
 *
 * @return  Reference to int with input added.
 */

BigInt& BigInt::operator+=(const BigInt& rhs)
{
    uint64_t maxBits    = max(nBits, rhs.nBits) + 1;
    uint64_t outBytes   = BYTES(maxBits);

    data.resize(outBytes);

    uint64_t lSize      = data.size();
    uint64_t rSize      = rhs.data.size();
    uint8_t carry       = 0;

    for (uint64_t i = 0; i < outBytes; i++)
    {
        uint8_t lVal    = i < lSize ? data[i] : 0;
        uint8_t rVal    = i < rSize ? rhs.data[i] : 0;
        uint16_t sum    = lVal + rVal + carry;

        data[i]         = sum & 0xFF;
        carry           = (sum >> 8) != 0;
    }

    uint64_t nBytes = data.size();

    for (uint64_t i = data.size(); i-- > 0;)
        if (data[i] == 0)
            nBytes--;
        else 
            break;

    data.resize(nBytes);
    nBits = 8 * (nBytes - 1) + (uint64_t)(log2(data[data.size() - 1]) + 1);

    return *this;
}

/**
 * BigInt::operator+ - Integer addition operator.
 *
 * @param rhs   [in] Second addend.
 *
 * @return  Sum of this and rhs.
 */

BigInt BigInt::operator+(const BigInt& rhs) const
{
    BigInt res = *this;
    res += rhs;
    return res;
}

/**
 * BigInt::operator-= - Compound integer subtraction operator.
 *
 * @param rhs       [in] Int to subtract from this one.
 *
 * @return          Reference to integer difference.
 */

BigInt& BigInt::operator-=(const BigInt& rhs)
{
    if (rhs > *this || rhs == *this)
    {
        data.resize(1);
        data[0] = 0;
        nBits   = 1;
        return *this;
    }

    uint64_t maxBits    = max(nBits, rhs.nBits) + 1;
    uint64_t outBytes   = BYTES(maxBits);
    uint64_t borrow     = 0;

    data.resize(outBytes);

    uint64_t lSize      = data.size();
    uint64_t rSize      = rhs.data.size();

    for (uint64_t i = 0; i < outBytes; i++)
    {
        uint16_t lVal    = i < lSize ? data[i] : 0;
        uint16_t rVal    = i < rSize ? rhs.data[i] : 0;

        if (borrow)
        {
            if (lVal > 0)
                borrow = 0;

            uint8_t tmp = lVal & 0xFF;
            tmp--;
            lVal = tmp;
        }

        if (rVal > lVal)
            borrow = 1;

        data[i] = (uint8_t)(256 * borrow + lVal - rVal);
    }

    uint64_t nBytes = outBytes;

    for (uint64_t i = outBytes; i-- > 0; )
    {
        if (data[i] == 0)
            nBytes--;
        else
            break;
    }

    data.resize(nBytes);
    nBits = 8 * (nBytes - 1) + (uint64_t)(log2(data[nBytes - 1]) + 1);

    return *this;
}

/**
 * BigInt::operator- - Integer subraction operator.
 *
 * @param rhs   [in] Int to subtract from this.
 *
 * @return  Difference of this and rhs.
 */

BigInt BigInt::operator-(const BigInt& rhs) const
{
    BigInt res = *this;
    res -= rhs;
    return res;
}


/**
 * BigInt::operator*= - Compound integer multiplication.
 *
 * @param rhs       [in] Integer to multiply this integer by.
 *
 * @return          Reference to product.
 */

BigInt& BigInt::operator*=(const BigInt& rhs)
{
    uint64_t n          = data.size();
    uint64_t m          = rhs.data.size();
    uint64_t bytesOut   = n + m - 1;

    vector<uint8_t> product;
    uint32_t carry = 0;

    for (uint64_t i = 0; i < bytesOut; i++)
    {
        uint32_t sum = 0;

        for (uint64_t j = 0; j <= i; j++)
        {
            uint64_t k = i - j;

            if (j >= n || k >= m)
                continue;

            sum += data[j] * rhs.data[k];
        }

        sum += carry;

        uint8_t out     = sum & 0xFF;
        carry           = sum >> 8;

        product.push_back(out);
    }

    if (carry)
        product.push_back(carry);

    *this = BigInt(product);
    return *this;
}

/**
 * BigInt::operator* - Integer multiplication operator.
 *
 * @param rhs   [in] Int to multiply this one by.
 *
 * @return  Product of this and rhs.
 */

BigInt BigInt::operator*(const BigInt& rhs) const
{
    BigInt res = *this;
    res *= rhs;
    return res;
}

/**
 * BigInt::operator*= - Compound integer multiplication.
 *
 * @param rhs       [in] Integer to multiply this integer by.
 *
 * @return          Reference to product.
 */

BigInt& BigInt::operator*=(const uint8_t rhs)
{
    uint16_t dig;
    uint16_t carry;

    vector<uint8_t> vals;

    for (uint64_t i = 0; i < data.size(); i++)
    {
        dig = data[i] * rhs;
        vals.push_back(dig & 0xFF);
        carry = dig >> 8;
    }

    if (carry)
        vals.push_back((uint8_t)carry);

    *this = BigInt(vals);
    return *this;
}

/**
 * BigInt::operator* - Integer multiplication operator.
 *
 * @param rhs   [in] Int to multiply this one by.
 *
 * @return  Product of this and rhs.
 */

BigInt BigInt::operator*(const uint8_t rhs) const
{
    BigInt res = *this;
    res *= rhs;
    return res;
}

/**
 * BigInt::operator/= - BigInt compound division operator.
 *
 * @param rhs   [in] Divisor.
 *
 * @return      Quotient of this int divided by RHS.
 */

BigInt& BigInt::operator/=(const BigInt& rhs)
{
    if (rhs > *this)
    {
        data.resize(1);
        data[0]     = 0;
        nBits       = 1;
        
        return *this;
    }

    if (this == &rhs || *this == rhs)
    {
        data.resize(1);
        data[0]     = 1;
        nBits       = 1;
        
        return *this;
    }

    vector<uint8_t> vals;

    BigInt rem      = *this;
    uint8_t leadRHS = rhs.data[rhs.data.size() - 1];

    while (1)
    {
        uint16_t leadDiv    = rem.data[rem.data.size() - 1];
        uint64_t i          = 1;

        while (leadDiv <= leadRHS)
        {
            leadDiv = leadDiv * 256 + rem.data[rem.data.size() - i - 1];
            i++;
        }

        uint8_t curQuot = leadDiv / leadRHS;
        uint32_t shift  = 8 * (rem.data.size() - i - rhs.data.size() + 1);
        
        BigInt cur      = BigInt(curQuot);
        cur             <<= shift;

        while (rhs * cur > rem)
        {
            curQuot--;
            cur = BigInt(curQuot);
            cur <<= shift;
        }

        vals.insert(vals.begin(), curQuot);
        rem = rem - rhs * cur;

        if (rhs > rem)
            break;
    }

    *this = BigInt(vals);
    return *this;
}

/**
 * BigInt::operator/= - BigInt compound division operator.
 *
 * @param rhs   [in] Divisor.
 *
 * @return      Quotient of this int divided by RHS.
 */

BigInt& BigInt::operator/=(const uint8_t rhs)
{
    BigInt dividend = *this;
    uint8_t divisor = rhs;
    BigInt quot;
    uint8_t rem;

    DivideU8(dividend, divisor, quot, rem);
    *this = quot;
    return *this;
}

/**
 * BigInt::operator/ - Integer division operator.
 *
 * @param rhs   [in] Int to divide this one by.
 *
 * @return  Quotient of this and rhs.
 */

BigInt BigInt::operator/(const BigInt& rhs) const
{
    BigInt res  = *this;
    res         /= rhs;
    return      res;
}

/**
 * BigInt::operator++ - Increment this int by 1.
 *
 * @return This int + 1.
 */

BigInt& BigInt::operator++(int rhs)
{
    uint8_t carry       = 1;

    for (uint64_t i = 0; i < data.size(); i++)
    {
        uint16_t inc    = (uint16_t)data[i] + carry;
        data[i]         = inc & 0xFF;
        carry           = inc >> 8;
        
        if (!carry)
            break;
    }

    if (carry)
    {
        data.push_back(1);
        nBits++;
    }

    return *this;
}

/**
 * BigInt::operator-- - Decrement this int by 1.
 *
 * @return This int - 1.
 */

BigInt& BigInt::operator--(int rhs)
{
    for (uint64_t i = 0; i < data.size(); i++)
    {
        if (data[i] == 0)
        {
            data[i] = 0xFF;
        }
        else
        {
            data[i]--;
            break;
        }
    }

    if (data[data.size() - 1] == 0)
    {
        data.resize(data.size() - 1);
        nBits--;
    }

    return *this;
}

/**
 * BigInt::Sqrt - Get the square root of this int N using a binary search. 
 * The squareroot s is the smallest int such that s * s <= N. 
 *
 * @return BigInt s such that s * s <= N.
 */

BigInt BigInt::Sqrt() const
{
    BigInt out;
    string lo   = string("1") + string(nBits / 2 - 1, '0');
    string hi   = string("1") + string(nBits / 2 + 1, '0');

    BigInt l(lo, 2);
    BigInt h(hi, 2);
    BigInt tmp;

    while (1)
    {
        BigInt m    = l + h;
        m           /= 2;
        tmp         = m;
        tmp         *= m;

        if (tmp == *this)
        {
            out = m;
            break;
        }

        if (l + 1 == h && h * h > *this)
        {
            out = l;
            break;
        }

        if (tmp > *this)
            h = m;
        else
            l = m;
    }

    return out;
}

/**
 * DivideU8 - Divide an arbitrary size integer by an input 8-bit int.
 *
 * @param dividend      [in]        Input integer to divide.
 * @param divisor       [in]        Input 8-bit int to divide by.
 * @param quotient      [in/out]    Output quotient.
 * @param rem           [in/out]    Output remainder.
 */

static void DivideU8(const BigInt& dividend, const uint8_t divisor, BigInt& quotient, uint8_t &rem)
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
        
        sum |= (dig << (i % 8));
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
            dig     = 1;
            borrow  = 1;
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
 * IsSquareBigInt - Check if a big integer is a perfect square.
 *
 * @param i        [in] Int to check for squareness.
 * 
 * @return          True if the input is square, false otherwise.
 */

bool IsSquareBigInt(const BigInt& i)
{
    BigInt root = i.Sqrt();
    if (root * root == i)
        return true;

    return false;
}