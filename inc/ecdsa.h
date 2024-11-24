#pragma once

#include "commoninc.h"
#include "bigint.h"
#include "sha.h"

using namespace std;

enum CurveType
{
    Weierstrass,
    Montgomery
};

enum FieldRep
{
    Prime,
    Binary
};

enum NISTCurve
{
    P192,
    P224,
    P256,
    P384,
    P521,
    K163,
    K233,
    K283,
    K409,
    K571,
    C25519,
    C448,
    ED25519,
    ED448,
    E448
};

struct DPStrings
{
    string q;
    CurveType curveType;
    FieldRep fr;
    string a;
    string b;
    string Gx;
    string Gy;
    string n;
    string h;
};

struct ECPoint
{
    BigInt x;
    BigInt y;

    ECPoint(BigInt xIn, BigInt yIn) : x(xIn), y(yIn) {}

    ECPoint(ECPoint& rhs)
    {
        x = rhs.x;
        y = rhs.y;
    }

    ECPoint(): x(0), y(0) {}

    ECPoint& operator=(const ECPoint& rhs)
    {
        x = rhs.x;
        y = rhs.y;

        return *this;
    }
};

struct DigSign
{
    BigInt r;
    BigInt s;
};

struct DomainParams
{
    BigInt q;
    CurveType curveType;
    FieldRep fr;
    BigInt a;
    BigInt b;
    ECPoint G;
    BigInt n;
    BigInt h;

    DomainParams(DPStrings &dpStrings);
};

struct EllipticCurve
{
    DomainParams params;
    ECPoint G;

    ECPoint MultiplyBase(BigInt k);
    ECPoint Add(ECPoint r, ECPoint s);
    
    DigSign GenerateSignature(
        vector<uint8_t> &msg, 
        BigInt d,
        SHASize sz
    );
};