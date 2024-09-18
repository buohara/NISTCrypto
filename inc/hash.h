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

    void Theta(vector<uint8_t> &block);
    void Rho(vector<uint8_t> &block);
    void Pi(vector<uint8_t> &block);
    void Chi(vector<uint8_t> &block);
    void Iota(vector<uint8_t> &block);

    void Hash(vector<uint8_t>& data, vector<uint8_t> &hashOut);
};