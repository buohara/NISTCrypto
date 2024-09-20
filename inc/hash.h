#pragma once

#include "commoninc.h"

using namespace std;

enum SHASize
{
    SHA224,
    SHA256,
    SHA384,
    SHA512
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
        uint64_t dIn, uint64_t wIn, uint64_t lIn, uint64_t nIn) : sz(szIn),
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

struct MsgStreamer
{
    uint64_t r;
    uint64_t b;
    uint64_t offset;

    vector<uint8_t> data;
    MsgStreamer() : r(576), b(1600), offset(0) {};

    MsgStreamer(uint64_t rIn, uint64_t bIn) :
        r(rIn), b(bIn), offset(0) {};

    void SetData(vector<uint8_t>& dataIn);
    void Reset();
    void Next(vector<uint8_t>& blockOut);
    bool End();
};

struct SHA3
{
    SHA3Params params;
    vector<uint8_t> state;

    MsgStreamer stream;

    SHA3();
    SHA3(SHA3Params &paramsIn);

    void SpongeAbsorbBlock(vector<uint8_t> &block);
    void ApplyKeccak();
    void SpongeSqueezeBlock();

    void Theta();
    void Rho();
    void Pi();
    void Chi();
    void Iota(uint64_t round);

    void Hash(vector<uint8_t>& data, vector<uint8_t> &hashOut);
    uint8_t GetIdx(uint64_t x, uint64_t y, uint64_t z);
    void SetIdx(uint64_t x, uint64_t y, uint64_t z, uint8_t val);
    void SetIdx(vector<uint8_t>& arrayIn, uint64_t x, uint64_t y, uint64_t z, uint8_t val);
};