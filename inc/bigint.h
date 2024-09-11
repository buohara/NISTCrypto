#pragma once

#include "commoninc.h"

using namespace std;

#define BYTES(x) (((x) / 8) + ((x) % 8 ? 1 : 0))

#include "test.h"

TestResult TestAssignBigInt();
TestResult TestCmpLShiftBigIntCorrect();
TestResult TestCmpRShiftBigIntCorrect();
TestResult TestCmpAddBigIntCorrect();
TestResult TestCmpSubBigIntCorrect();
TestResult TestCmpMulBigIntCorrect();
TestResult TestCmpDivBigIntCorrect();

struct BigInt
{
    vector<uint8_t> data;
    uint64_t nBits;

    BigInt();
    BigInt(string val, uint64_t base);
    BigInt(vector<uint8_t>& dataIn);
    BigInt(uint64_t val);

    string GetDecimalString();
    string GetBinaryString();
    string GetHexString();

    bool operator==(const BigInt& rhs) const;
    bool operator==(const uint64_t rhs);
    bool operator>(const BigInt& rhs) const;
    bool operator<(const BigInt& rhs) const;
    BigInt& operator=(const BigInt& rhs);
    BigInt& operator<<=(uint64_t shift);
    BigInt& operator>>=(uint64_t shift);
    BigInt& operator+=(const BigInt& rhs);
    BigInt& operator-=(const BigInt& rhs);
    BigInt& operator*=(const BigInt& rhs);
    BigInt& operator/=(const BigInt& rhs);

    BigInt Sqrt();
};

void BigIntRand(uint64_t nBits, BigInt& bigInt);