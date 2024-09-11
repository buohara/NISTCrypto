
#include "primes.h"
#include "bigint.h"

int main(int argc, char** argv)
{
    /*
    Let's factor this next:
    6962155154859963260211100482357357666900094513013513488352858667799199787495340476167566639530574848375895722792291996203873323650274512138128403360943634134259376986501375967452208380337012919869885380406071772232795575963202558402893589313281327208179913789760736615950818685956393838601277519011418885197723428318400763080858914698836058070301404903262955501113318317950597435778777212408626799143
    */

    /*BigInt a("32168516513513", 10);
    printf("BigInt a in decimal: %s\n", a.GetDecimalString().c_str());

    BigInt b("111010100000111010000111111111110111011101001", 2);
    printf("BigInt b in binary: %s\n", b.GetBinaryString().c_str());

    BigInt c("1D41D0FFEEE9", 16);
    printf("BigInt c in hex: %s\n", c.GetHexString().c_str());*/

    srand((unsigned int)time(NULL));

    TestResult res = TestAssignBigInt();
    assert(res.code == PASS);

    res = TestCmpLShiftBigIntCorrect();
    assert(res.code == PASS);

    //res = TestCmpRShiftBigIntCorrect();
    assert(res.code == PASS);

    res = TestCmpAddBigIntCorrect();
    assert(res.code == PASS);

    res = TestCmpSubBigIntCorrect();
    assert(res.code == PASS);

    res = TestCmpMulBigIntCorrect();
    assert(res.code == PASS);

    res = TestCmpDivBigIntCorrect();
    assert(res.code == PASS);

    return 0;
}