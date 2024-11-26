#pragma once

#include "commoninc.h"

#ifdef _WIN32
#include "Windows.h"
#include "bcrypt.h"
#endif

using namespace std;

#define BYTES(x) (((x) / 8) + ((x) % 8 ? 1 : 0))

#define REVERSE_ENDIAN32(x) ((((x) & 0xFF000000UL) >> 24UL) | \
(((x) & 0x00FF0000UL) >> 8UL) | \
(((x) & 0x0000FF00UL) << 8UL) | \
(((x) & 0x000000FFUL) << 24UL))

#define REVERSE_ENDIAN64(x) ((((x) & 0xFF00000000000000ULL) >> 56ULL) | \
(((x) & 0x00FF000000000000ULL) >> 40ULL) | \
(((x) & 0x0000FF0000000000ULL) >> 24ULL) | \
(((x) & 0x000000FF00000000ULL) >> 8ULL) | \
(((x) & 0x00000000FF000000ULL) << 8ULL) | \
(((x) & 0x0000000000FF0000ULL) << 24ULL) | \
(((x) & 0x000000000000FF00ULL) << 40ULL) | \
(((x) & 0x00000000000000FFULL) << 56ULL))

#define BYTE32(x, b) (((x) & (0xFF << (8 * (b)))) >> (8 * (b)))

void StringToHexArray(const string val, vector<uint8_t>& bytes, bool bLittleEndian = true);
void HexArrayToString(const vector<uint8_t>& bytes, string &out, bool bLittleEndian = true);
uint8_t Parity(uint8_t val);
void GenKey(uint64_t bitLen, vector<uint8_t>& keyOut);
void Seive(const uint64_t min, const uint64_t max, vector<uint64_t>& primes);

uint64_t InvModN(const uint64_t k, const uint64_t n);