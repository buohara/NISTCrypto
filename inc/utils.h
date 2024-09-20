#pragma once

#include "commoninc.h"

using namespace std;

#define BYTES(x) (((x) / 8) + ((x) % 8 ? 1 : 0))

void ParseHexString(const string val, vector<uint8_t>& bytes);
void HexToString(const vector<uint8_t>& bytes, string &out);