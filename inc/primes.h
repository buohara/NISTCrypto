#pragma once

#include "commoninc.h"
#include "bigint.h"

using namespace std;

void Seive(const uint64_t min, const uint64_t max, vector<uint64_t> &primes);
void FactorSemiPrimeNearestSquares64(const uint64_t n, uint64_t &p1, uint64_t& p2);
void FactorSemiPrimeNearestSquaresBigInt(BigInt& n, BigInt& p1, BigInt& p2);