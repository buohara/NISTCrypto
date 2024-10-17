#pragma once

#include "commoninc.h"
#include "utils.h"

using namespace std;

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

struct AESStreamer
{
    uint32_t offset;
    uint32_t r;
    vector<uint8_t> data;

    AESStreamer() : offset(0), r(128) {};

    void SetData(const vector<uint8_t>& dataIn, bool bLittleEndian = true);
    void SetBitRate(const uint32_t rateIn);
    void Reset();
    void Next(uint32_t block[4]);
    bool End();
};