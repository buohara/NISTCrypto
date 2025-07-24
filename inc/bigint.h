#pragma once

#include "commoninc.h"
#include "utils.h"

using namespace std;

struct BigInt
{
    vector<uint8_t> data;
    uint64_t nBits;

    BigInt();
    BigInt(string val, uint64_t base);
    BigInt(vector<uint8_t>& dataIn);
    BigInt(uint64_t val);
    BigInt(const BigInt& other);

    string GetDecimalString();
    string GetBinaryString();
    string GetHexString();

    bool operator==(const BigInt& rhs) const;
    bool operator==(const uint64_t rhs);
    bool operator>(const BigInt& rhs) const;
    bool operator<(const BigInt& rhs) const;
    bool operator>=(const BigInt& rhs) const;
    bool operator<=(const BigInt& rhs) const;
    BigInt& operator=(const BigInt& rhs);
    BigInt& operator<<=(uint64_t shift);
    BigInt& operator>>=(uint64_t shift);
    BigInt& operator+=(const BigInt& rhs);
    BigInt operator+(const BigInt& rhs) const;
    BigInt& operator-=(const BigInt& rhs);
    BigInt operator-(const BigInt& rhs) const;
    BigInt& operator*=(const BigInt& rhs);
    BigInt operator*(const BigInt& rhs) const;
    BigInt& operator*=(const uint8_t rhs);
    BigInt operator*(const uint8_t rhs) const;
    BigInt& operator/=(const BigInt& rhs);
    BigInt& operator/=(const uint8_t rhs);
    BigInt& operator%=(const BigInt& rhs);
    BigInt operator%(const BigInt& rhs) const;
    BigInt operator/(const BigInt& rhs) const;
    BigInt& operator++(int);
    BigInt& operator--(int);
    bool operator!=(const BigInt& rhs) const;
    BigInt Sqrt() const;
};

void BigIntRand(uint64_t nBits, BigInt& bigInt);
bool IsSquareBigInt(const BigInt& i);
BigInt GetModInverse(BigInt k, BigInt n);
BigInt GetGCD(BigInt aIn, BigInt bIn);