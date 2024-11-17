#include "ecdsa.h"

map<NISTCurve, DPStrings> curveDomainParams =
{
    //{ P192, {}},
    {
        P224,
        {
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000001",
            Weierstrass,
            Prime,
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFE",
            "B4050A850C04B3ABF54132565044B0B7D7BFD8BA270B39432355FFB4",
            "B70E0cbd6bb4bf7f321390b94a03c1d356c21122343280d6115c1d21",
            "bd376388b5f723fb4c22dfe6cd4375a05a07476444d5819985007e34",
            "ffffffffffffffffffffffffffff16a2e0b8f03e13dd29455c5c2a3d",
            "1"
        }
    },
    //{ P256, {}},
    //{ P384, {}},
    //{ P521, {}},
    //{ K163, {}},
    //{ K233, {}},
    //{ K283, {}},
    //{ K409, {}},
    //{ K571, {}},
    //{ C25519, {}},
    //{ C448, {}},
    //{ ED25519, {}},
    //{ ED448, {}},
    //{ E448, {}}
};

/**
 * DomainParams - Elliptic curve domain paramater constructor.
 *
 * @param   dpStrings   [in] List of domain parameter strings.
 */

DomainParams::DomainParams(DPStrings &dpStrings) :
    q(dpStrings.q, 16),
    curveType(dpStrings.curveType),
    fr(dpStrings.fr),
    a(dpStrings.a, 16),
    b(dpStrings.b, 16),
    G{ { dpStrings.Gx, 16 }, { dpStrings.Gy, 16 } },
    n(dpStrings.n, 16),
    h(dpStrings.h, 16) 
{
}

/**
 * EllipticCurve::Add - Add two points on an elliptic curve.
 *
 * @param   k   [in]    Per message k value.
 *
 * @return R = kG.
 */

ECPoint EllipticCurve::Add(ECPoint r, ECPoint s)
{
    assert(r.x != s.x);
    assert(r.y != s.y);

    BigInt lambda = (s.y - r.y) / (s.x - r.x);
    lambda %= params.q;

    ECPoint res;

    res.x   = lambda * lambda - r.x - r.y;
    res.x   %= params.q;
    
    res.y   = lambda * (r.x - res.x) - r.y;
    res.y   %= params.q;

    return res;
}

/**
 * EllipticCurve::MultiplyBase - For ECDSA, a per-message random number k
 * is generated. Compute R = kG, where G is the base point of the curve.
 *
 * @param   k   [in]    Per message k value.
 *
 * @return R = kG.
 */

ECPoint EllipticCurve::MultiplyBase(BigInt k)
{
    ECPoint R = G;
    ECPoint res;

    for (uint32_t i = 0; i < k.data.size(); i++)
    {
        uint8_t tmp = k.data[i];

        while (tmp)
        {
            if (tmp & 0x1)
            {
                res.x += R.x;
                res.y += R.y;
            }

            BigInt lambda = (BigInt(3) * R.x * R.x + params.a) / (BigInt(2) * R.y);
            lambda %= params.q;

            BigInt x = lambda * lambda - BigInt(2) * R.x;
            x %= params.q;

            BigInt y = lambda * (R.x - x) - R.y;
            y %= params.q;

            R.x = x;
            R.y = y;
        }
    }

    return R;
}