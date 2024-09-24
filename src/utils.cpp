#include "utils.h"

/**
 * StringToHexArray - Convert an integer represented as a hex string to a
 * binary integer stored as an array of bytes.
 *
 * @param val       [in] String val to convert to an integer.
 * @param bytes     [in/out] Array of bytes to populate with binary representation of integer.
 */

void StringToHexArray(const string val, vector<uint8_t>& bytes)
{
    if (bytes.size() != 0)
        throw invalid_argument("Expected input byte array to be empty when parsing BigInt hex string.");

    map<char, uint8_t> hexCharVals =
    {
        { '0', 0x0 },
        { '1', 0x1 },
        { '2', 0x2 },
        { '3', 0x3 },
        { '4', 0x4 },
        { '5', 0x5 },
        { '6', 0x6 },
        { '7', 0x7 },
        { '8', 0x8 },
        { '9', 0x9 },
        { 'A', 0xA },
        { 'B', 0xB },
        { 'C', 0xC },
        { 'D', 0xD },
        { 'E', 0xE },
        { 'F', 0xF },
        { 'a', 0xA },
        { 'b', 0xB },
        { 'c', 0xC },
        { 'd', 0xD },
        { 'e', 0xE },
        { 'f', 0xF }
    };

    const uint64_t l        = val.length();
    const uint64_t sizeOut  = (l + 1) / 2;
    bytes.resize(sizeOut);
    bool bHi                = false;
    const uint8_t bOdd      = l % 2;

    for (uint64_t i = l; i-- > 0;)
    {
        if (hexCharVals.count(val[i]) == 0)
        {
            throw invalid_argument("Encountered unexpected hex character converting hex string to byte array.");
            bytes.resize(0);
            return;
        }

        uint64_t outPlace   = (i + bOdd) / 2;
        uint64_t outIdx     = sizeOut - outPlace - 1;
        bytes[outIdx]       |= (bHi) ? hexCharVals[val[i]] << 4 : hexCharVals[val[i]];
        bHi                 = !bHi;
    }
}

/**
 * HexArrayToString - Convert a hex array to its string equivalent.
 *
 * @param bytes     [in]        Byte array to convert to string.
 * @param out       [in/out]    Output string to fill. Assumed empty on input.
 */

void HexArrayToString(const vector<uint8_t>& bytes, string& out)
{
    if (out.size() != 0)
        throw invalid_argument("Expected output to be empty when converting hex to string.");

    map<uint8_t, char> hexCharVals =
    {
        { 0x0, '0' },
        { 0x1, '1' },
        { 0x2, '2' },
        { 0x3, '3' },
        { 0x4, '4' },
        { 0x5, '5' },
        { 0x6, '6' },
        { 0x7, '7' },
        { 0x8, '8' },
        { 0x9, '9' },
        { 0xA, 'A' },
        { 0xB, 'B' },
        { 0xC, 'C' },
        { 0xD, 'D' },
        { 0xE, 'E' },
        { 0xF, 'F' }
    };

    bool bHi = false;

    for (uint64_t i = 0; i < bytes.size(); i++)
    {
        out = hexCharVals[(bHi ? ((bytes[i] >> 4) & 0xF) : bytes[i] & 0xF)] + out;
        bHi = !bHi;
    }
}

/**
 * Parity - Return 1 if an odd number of bits are set, zero otherwise.
 *
 * @param val   [in] Input val to check parity.
 * 
 * @return Parity of input val.
 */

uint8_t Parity(uint8_t val)
{
    bool parity = false;

    while (val)
    {
        parity 
    }
}