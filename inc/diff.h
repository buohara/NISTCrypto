#pragma once

#include "commoninc.h"

using namespace std;

struct Diff
{
    vector<uint8_t> d1;
    vector<uint8_t> d2;
    uint64_t offset;

    Diff() : offset(0) {};
};

enum TraceBackDir
{
    UP,
    LEFT,
    DIAG,
    EMPTY
};

struct LCSEntry
{
    TraceBackDir dir;
    uint64_t len;

    LCSEntry() : dir(EMPTY), len(0) {};
    LCSEntry(TraceBackDir dirIn, uint64_t lenIn) : dir(dirIn), len(lenIn) {};
};

void LCS(const vector<uint8_t>& s1, const vector<uint8_t>& s2, vector<uint8_t>& LCS, vector<Diff>& diffs);

void StringToByteVec(const string& s, vector<uint8_t>& b);
void ByteVecToString(const vector<uint8_t>& b, string& s);