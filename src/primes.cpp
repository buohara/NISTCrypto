#include "primes.h"

/**
 * Seive - Generate a list of prime numbers between min and max.
 * 
 * @param min       [in] Lower end of the prime range, inclusive.
 * @param max       [in] Upper end of the prime range, inclusive.
 * @param primes    [in/out] List of generated primes to populate. Assumed empty on input.
 */

void Seive(const uint64_t min, const uint64_t max, vector<uint64_t> &primes)
{
    if (min > max)
        throw invalid_argument("Prime seive search range min argument greater than max");

    if (primes.size() != 0)
        throw invalid_argument("Prime seive expects prime list argument to be empty");

    uint64_t primeCnt       = 0;

    vector<bool> composites(max - min + 1, false);

    if (min == 0)
        composites[0] = composites[1] = true;

    if (min == 1)
        composites[0] = true;

    for (uint64_t i = 2; i * i <= max; i++)
    {
        uint64_t offset = (i < min) ? ((i * (min / i)) + (min % i ? i : 0) - min) :
            2 * i;

        for (uint64_t j = offset; j < composites.size(); j += i)
            composites[j] = true;
    }

    for (uint64_t i = 0; i < composites.size(); i++)
        if (!composites[i])
            primeCnt++;

    primes.resize(primeCnt);

    uint64_t offset = 0;

    for (uint64_t i = 0; i < composites.size(); i++)
        if (!composites[i])
            primes[offset++] = i + min;
}

/**
 * IsSquare - Determine if an input integer is square.
 * 
 * @param val   [in]
 */

__inline bool IsSquare(const uint64_t val)
{
    for (uint64_t i = 1; i * i <= val; i++)
        if (i * i == val)
            return true;

    return false;
}

/**
 * FactorSemiPrimeNearestSquares64 - Factor an input semiprime N using the nearest squares method.
 * Find candidate solutions x = -b +- sqrt(b^2 + N) for some b to be determined. First,
 * find square numbers A > N such that N - A is a square number b^2, then use b to find
 * prime factors x via the above formula.
 * 
 * @param n     [in] Input value to be factored.
 * @param p1    [in/out] First prime divisor of n, if found, zero otherwise. Assumed zero on input.
 * @param p1    [in/out] Second prime divisor of n, if found, zero otherwise. Assumed zero on input.
 */

void FactorSemiPrimeNearestSquares64(const uint64_t n, uint64_t& p1, uint64_t& p2)
{
    if (p1 != 0)
        throw invalid_argument("Expected input p1 to FactorSemiPrimeNearestSquares to be zero");

    if (p2 != 0)
        throw invalid_argument("Expected input p2 to FactorSemiPrimeNearestSquares to be zero");

    double val      = (double)n;
    uint64_t root   = (uint64_t)floor(sqrt(val));

    root++;

    while (1)
    {
        uint64_t diff = root * root - n;

        if (IsSquare(diff))
        {
            double b    = sqrt((double)diff);
            p1          = (uint64_t)(-b + sqrt(b * b + n));
            p2          = n / p1;

            return;
        }

        root++;
    }
}
