#pragma once

#include "commoninc.h"

using namespace std;

#define BYTES(x) (((x) / 8) + ((x) % 8 ? 1 : 0))

#include "test.h"

TestResult TestAssignBigInt();
TestResult TestLeftShiftBigInt();
TestResult TestRightShiftBigInt();
TestResult TestCompoundAddBigInt();
TestResult TestCompoundSubtractBigInt();
TestResult TestCompoundMultiplyBigInt();
TestResult TestCompoundDivideBigInt();


struct BigInt;
void DivideU8(BigInt& dividend, uint8_t divisor, BigInt& quotient, uint8_t &rem);
void BigIntRand(uint64_t nBits, BigInt& bigInt);

struct BigInt
{
    vector<uint8_t> data;
    uint64_t nBits;

    /**
     * BigInt - Default constructor. Initialize integer to zero.
     */

    BigInt() : nBits(1) 
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

    BigInt(string val, uint64_t base)
    {
        if (base != 10 && base != 2 && base != 16)
            throw invalid_argument("Big int string constructor expected base 10, 2, or 16.");

        if (base == 2)
        {
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
                    nBits = 1;
                    data.push_back(0);
                    return;
                }
            }

            nBits = val.length();
            data.resize(BYTES(nBits));
            string valRev = val;

            for (uint64_t i = 0; i < val.length(); i++)
                valRev[i] = val[val.length() - i - 1];

            for (uint64_t i = 0; i < val.length(); i++)
            {
                uint64_t byteOut = i / 8;
                data[byteOut] |= binCharVals[valRev[i]] << (i % 8);
            }
        }

        if (base == 10)
        {
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
                    nBits = 1;
                    data.push_back(0);
                    return;
                }
            }

            vector<uint8_t> outBytes;
            string quot         = val;

            while (quot != "")
            {
                string quotTmp;
                bool bLeadZeros = true;
                uint16_t rem    = 0;

                for (uint64_t i = 0; i < quot.length(); i++)
                {
                    rem         *= 10;
                    rem         += quot[i] - '0';

                    if (bLeadZeros && (rem / 256 == 0))
                        continue;

                    quotTmp     += (rem / 256) + '0';
                    bLeadZeros  = false;
                    rem         = rem % 256;
                }

                outBytes.push_back((uint8_t)rem);
                quot = quotTmp;
            }

            *this = BigInt(outBytes);
        }

        if (base == 16)
        {
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
                    nBits = 1;
                    data.push_back(0);
                    return;
                }
            }

            nBits = val.length() * 4;
            data.resize(BYTES(nBits));
            string valRev = val;

            for (uint64_t i = 0; i < val.length(); i++)
                valRev[i] = val[val.length() - i - 1];

            for (uint64_t i = 0; i < val.length(); i++)
            {
                uint64_t byteOut = i / 2;
                data[byteOut] |= (hexCharVals[val[i]] << 4 * (i % 2));
            }
        }
    }

    /**
     * BigInt::operator== - Check if another integer equals this one.
     *
     * @param rhs       [in] Integer to check equality against.
     * 
     * @return          True if integers are equal, false otherwise.
     */

    bool operator==(const BigInt& rhs) const
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

    bool operator==(const uint64_t rhs)
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
     * BigInt - Construct a big int from a byte array.
     *
     * @param data       [in] Input integer bytes.
     */

    BigInt(vector<uint8_t>& dataIn)
    {
        if (dataIn.size() == 0)
        {
            nBits = 1;
            data.resize(1);
            data[0] = 0;
            return;
        }

        this->data.resize(dataIn.size());

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

    BigInt(uint64_t val)
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

    string GetDecimalString()
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

    string GetBinaryString()
    {
        string out = "";

        for (uint64_t i = 0; i < nBits; i++)
        {
            uint64_t byte   = i / 8;
            uint64_t bit    = i % 8;

            out = (data[byte] & (1 << bit) ? "1" : "0") + out;
        }

        return out + "b";
    }

    /**
     * GetHexString - Get a hex string representation of this integer.
     *
     * @return Hex string representation of this integer.
     */

    string GetHexString()
    {
        char hexChars[16] =
        { 
            '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
        };

        string out = "";

        for (uint64_t i = 0; i < data.size(); i++)
        {
            uint8_t valLo   = data[i] & 0xF;
            uint8_t valHi   = (data[i] & 0xF0) >> 4;

            if (i == data.size() - 1 && valHi == 0)
                out = out + string{ hexChars[valLo] };
            else
                out = out + string{ hexChars[valLo] } + string{ hexChars[valHi] };
        }

        return "0x" + out;
    }

    /**
     * BigInt::operator= - Assignment operator. Check for self-assignment,
     * copy contents from RHS and return reference to updated contents.
     * 
     * @param rhs       [in] Int value to assign to this int.
     * 
     * @return Reference to updated int.
     */

    BigInt& operator=(const BigInt& rhs)
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

    BigInt& operator<<=(uint64_t shift)
    {
        if (shift == 0)
            return *this;

        uint8_t hiMasks[8]  = { 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
        uint8_t loMasks[8]  = { 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01 };
        uint8_t shiftCarry  = shift % 8;

        uint8_t maskHi      = hiMasks[shiftCarry];
        uint8_t maskLo      = loMasks[shiftCarry];

        vector<uint8_t> valsOut(BYTES(nBits + shift));
        uint64_t byteShift  = shift / 8;

        for (uint64_t i = 0; i < data.size(); i++)
        {
            uint64_t targetHi = byteShift + i + (((shift % 8) + nBits > 8) ? 1 : 0);
            uint64_t targetLo = byteShift + i;

            valsOut[targetHi] |= ((data[i] & maskHi) >> (8 - shiftCarry));
            valsOut[targetLo] |= (data[i] & maskLo) << shiftCarry;
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

    BigInt& operator>>=(uint64_t shift)
    {
        if (shift == 0)
            return *this;

        if (shift >= nBits)
        {
            data.resize(1);
            data[0] = 0;
            nBits   = 1;
            return *this;
        }

        return *this;
    }

    /**
     * BigInt::operator+= - Compound integer addition operator.
     *
     * @param rhs   [in] Int value to add to current int.
     *
     * @return  Reference to int with input added.
     */

    BigInt& operator+=(const BigInt& rhs)
    {
        uint64_t maxBits    = max(nBits, rhs.nBits) + 1;
        uint64_t nBytes     = BYTES(maxBits);
        
        data.resize(nBytes);
        uint8_t carry = 0;

        for (uint64_t i = 0; i < data.size(); i++)
        {
            uint16_t sum    = data[i] + rhs.data[i] + carry;
            data[i]         = sum & 0xFF;
            carry           = sum >> 8;
        }

        return *this;
    }

    /**
     * BigInt::operator-= - Compound integer subtraction operator.
     *
     * @param rhs       [in] Int to subtract from this one.
     *
     * @return          Reference to integer difference.
     */

    BigInt& operator-=(const BigInt& rhs)
    {
        nBits               = max(nBits, rhs.nBits) + 1;
        uint64_t nBytes     = BYTES(nBits);

        data.resize(nBytes);

        for (uint64_t i = 0; i < data.size(); i++)
            data[i] -= rhs.data[i];

        return *this;
    }

    /**
     * BigInt::operator*= - Compound integer multiplication.
     *
     * @param rhs       [in] Integer to multiply this integer by.
     *
     * @return          Reference to product.
     */

    BigInt& operator*=(const BigInt& rhs)
    {
        uint64_t n          = data.size();
        uint64_t m          = rhs.data.size();
        uint64_t bytesOut   = n + m - 1;

        vector<uint8_t> product;
        uint8_t carry = 0;

        for (uint64_t i = 0; i < bytesOut; i++)
        {
            uint16_t sum    = 0;

            for (uint64_t j = 0; j <= i; j++)
            {
                uint64_t k = i - j;
                
                if (j >= n || k >= m)
                    continue;

                sum += data[j] * rhs.data[k];
            }

            uint8_t out = (sum & 0x00FF) + carry;
            product.push_back(out);
            carry       = (sum >> 8) & 0xFF;
        }

        if (carry)
            product.push_back(carry);

        *this = BigInt(product);
        return *this;
    }

    /**
     * BigInt::operator/= -
     *
     * @param rhs
     *
     * @return
     */

    BigInt& operator/=(const BigInt& rhs)
    {
        return *this;
    }

    BigInt Sqrt()
    {
        BigInt out;

        uint64_t lo = nBits / 2;
        uint64_t hi = lo + 1;
    }
};