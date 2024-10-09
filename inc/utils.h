#pragma once

#include "commoninc.h"

using namespace std;

#define BYTES(x) (((x) / 8) + ((x) % 8 ? 1 : 0))
#define REVERSE_ENDIAN32(x) (((x) & 0xFF000000 >> 24) | ((x) & 0x00FF0000 >> 8) | ((x) & 0x0000FF00 << 8) | ((x) & 0x000000FF << 24))
#define BYTE32(x, b) (((x) & (0xFF << (8 * (b)))) >> (8 * (b)))

void StringToHexArray(const string val, vector<uint8_t>& bytes, bool bLittleEndian = true);
void HexArrayToString(const vector<uint8_t>& bytes, string &out, bool bLittleEndian = true);
uint8_t Parity(uint8_t val);
void GenKey(uint64_t bitLen, vector<uint8_t>& keyOut);