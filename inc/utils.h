#pragma once

#include "commoninc.h"

using namespace std;

#define BYTES(x) (((x) / 8) + ((x) % 8 ? 1 : 0))

void StringToHexArray(const string val, vector<uint8_t>& bytes);
void HexArrayToString(const vector<uint8_t>& bytes, string &out);