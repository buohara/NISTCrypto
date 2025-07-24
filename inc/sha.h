#pragma once

#include "commoninc.h"
#include "utils.h"

using namespace std;

#define ROTR32(x, n) (((x) >> (n)) | ((x) << ((32 - n))))
#define ROTR64(x, n) (((x) >> (n)) | ((x) << ((64 - n))))

#define STATE_W             5
#define STATE_H             5
#define STATE_L             64
#define STATE_BYTES         STATE_W * STATE_H * STATE_L / 8
#define NUM_SHA3_ROUNDS     24

#define SHA3_WORD_SZ        64
#define LANE(x, y) (5 * (y) + (x))
#define STATE_IDX(x, y, z) (320 * (y) + 64 * (x) + (z))

struct SHAStreamer
{
    uint64_t r;
    uint64_t offset;

    vector<uint8_t> data;

    SHAStreamer() : r(0), offset(0) {}
    SHAStreamer(uint64_t rIn) : r(rIn), offset(0) {};

    void SetData(vector<uint8_t>& dataIn, bool bLittleEndian = true);
    void Reset();
    void Next(vector<uint64_t>& blockOut);
    bool End();
};

enum SHASize
{
    SHA224,
    SHA256,
    SHA384,
    SHA512
};

enum PrintMode
{
    XY,
    LINEAR
};

struct SHA2
{
    void Hash(SHASize sz, vector<uint8_t>& msg, vector<uint8_t>& md);

private:

    uint32_t ch32(uint32_t x, uint32_t y, uint32_t z);
    uint32_t maj32(uint32_t x, uint32_t y, uint32_t z);
    uint32_t Sig032(uint32_t x);
    uint32_t Sig132(uint32_t x);
    uint32_t sig032(uint32_t x);
    uint32_t sig132(uint32_t x);

    uint64_t ch64(uint64_t x, uint64_t y, uint64_t z);
    uint64_t maj64(uint64_t x, uint64_t y, uint64_t z);
    uint64_t Sig064(uint64_t x);
    uint64_t Sig164(uint64_t x);
    uint64_t sig064(uint64_t x);
    uint64_t sig164(uint64_t x);

    void Hash224(vector<uint8_t>& msg, vector<uint8_t>& md);
    void Hash256(vector<uint8_t>& msg, vector<uint8_t>& md);
    void Hash384(vector<uint8_t>& msg, vector<uint8_t>& md);
    void Hash512(vector<uint8_t>& msg, vector<uint8_t>& md);
};

struct SHA3Params
{
    SHASize sz;

    uint64_t b;
    uint64_t c;
    uint64_t r;
    uint64_t d;
    uint64_t w;
    uint64_t l;
    uint64_t n;

    SHA3Params() : sz(SHA512), b(1600), c(1024),
        r(576), d(512), w(64), l(6), n(24) {}

    SHA3Params(SHASize szIn, uint64_t bIn, uint64_t cIn, uint64_t rIn, 
        uint64_t dIn, uint64_t, uint64_t lIn, uint64_t nIn) : sz(szIn),
        b(bIn), c(cIn), r(rIn), d(dIn), w(dIn), l(lIn), n(nIn) 
    {
        assert(r + c == b);
        assert(c == 2 * d);
        assert(b / 25 == w);
        assert(log2(w) == l);
        
        if (szIn == SHA224)
            assert(d == 224);

        if (szIn == SHA256)
            assert(d == 256);

        if (szIn == SHA384)
            assert(d == 384);

        if (szIn == SHA512)
            assert(d == 512);
    }
};

struct SHA3
{
    SHA3Params params;
    uint64_t state[STATE_W * STATE_H];
    SHAStreamer stream;

    SHA3(SHASize sz);

    void ClearState();
    void PrintState(PrintMode mode = XY);

    void Hash(vector<uint8_t> &msg, vector<uint8_t> &md);
    void SpongeAbsorbBlock(vector<uint64_t> &block);
    void ApplyKeccak();
    void SpongeSqueezeBlock(vector<uint8_t>& md);

    void Theta();
    void Rho();
    void Pi();
    void Chi();
    void Iota(uint64_t round);

    uint64_t GetBit(uint64_t x, uint64_t y, uint64_t z);
    void SetBit(uint64_t x, uint64_t y, uint64_t z, uint64_t bit);
    void SetBit(uint64_t arrayIn[STATE_W * STATE_H], uint64_t x,
        uint64_t y, uint64_t z, uint64_t bit);

private:

    SHA3();
};