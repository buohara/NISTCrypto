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
 * EllipticCurve::Add - Add two points on an elliptic curve.
 *
 * @param   k   [in]    Per message k value.
 *
 * @return Sum of r and s over this curve.
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

            BigInt lambda   = (BigInt(3) * R.x * R.x + params.a) / (BigInt(2) * R.y);
            lambda %= params.q;

            BigInt x        = lambda * lambda - BigInt(2) * R.x;
            x %= params.q;

            BigInt y        = lambda * (R.x - x) - R.y;
            y %= params.q;

            R.x = x;
            R.y = y;
        }
    }

    return R;
}

/**
 * EllipticCurve::GenerateSignature - Generate an ECDSA signature
 * for a given message M and private key d.
 *
 * @param   msg     [in]    Message M to sign.
 * @param   d       [in]    Private key of signer.
 * @param   sz      [in]    SHA size to use when hashing message for signing.
 * 
 * @return  The digital signature pair (r, s) for input message M and private key d.
 */

DigSign EllipticCurve::GenerateSignature(vector<uint8_t>& msg, BigInt d, SHASize sz)
{
    vector<uint8_t> md;
    SHA2 sha;
    sha.Hash(sz, msg, md);

    const uint64_t n = params.n.nBits;
    BigInt e;

    if (n < 8 * md.size())
        md.resize(n / 8);

    e = BigInt(md);

    vector<uint8_t> kData;
    GenKey(n, kData);

    BigInt k(kData);
    BigInt kInv = GetModInverse(k, params.n);

    ECPoint R   = MultiplyBase(k);
    BigInt r    = R.x;
    r           %= params.n;

    BigInt s    = kInv * (e + r * d);
    s           %= params.n;

    return DigSign({ r, s });
}