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