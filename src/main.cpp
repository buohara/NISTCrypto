#include "primes.h"
#include "bigint.h"
#include "diff.h"

int main(int argc, char** argv)
{
    /*
    Let's factor this next:
    6962155154859963260211100482357357666900094513013513488352858667799199787495340476167566639530574848375895722792291996203873323650274512138128403360943634134259376986501375967452208380337012919869885380406071772232795575963202558402893589313281327208179913789760736615950818685956393838601277519011418885197723428318400763080858914698836058070301404903262955501113318317950597435778777212408626799143
    */

    string m1 = "Once upon a time, in a galaxy far, far away.";
    vector<uint8_t> s1;
    StringToByteVec(m1, s1);
    
    string m2 = "Once upon a time, in a magnificent galaxy far away.";
    vector<uint8_t> s2;
    StringToByteVec(m2, s2);

    vector<uint8_t> lcs;
    vector<Diff> diffs;

    LCS(s1, s2, lcs, diffs);

    string lcsStr;
    ByteVecToString(lcs, lcsStr);

    printf("LCS = %s\n\n", lcsStr.c_str());

    for (uint64_t i = 0; i < diffs.size(); i++)
    {
        string d1Str;
        string d2Str;

        ByteVecToString(diffs[i].d1, d1Str);
        ByteVecToString(diffs[i].d2, d2Str);

        printf("LCS diff %llu at LCS[%llu]: d1 = '%s', d2 = '%s'\n",
            i, diffs[i].offset, d1Str.c_str(), d2Str.c_str());
    }

    return 0;
}