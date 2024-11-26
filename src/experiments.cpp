#include "experiments.h"
#include "test.h"

/**
 * IsSquare - Determine if an input integer is square.
 *
 * @param val   [in] Value to check for wquareness.
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

    double val = (double)n;
    uint64_t root = (uint64_t)floor(sqrt(val));

    root++;

    while (1)
    {
        uint64_t diff = root * root - n;

        if (IsSquare(diff))
        {
            double b = sqrt((double)diff);
            p1 = (uint64_t)(-b + sqrt(b * b + n));
            p2 = n / p1;

            return;
        }

        root++;
    }
}

/**
 * FactorSemiPrimeNearestSquaresBigInt - Factor an input semiprime N using the nearest squares method.
 * Find candidate solutions x = -b +- sqrt(b^2 + N) for some b to be determined. First,
 * find square numbers A > N such that N - A is a square number b^2, then use b to find
 * prime factors x via the above formula. Use arbitrary integers.
 *
 * @param n     [in] Input value to be factored.
 * @param p1    [in/out] First prime divisor of n, if found, zero otherwise. Assumed zero on input.
 * @param p1    [in/out] Second prime divisor of n, if found, zero otherwise. Assumed zero on input.
 */

void FactorSemiPrimeNearestSquaresBigInt(BigInt& n, BigInt& p1, BigInt& p2)
{
    BigInt root = n.Sqrt();
    root++;

    while (1)
    {
        BigInt diff = root * root - n;

        if (IsSquareBigInt(diff))
        {
            BigInt b = diff.Sqrt();
            p1 = (b * b + n).Sqrt() - b;
            p2 = n / p1;

            return;
        }

        root++;
    }
}

/**
 * GenerateNearestSquareFactors - Given a maximum N, factor each integer M less than or equal to
 * N and get the factors (not necessarily prime) that are closest to the square root of M.
 *
 * @param max       [in]    Get nearest square factors for all numbers m with 0 <= m <= max.
 * @param nsFacs    [in]    List of nearest square factors.
 */

void GenerateNearestSquareFactors(const uint64_t max, vector<NearestSquareFactors>& nsFacs)
{
    uint64_t sqrtStrt = 2;

    nsFacs.push_back({ 0, 0, 0, 0, 0, });
    nsFacs.push_back({ 1, 1, 1, 0, 0, });

    for (uint64_t i = 2; i <= max; i++)
    {
        while (sqrtStrt * sqrtStrt < i)
            sqrtStrt++;

        uint64_t cur = sqrtStrt;

        while ((i % cur) != 0)
            cur--;

        nsFacs.push_back({
            i,
            sqrt(i),
            cur,
            i / cur,
            (log(cur) / log(i)),
            (log(i / cur) / log(i))
            });
    }
}

/**
 * GetNearestSquareFactors - 
 */

TestResult GetNearestSquareFactors()
{
    TestResult res;

    FILE* pFile = fopen("doc/logs/nsfacs.txt", "w");

    if (pFile == nullptr)
        assert(false);

    vector<NearestSquareFactors> nsFacs;
    GenerateNearestSquareFactors(1000000, nsFacs);

    res.caseResults.push_back({ PASS, "" });

    for (uint32_t i = 0; i < nsFacs.size(); i++)
    {
        fprintf(
            pFile,
            "%llu, %3.4f, %llu, %llu, %3.4f, %3.4f,\n",
            nsFacs[i].n,
            nsFacs[i].sqrt,
            nsFacs[i].p1,
            nsFacs[i].p2,
            nsFacs[i].lP1,
            nsFacs[i].lP2
        );
    }

    return res;
}