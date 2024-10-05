#pragma once

#include "commoninc.h"

using namespace std;

struct SHAStreamer
{
    uint64_t r;
    uint64_t offset;

    vector<uint8_t> data;

    SHAStreamer() : r(0), offset(0) {}
    SHAStreamer(uint64_t rIn) : r(rIn), offset(0) {};

    void SetData(vector<uint8_t>& dataIn);
    void Reset();
    void Next(vector<uint64_t>& blockOut);
    bool End();
};

struct AESStreamer
{
    uint32_t offset;
    vector<uint8_t> data;

    AESStreamer() : offset(0) {};

    void SetData(vector<uint8_t>& dataIn);
    void Reset();
    void Next(uint32_t block[4]);
    bool End();
};