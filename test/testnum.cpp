#include "test.h"
#include "primes.h"

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
            nsFacs[i].lP1 ,
            nsFacs[i].lP2
        );
    }

    return res;
}