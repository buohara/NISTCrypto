#pragma once

#include "commoninc.h"
#include "bigint.h"

struct NearestSquareFactors
{
    uint64_t n;
    double sqrt;
    uint64_t p1;
    uint64_t p2;
    double lP1;
    double lP2;
};

using namespace std;

void FactorSemiPrimeNearestSquares64(const uint64_t n, uint64_t& p1, uint64_t& p2);
void FactorSemiPrimeNearestSquaresBigInt(BigInt& n, BigInt& p1, BigInt& p2);
void GenerateNearestSquareFactors(const uint64_t max, vector<NearestSquareFactors>& nsFacs);