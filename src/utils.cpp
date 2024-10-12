#include "utils.h"

static map<char, uint8_t> char2Hex =
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

static map<uint8_t, char> hex2Char =
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

/**
 * StringToHexArray - Convert an integer represented as a hex string to a
 * binary integer stored as an array of bytes.
 *
 * @param val           [in]        String val to convert to an integer.
 * @param bytes         [in/out]    Array of bytes to populate with binary representation of integer.
 * @param bLittleEndian [in]        Should output bytes be stored LSB or MSB first.
 */

void StringToHexArray(const string val, vector<uint8_t>& bytes, bool bLittleEndian)
{
    if (bytes.size() != 0)
        throw invalid_argument("Expected input byte array to be empty when parsing BigInt hex string.");

    string tmp = val;
    if (tmp.length() % 2)
        tmp = "0" + tmp;

    const uint64_t l        = tmp.length();
    const uint64_t sizeOut  = l / 2;
    bytes.resize(sizeOut);
    uint8_t outByte         = 0;

    for (uint64_t i = 0; i < l; i++)
    {
        uint64_t idx = bLittleEndian ? l - i - 1 : i;

        if (char2Hex.count(tmp[i]) == 0)
        {
            throw invalid_argument("Encountered unexpected hex character converting hex string to byte array.");
            bytes.resize(0);
            return;
        }

        if (bLittleEndian)
            outByte |= (i % 2) ? (char2Hex[tmp[idx]] << 4) : char2Hex[tmp[idx]];
        else
            outByte |= (i % 2) ?  char2Hex[tmp[idx]] : (char2Hex[tmp[idx]] << 4);

        if (i % 2)
        {
            bytes[i / 2]    = outByte;
            outByte         = 0;
        }
    }
}

/**
 * HexArrayToString - Convert a hex array to its string equivalent.
 *
 * @param bytes         [in]        Byte array to convert to string.
 * @param out           [in/out]    Output string to fill. Assumed empty on input.
 * @param bLittleEndian [in]        Are input bytes stored LSB or MSB first.
 */

void HexArrayToString(const vector<uint8_t>& bytes, string& out,
    bool bLittleEndian)
{
    if (out.size() != 0)
        throw invalid_argument("Expected output to be empty when converting hex to string.");

    const uint64_t l = 2 * bytes.size();

    out.resize(2 * bytes.size());

    for (uint64_t i = 0; i < bytes.size(); i++)
    {
        char c1 = hex2Char[bytes[i] & 0x0F];
        char c2 = hex2Char[(bytes[i] & 0xF0) >> 4];

        if (bLittleEndian)
        {
            uint64_t outIdx     = l - 2 * i - 1;
            out[outIdx]         = c1;
            out[outIdx - 1]     = c2;
        }
        else
        {
            out[2 * i]          = c2;
            out[2 * i + 1]      = c1;
        }
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
    uint8_t bitCount = 0;

    while (val)
    {
        if (val & 0x1)
            bitCount++;

        val >>= 1;
    }

    return bitCount % 2;
}

/**
 * GenKey - Return a **NON-SECURE** cryptographic key for testing algorithms.
 * This routine uses srand to generate "random" bytes. However, the randonmess
 * of srand is not considered strong enough for security applications.
 *
 * @param bitLen    [in]        Bit length of desired key.
 * @param keyOut    [in/out]    Generated output key.
 */

void GenKey(const uint64_t bitLen, vector<uint8_t>& keyOut)
{
    assert((bitLen) % 8 == 0);
    const uint64_t nBytes = bitLen / 8;
    keyOut.resize(nBytes);

    for (uint64_t i = 0; i < nBytes; i++)
        keyOut[i] = rand() % 256;
}