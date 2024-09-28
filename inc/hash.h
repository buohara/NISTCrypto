#pragma once

#include "commoninc.h"

using namespace std;

#define STATE_W             5
#define STATE_H             5
#define STATE_L             64
#define NUM_SHA3_ROUNDS     24

#define STATE_IDX(x, y, z) (320 * (y) + 64 * (x) + (z))

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
    SHA3(SHASize sz);
    SHA3(SHA3Params &paramsIn);
    uint64_t Size();

    void ClearState();
    void PrintState();

    void Hash(vector<uint8_t>& data, vector<uint8_t>& hashOut);
    void SpongeAbsorbBlock(vector<uint8_t> &block);
    void ApplyKeccak();
    void SpongeSqueezeBlock();

    void Theta();
    void Rho();
    void Pi();
    void Chi();
    void Iota(uint64_t round);

    uint8_t GetBit(uint64_t x, uint64_t y, uint64_t z);
    void SetBit(uint64_t x, uint64_t y, uint64_t z);
    void SetBit(vector<uint8_t>& arrayIn, uint64_t x, uint64_t y, uint64_t z);
    void ClearBit(uint64_t x, uint64_t y, uint64_t z);
    void ClearBit(vector<uint8_t>& arrayIn, uint64_t x, uint64_t y, uint64_t z);

    uint8_t GetRow(const uint64_t y, const uint64_t z);
    uint8_t GetColumn(const uint64_t x, const uint64_t z);
    void GetLane(const uint64_t x, const uint64_t y, vector<uint8_t> &laneOut);

    void SetRow(const uint64_t y, const uint64_t z, uint8_t val);
    void SetColumn(const uint64_t x, const uint64_t z, uint8_t val);
    void SetLane(const uint64_t x, const uint64_t y, vector<uint8_t>& val);
};