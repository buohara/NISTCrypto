#include "encrypt.h"

static const uint32_t rcs[11] =
{
    0x00000000,
    0x01000000,
    0x02000000,
    0x04000000,
    0x08000000,
    0x10000000,
    0x20000000,
    0x40000000,
    0x80000000,
    0x1b000000,
    0x36000000,
};

static const uint32_t sbox[16][16] =
{
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static const uint32_t sboxInv[16][16] =
{
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

/**
 * GFMult - Multiply two 8-bit numbers as elements of GF(2^8) (see FIPS 197). Multiply
 * the two 8-bit values as polynomials mod 2, then reduce by the irreducible/prime polynomial 
 * 0b100011011. This follows the iterative XTimes method outlined in FIPS 197.
 *
 * @param a [in] First factor.
 * @param b [in] Second factor.
 * 
 * @return Product of a and b over GF(2^8).
 */

static uint8_t GFMult(uint8_t a, uint8_t b)
{
    uint8_t tmpA        = a;
    uint8_t tmpB        = b;
    const uint8_t bitHi = 0x80;
    const uint8_t mod   = 0x1B;
    uint8_t xTimes[8]   = { 0 };
    uint8_t e           = 0;
    uint8_t out         = 0;
    xTimes[0]           = tmpA;

    for (uint8_t i = 1; i < 8; i++)
    {
        if (tmpA & bitHi)
            tmpA = (tmpA << 1) ^ mod;
        else
            tmpA <<= 1;

        xTimes[i] = tmpA;
    }

    while (tmpB)
    {
        if (tmpB & 0x1)
            out ^= xTimes[e];

        e++;
        tmpB >>= 1;
    }

    return out;
}

/**
 * AESStreamer::AESStreamer - Constructor. Initialize offset and bitrate based on
 * AES mode.
 *
 * @param modeIn    [in] Input AES mode.
 */

AESStreamer::AESStreamer(AESMode modeIn) : bitOffset(0), mode(modeIn)
{
    switch (mode)
    {
        case CFB1:

            r = 1;
            break;

        case CFB8:

            r = 8;
            break;

        case CFB128:
        case OFB:
        case CBC:
        case ECB:
        default:

            r = 128;
            break;
    }
}

/**
 * AESStreamer::SetData - Set message data in the AES streamer. Pad data according
 * to the bitrate set for the current AES mode.
 *
 * @param dataIn            [in] Message data to encrypt.
 * @param bLittleEndian     [in] True if input data is in little endian order.
 */

void AESStreamer::SetData(const vector<uint8_t>& dataIn, bool bLittleEndian)
{
    const uint32_t rateBytes    = (r <= 8) ? 1 : 16;
    const uint32_t sizeIn       = dataIn.size();
    const uint8_t padBytes      = (uint8_t)(sizeIn % rateBytes);

    if (sizeIn)
    {
        data.resize(sizeIn + padBytes);
        memcpy(&data[0], &dataIn[0], sizeIn);

        uint32_t curVal = 0;

        if (mode == ECB || mode == CBC)
        {
            for (uint32_t i = 0; i < sizeIn; i += 4)
            {
                uint8_t tmp = data[i];
                data[i]     = data[i + 3];
                data[i + 3] = tmp;

                tmp         = data[i + 1];
                data[i + 1] = data[i + 2];
                data[i + 2] = tmp;
            }
        }
    }

    for (uint32_t i = 0; i < padBytes; i++)
        data[sizeIn + i] = padBytes;

    Reset();
}

/**
 * AESStreamer::Reset - Set the streamer back to the beginning of the message.
 */

void AESStreamer::Reset()
{
    bitOffset = 0;
}

/**
 * AESStreamer::Next - Get the next r bits from the message stream.
 */

void AESStreamer::Next(uint32_t block[4])
{
    assert((bitOffset + r) <= (8 * data.size()));

    uint32_t byte   = 0;
    uint32_t mask   = 0;
    uint32_t shift  = 0;
    uint32_t bit    = 0;

    switch (r)
    {
    case 1:

        byte        = bitOffset / 8;
        shift       = bitOffset % 8;
        mask        = 1LU << shift;
        bit         = (data[byte] & mask) >> shift;
        block[0]    = bit;

        break;

    case 8:

        block[0] = data[bitOffset / 8];
        break;

    case 128:

        memcpy(&block[0], &data[bitOffset / 8], 16);
        break;
    }

    bitOffset += r;
}

/**
 * AESStreamer::End - Return true if input all message blocks have been
 * processed.
 *
 * @return True if at the end of the message, false otherwise.
 */

bool AESStreamer::End()
{
    if (bitOffset == 8 * data.size())
        return true;

    return false;
}

/**
 * AES - Constructor. Takes an AES key size and sets number of rounds and
 * key expansion size.
 * 
 * @param sz [in]   AES key size to use.
 */

AES::AES(AESSize sz, AESMode modeIn) : w{0}, mode(modeIn), stream(modeIn)
{
    switch (sz)
    {
        case AES128:

            nr = 10;
            nk = 4;
            break;

        case AES192:

            nr = 12;
            nk = 6;
            break;

        case AES256:
        default :

            nr = 14;
            nk = 8;
            break;
    }
}

/**
 * AES::ClearState - Reset the AES state to zeros.
 */

void AES::ClearState()
{
    state[0] = 0;
    state[1] = 0;
    state[2] = 0;
    state[3] = 0;
}

/**
 * AES::PrintState - Print the state array for debugging
 * and validation.
 */

void AES::PrintState()
{
    printf("r[0]: ");
    printf("%02x ",     (state[0] & 0x000000FF) >> 0);
    printf("%02x ",     (state[0] & 0x0000FF00) >> 8);
    printf("%02x ",     (state[0] & 0x00FF0000) >> 16);
    printf("%02x\n",    (state[0] & 0xFF000000) >> 24);
    
    printf("r[1]: ");
    printf("%02x ",     (state[1] & 0x000000FF) >> 0);
    printf("%02x ",     (state[1] & 0x0000FF00) >> 8);
    printf("%02x ",     (state[1] & 0x00FF0000) >> 16);
    printf("%02x\n",    (state[1] & 0xFF000000) >> 24);

    printf("r[2]: ");
    printf("%02x ",     (state[2] & 0x000000FF) >> 0);
    printf("%02x ",     (state[2] & 0x0000FF00) >> 8);
    printf("%02x ",     (state[2] & 0x00FF0000) >> 16);
    printf("%02x\n",    (state[2] & 0xFF000000) >> 24);

    printf("r[3]: ");
    printf("%02x ",     (state[3] & 0x000000FF) >> 0);
    printf("%02x ",     (state[3] & 0x0000FF00) >> 8);
    printf("%02x ",     (state[3] & 0x00FF0000) >> 16);
    printf("%02x\n\n",  (state[3] & 0xFF000000) >> 24);
}

/**
 * SBox - Given an input 32-bit value, replace its bytes using
 * a lookup table.
 * 
 * @param val   [in]    Value whose bytes are to be substituted. 
 */

inline static uint32_t SBox(uint32_t val)
{
    uint32_t tmp = 0;

    tmp |= sbox[(val & 0x000000F0) >>  4][(val & 0x0000000F)];
    tmp |= sbox[(val & 0x0000F000) >> 12][(val & 0x00000F00) >> 8] << 8;
    tmp |= sbox[(val & 0x00F00000) >> 20][(val & 0x000F0000) >> 16] << 16;
    tmp |= sbox[(val & 0xF0000000) >> 28][(val & 0x0F000000) >> 24] << 24;

    return tmp;
}

/**
 * RotLeft32 - Rotate bytes in a 32-bit integer by specified number
 * of bytes.
 *
 * @param val   [in]    Value to be rotated.
 * @param n     [in]    Number of bytes to rotate.
 */

inline static uint32_t RotLeft32(uint32_t val, uint32_t n)
{
    assert(n < 4);

    if (n == 0)
        return val;

    const uint32_t shift    = 8 * n;
    return ((val << shift) | (val >> (32 - shift)));
}

/**
 * AES::SubBytes - AES round SubBytes transformation. Replace bytes in state
 * array using lookup table.
 */

void AES::SubBytes()
{
    state[0] = SBox(state[0]);
    state[1] = SBox(state[1]);
    state[2] = SBox(state[2]);
    state[3] = SBox(state[3]);
}

/**
 * AES::ShiftRows - AES round ShiftRows transformation. Rotate
 * row bytes in the state.
 */

void AES::ShiftRows()
{
    uint32_t tmp[4];

    tmp[0] = (state[0] & 0xFF000000) | (state[1] & 0x00FF0000) | (state[2] & 0x0000FF00) | (state[3] & 0x000000FF);
    tmp[1] = (state[1] & 0xFF000000) | (state[2] & 0x00FF0000) | (state[3] & 0x0000FF00) | (state[0] & 0x000000FF);
    tmp[2] = (state[2] & 0xFF000000) | (state[3] & 0x00FF0000) | (state[0] & 0x0000FF00) | (state[1] & 0x000000FF);
    tmp[3] = (state[3] & 0xFF000000) | (state[0] & 0x00FF0000) | (state[1] & 0x0000FF00) | (state[2] & 0x000000FF);

    state[0] = tmp[0];
    state[1] = tmp[1];
    state[2] = tmp[2];
    state[3] = tmp[3];
}

/**
 * AES::MixColumns - AES round MixColumns tranformation. Multiply
 * columns in the state by a fixed matrix.
 */

void AES::MixColumns()
{
    uint32_t b1 =   GFMult(0x02, BYTE32(state[0], 3)) ^ GFMult(0x03, BYTE32(state[0], 2)) ^ 
                    BYTE32(state[0], 1) ^ BYTE32(state[0], 0);

    uint32_t b2 =   BYTE32(state[0], 3) ^ GFMult(0x02, BYTE32(state[0], 2)) ^ 
                    GFMult(0x03, BYTE32(state[0], 1)) ^ BYTE32(state[0], 0);

    uint32_t b3 =   BYTE32(state[0], 3) ^ BYTE32(state[0], 2) ^
                    GFMult(0x02, BYTE32(state[0], 1)) ^ GFMult(0x03, BYTE32(state[0], 0));

    uint32_t b4 =   GFMult(0x03, BYTE32(state[0], 3)) ^ BYTE32(state[0], 2) ^ 
                    BYTE32(state[0], 1) ^ GFMult(0x02, BYTE32(state[0], 0));

    state[0] = b1 << 24 | b2 << 16 | b3 << 8 | b4;


    b1 =            GFMult(0x02, BYTE32(state[1], 3)) ^ GFMult(0x03, BYTE32(state[1], 2)) ^
                    BYTE32(state[1], 1) ^ BYTE32(state[1], 0);

    b2 =            BYTE32(state[1], 3) ^ GFMult(0x02, BYTE32(state[1], 2)) ^
                    GFMult(0x03, BYTE32(state[1], 1)) ^ BYTE32(state[1], 0);

    b3 =            BYTE32(state[1], 3) ^ BYTE32(state[1], 2) ^
                    GFMult(0x02, BYTE32(state[1], 1)) ^ GFMult(0x03, BYTE32(state[1], 0));

    b4 =            GFMult(0x03, BYTE32(state[1], 3)) ^ BYTE32(state[1], 2) ^
                    BYTE32(state[1], 1) ^ GFMult(0x02, BYTE32(state[1], 0));

    state[1] = b1 << 24 | b2 << 16 | b3 << 8 | b4;


    b1 =            GFMult(0x02, BYTE32(state[2], 3)) ^ GFMult(0x03, BYTE32(state[2], 2)) ^
                    BYTE32(state[2], 1) ^ BYTE32(state[2], 0);

    b2 =            BYTE32(state[2], 3) ^ GFMult(0x02, BYTE32(state[2], 2)) ^
                    GFMult(0x03, BYTE32(state[2], 1)) ^ BYTE32(state[2], 0);

    b3 =            BYTE32(state[2], 3) ^ BYTE32(state[2], 2) ^
                    GFMult(0x02, BYTE32(state[2], 1)) ^ GFMult(0x03, BYTE32(state[2], 0));

    b4 =            GFMult(0x03, BYTE32(state[2], 3)) ^ BYTE32(state[2], 2) ^
                    BYTE32(state[2], 1) ^ GFMult(0x02, BYTE32(state[2], 0));

    state[2] = b1 << 24 | b2 << 16 | b3 << 8 | b4;


    b1 =            GFMult(0x02, BYTE32(state[3], 3)) ^ GFMult(0x03, BYTE32(state[3], 2)) ^
                    BYTE32(state[3], 1) ^ BYTE32(state[3], 0);

    b2 =            BYTE32(state[3], 3) ^ GFMult(0x02, BYTE32(state[3], 2)) ^
                    GFMult(0x03, BYTE32(state[3], 1)) ^ BYTE32(state[3], 0);

    b3 =            BYTE32(state[3], 3) ^ BYTE32(state[3], 2) ^
                    GFMult(0x02, BYTE32(state[3], 1)) ^ GFMult(0x03, BYTE32(state[3], 0));

    b4 =            GFMult(0x03, BYTE32(state[3], 3)) ^ BYTE32(state[3], 2) ^
                    BYTE32(state[3], 1) ^ GFMult(0x02, BYTE32(state[3], 0));

    state[3] = b1 << 24 | b2 << 16 | b3 << 8 | b4;
}

/**
 * AES::AddRoundKey - XOR this rounds key value into the state array.
 * 
 * @param round [in]    Current AES round number.
 */

void AES::AddRoundKey(const uint32_t round)
{
    uint32_t offset = 4 * round;

    state[0] ^= w[offset];
    state[1] ^= w[offset + 1];
    state[2] ^= w[offset + 2];
    state[3] ^= w[offset + 3];
}

/**
 * SBoxInv - Given an input 32-bit value, replace its bytes using
 * an inverse lookup table.
 *
 * @param val   [in]    Value whose bytes are to be substituted.
 */

inline static uint32_t SBoxInv(uint32_t val)
{
    uint32_t tmp = 0;

    tmp |= sboxInv[(val & 0x000000F0) >> 4][(val & 0x0000000F)];
    tmp |= sboxInv[(val & 0x0000F000) >> 12][(val & 0x00000F00) >> 8] << 8;
    tmp |= sboxInv[(val & 0x00F00000) >> 20][(val & 0x000F0000) >> 16] << 16;
    tmp |= sboxInv[(val & 0xF0000000) >> 28][(val & 0x0F000000) >> 24] << 24;

    return tmp;
}

/**
 * AES::InvSubBytes - AES inverse round InvSubBytes. Do an inverse byte
 * substituation using a lookup table.
 */

void AES::InvSubBytes()
{
    state[0] = SBoxInv(state[0]);
    state[1] = SBoxInv(state[1]);
    state[2] = SBoxInv(state[2]);
    state[3] = SBoxInv(state[3]);
}

/**
 * AES::InvShiftRows - AES inverse round InvShiftRows. Shift AES state
 * rows back from the forward transformation.
 */

void AES::InvShiftRows()
{
    uint32_t tmp[4];

    tmp[0] = (state[0] & 0xFF000000) | (state[1] & 0x000000FF) | (state[2] & 0x0000FF00) | (state[3] & 0x00FF0000);
    tmp[1] = (state[1] & 0xFF000000) | (state[2] & 0x000000FF) | (state[3] & 0x0000FF00) | (state[0] & 0x00FF0000);
    tmp[2] = (state[2] & 0xFF000000) | (state[3] & 0x000000FF) | (state[0] & 0x0000FF00) | (state[1] & 0x00FF0000);
    tmp[3] = (state[3] & 0xFF000000) | (state[0] & 0x000000FF) | (state[1] & 0x0000FF00) | (state[2] & 0x00FF0000);

    state[0] = tmp[0];
    state[1] = tmp[1];
    state[2] = tmp[2];
    state[3] = tmp[3];
}

/**
 * AES::SetIV - Set an initialization vector for
 * CBC, CFB, and OFB modes.
 */

void AES::SetIV(const vector<uint32_t>& ivIn)
{
    iv = ivIn;
}

/**
 * AES::RotateIVLeft - For CFB mode, plaintext/ciphertext are processed s bits
 * at a time, with bits from prior input blocks rotated left s bits each
 * iteration.
 * 
 * @param s [in]    Number of bits to rotate (support 1, 8, or 128 only).
 */

void AES::RotateIVLeft(const uint32_t s)
{
    assert(s == 1 || s == 8 || s == 128);

    if (s == 1)
    {
        uint32_t hi1    = ((iv[3] & 0x80000000) >> 31);
        uint32_t hi2    = 0;
        iv[3]           <<= 1;

        hi2     = ((iv[2] & 0x80000000) >> 31);
        iv[2]   <<= 1;
        iv[2]   |= hi1;
        hi1     = hi2;

        hi2     = ((iv[1] & 0x80000000) >> 31);
        iv[1]   <<= 1;
        iv[1]   |= hi1;
        hi1     = hi2;

        iv[0]   <<= 1;
        iv[0]   |= hi1;
    }
    else if (s == 8)
    {
        uint32_t hi1    = ((iv[3] & 0xFF000000) >> 24);
        uint32_t hi2    = 0;
        iv[3]           <<= 8;

        hi2     = ((iv[2] & 0xFF000000) >> 24);
        iv[2]   <<= 8;
        iv[2]   |= hi1;
        hi1     = hi2;

        hi2     = ((iv[1] & 0xFF000000) >> 24);
        iv[1]   <<= 8;
        iv[1]   |= hi1;
        hi1     = hi2;

        iv[0]   <<= 8;
        iv[0]   |= hi1;
    }
    else if (s == 128)
    {
        iv[3] = 0;
        iv[2] = 0;
        iv[1] = 0;
        iv[2] = 0;
    }
}

/**
 * AES::InvMixColumns - AES inverse round InvMixColumns. Apply an inverse
 * mix columns matrix.
 */

void AES::InvMixColumns()
{
    uint32_t b1 =   GFMult(0x0E, BYTE32(state[0], 3)) ^ GFMult(0x0B, BYTE32(state[0], 2)) ^
                    GFMult(0x0D, BYTE32(state[0], 1)) ^ GFMult(0x09, BYTE32(state[0], 0));

    uint32_t b2 =   GFMult(0x09, BYTE32(state[0], 3)) ^ GFMult(0x0E, BYTE32(state[0], 2)) ^
                    GFMult(0x0B, BYTE32(state[0], 1)) ^ GFMult(0x0D, BYTE32(state[0], 0));

    uint32_t b3 =   GFMult(0x0D, BYTE32(state[0], 3)) ^ GFMult(0x09, BYTE32(state[0], 2)) ^
                    GFMult(0x0E, BYTE32(state[0], 1)) ^ GFMult(0x0B, BYTE32(state[0], 0));

    uint32_t b4 =   GFMult(0x0B, BYTE32(state[0], 3)) ^ GFMult(0x0D, BYTE32(state[0], 2)) ^
                    GFMult(0x09, BYTE32(state[0], 1)) ^ GFMult(0x0E, BYTE32(state[0], 0));

    state[0] = b1 << 24 | b2 << 16 | b3 << 8 | b4;


    b1 =            GFMult(0x0E, BYTE32(state[1], 3)) ^ GFMult(0x0B, BYTE32(state[1], 2)) ^
                    GFMult(0x0D, BYTE32(state[1], 1)) ^ GFMult(0x09, BYTE32(state[1], 0));

    b2 =            GFMult(0x09, BYTE32(state[1], 3)) ^ GFMult(0x0E, BYTE32(state[1], 2)) ^
                    GFMult(0x0B, BYTE32(state[1], 1)) ^ GFMult(0x0D, BYTE32(state[1], 0));

    b3 =            GFMult(0x0D, BYTE32(state[1], 3)) ^ GFMult(0x09, BYTE32(state[1], 2)) ^
                    GFMult(0x0E, BYTE32(state[1], 1)) ^ GFMult(0x0B, BYTE32(state[1], 0));

    b4 =            GFMult(0x0B, BYTE32(state[1], 3)) ^ GFMult(0x0D, BYTE32(state[1], 2)) ^
                    GFMult(0x09, BYTE32(state[1], 1)) ^ GFMult(0x0E, BYTE32(state[1], 0));

    state[1] = b1 << 24 | b2 << 16 | b3 << 8 | b4;


    b1 =            GFMult(0x0E, BYTE32(state[2], 3)) ^ GFMult(0x0B, BYTE32(state[2], 2)) ^
                    GFMult(0x0D, BYTE32(state[2], 1)) ^ GFMult(0x09, BYTE32(state[2], 0));

    b2 =            GFMult(0x09, BYTE32(state[2], 3)) ^ GFMult(0x0E, BYTE32(state[2], 2)) ^
                    GFMult(0x0B, BYTE32(state[2], 1)) ^ GFMult(0x0D, BYTE32(state[2], 0));

    b3 =            GFMult(0x0D, BYTE32(state[2], 3)) ^ GFMult(0x09, BYTE32(state[2], 2)) ^
                    GFMult(0x0E, BYTE32(state[2], 1)) ^ GFMult(0x0B, BYTE32(state[2], 0));

    b4 =            GFMult(0x0B, BYTE32(state[2], 3)) ^ GFMult(0x0D, BYTE32(state[2], 2)) ^
                    GFMult(0x09, BYTE32(state[2], 1)) ^ GFMult(0x0E, BYTE32(state[2], 0));

    state[2] = b1 << 24 | b2 << 16 | b3 << 8 | b4;


    b1 =            GFMult(0x0E, BYTE32(state[3], 3)) ^ GFMult(0x0B, BYTE32(state[3], 2)) ^
                    GFMult(0x0D, BYTE32(state[3], 1)) ^ GFMult(0x09, BYTE32(state[3], 0));

    b2 =            GFMult(0x09, BYTE32(state[3], 3)) ^ GFMult(0x0E, BYTE32(state[3], 2)) ^
                    GFMult(0x0B, BYTE32(state[3], 1)) ^ GFMult(0x0D, BYTE32(state[3], 0));

    b3 =            GFMult(0x0D, BYTE32(state[3], 3)) ^ GFMult(0x09, BYTE32(state[3], 2)) ^
                    GFMult(0x0E, BYTE32(state[3], 1)) ^ GFMult(0x0B, BYTE32(state[3], 0));

    b4 =            GFMult(0x0B, BYTE32(state[3], 3)) ^ GFMult(0x0D, BYTE32(state[3], 2)) ^
                    GFMult(0x09, BYTE32(state[3], 1)) ^ GFMult(0x0E, BYTE32(state[3], 0));

    state[3] = b1 << 24 | b2 << 16 | b3 << 8 | b4;
}

/**
 * AES::ExpandKey - Expand an input encryption key into a schedule of
 * round keys for AES rounds.
 *
 * @param key   [in]    Cryptographic key to expand.
 */

void AES::ExpandKey(const vector<uint32_t>& key)
{
    assert(key.size() == nk);
    memcpy(w, &key[0], 4 * key.size());

    for (uint32_t i = nk; i <= 4 * nr + 3; i++)
    {
        uint32_t tmp = w[i - 1];

        if ((i % nk) == 0)
        {
            tmp = RotLeft32(tmp, 1);
            tmp = SBox(tmp);
            uint32_t rc = rcs[i / nk];
            tmp ^= rcs[i / nk];
        }
        else if ((nk == 8) && (i % nk == 4))
        {
            tmp = SBox(tmp);
        }

        w[i] = w[i - nk] ^ tmp;
    }
}

/**
 * AES::Encrypt - AES encryption entry. Route to the appropriate encryption routine based on
 * AES mode.
 *
 * @param plainTxtIn    [in]        Plaintext to be encrypted.
 * @param ciphTxtOut    [in/out]    Output ciphertext.
 * @param key           [in]        AES key for encryption.
 */

void AES::Encrypt(const vector<uint8_t>& plainTxtIn, vector<uint8_t>& ciphTxtOut, const vector<uint32_t>& key)
{
    switch (mode)
    {
        case ECB:
        case CBC:

            EncryptECBCBC(plainTxtIn, ciphTxtOut, key);
            break;

        case CFB1:

            EncryptCFB(plainTxtIn, 1, ciphTxtOut, key);
            break;

        case CFB8:

            EncryptCFB(plainTxtIn, 8, ciphTxtOut, key);
            break;

        default:

            break;
    }
}

/**
 * AES::Encrypt - AES decryption entry. Route to the appropriate decryption routine based on
 * AES mode.
 * 
 * @param ciphTxtIn     [in]        Ciphertext to be decrypted.
 * @param plainTxtOut   [in/out]    Output plaintext.
 * @param key           [in]        AES key for decryption.
 */

void AES::Decrypt(const vector<uint8_t>& ciphTxtIn, vector<uint8_t>& plainTxtOut, const vector<uint32_t>& key)
{
    switch (mode)
    {
    case ECB:
    case CBC:

        DecryptECBCBC(ciphTxtIn, plainTxtOut, key);
        break;

    case CFB1:

        DecryptCFB(ciphTxtIn, 1, plainTxtOut, key);
        break;

    default:

        break;
    }
}

/**
 * AES::EncryptECBCBC - Encrypt input plaintext in ECB or CBC modes.
 *
 * @param plainTxtIn    [in]        Plaintext to be encrypted.
 * @param msgOut        [in/out]    Output ciphertext.
 * @param key           [in]        AES key for encryption.
 */

void AES::EncryptECBCBC(const vector<uint8_t>& plainTxtIn, vector<uint8_t>& ciphTxtOut,
    const vector<uint32_t>& key)
{
    assert(ciphTxtOut.size() == 0);
    ciphTxtOut.resize(plainTxtIn.size());
    uint32_t offset = 0;

    stream.SetData(plainTxtIn, false);
    ExpandKey(key);

    while (!stream.End())
    {
        ClearState();
        uint32_t plainTxt[4];
        stream.Next(plainTxt);

        state[0] ^= plainTxt[0];
        state[1] ^= plainTxt[1];
        state[2] ^= plainTxt[2];
        state[3] ^= plainTxt[3];

        if (mode == CBC)
        {
            state[0] ^= iv[0];
            state[1] ^= iv[1];
            state[2] ^= iv[2];
            state[3] ^= iv[3];
        }

        AddRoundKey(0);

        for (uint32_t i = 1; i < nr; i++)
        {
            SubBytes();
            ShiftRows();
            MixColumns();
            AddRoundKey(i);
        }

        SubBytes();
        ShiftRows();
        AddRoundKey(nr);

        memcpy(&ciphTxtOut[offset], state, 16);

        if (mode == CBC)
            memcpy(&iv[0], &state[0], 16);

        offset += 16;
    }

    for (uint32_t i = 0; i < ciphTxtOut.size(); i += 4)
    {
        uint8_t tmp         = ciphTxtOut[i];
        ciphTxtOut[i]       = ciphTxtOut[i + 3];
        ciphTxtOut[i + 3]   = ciphTxtOut[i + 1];
        ciphTxtOut[i + 1]   = ciphTxtOut[i + 2];
        ciphTxtOut[i + 2]   = ciphTxtOut[i + 3];
        ciphTxtOut[i + 3]   = tmp;
    }
}

/**
 * AES::DecryptECBCBC - Decrypt input ciphertext in ECB or CBC modes.
 *
 * @param ciphTxtIn     [in]        Ciphertext to be decrypted.
 * @param plainTxtOut   [in/out]    Output plaintext.
 * @param key           [in]        AES key for decryption.
 */

void AES::DecryptECBCBC(const vector<uint8_t>& ciphTxtIn, vector<uint8_t>& plainTxtOut,
    const vector<uint32_t>& key)
{
    assert(plainTxtOut.size() == 0);
    plainTxtOut.resize(ciphTxtIn.size());
    uint32_t offset = 0;

    stream.SetData(ciphTxtIn, false);
    ExpandKey(key);

    while (!stream.End())
    {
        ClearState();
        uint32_t ciphTxt[4];
        stream.Next(ciphTxt);

        state[0] ^= ciphTxt[0];
        state[1] ^= ciphTxt[1];
        state[2] ^= ciphTxt[2];
        state[3] ^= ciphTxt[3];

        AddRoundKey(nr);
        InvShiftRows();
        InvSubBytes();

        for (uint32_t i = nr; i-- > 1;)
        {
            AddRoundKey(i);
            InvMixColumns();
            InvShiftRows();
            InvSubBytes();
        }

        AddRoundKey(0);

        if (mode == CBC)
        {
            state[0] ^= iv[0];
            state[1] ^= iv[1];
            state[2] ^= iv[2];
            state[3] ^= iv[3];

            iv[0] = ciphTxt[0];
            iv[1] = ciphTxt[1];
            iv[2] = ciphTxt[2];
            iv[3] = ciphTxt[3];
        }

        memcpy(&plainTxtOut[offset], state, 16);
        offset += 16;
    }

    for (uint32_t i = 0; i < plainTxtOut.size(); i += 4)
    {
        uint8_t tmp         = plainTxtOut[i];
        plainTxtOut[i]      = plainTxtOut[i + 3];
        plainTxtOut[i + 3]  = plainTxtOut[i + 1];
        plainTxtOut[i + 1]  = plainTxtOut[i + 2];
        plainTxtOut[i + 2]  = plainTxtOut[i + 3];
        plainTxtOut[i + 3]  = tmp;
    }
}

/**
 * AES::WriteBits - Helper for CFB mode. Write s MSB bits from the AES state to an output buffer.
 *
 * @param s         [in]        Number of bits to write. Supports 1, 8, or 128 bits.
 * @param msgOut    [in/out]    Output buffer to write.
 * @param offset    [in]        Output buffer write offset in bits.
 */

void AES::WriteBits(const uint32_t s, vector<uint8_t>& msgOut, const uint32_t offset)
{
    assert(s == 1 || s == 8 || s == 128);
    assert(offset <= 8 * msgOut.size());

    if (s == 8 || s == 128)
    {
        const uint32_t offsetBytes = offset / 8;

        if (s == 8)
            msgOut[offsetBytes] = ((state[0] & 0xFF000000) >> 24);

        if (s == 128)
            memcpy(&msgOut[offsetBytes], &state[0], 16);
    }
    else
    {
        uint32_t byte   = offset / 8;
        uint32_t shift  = offset % 8;
        uint32_t bit    = ((state[0] & 0x80000000) >> 31);
        msgOut[byte]    |= (bit << shift);
    }
}

/**
 * AES::UpdateInputBlock - Helper for CFB mode. After processing each chunk of s bits of plaintext,
 * input blocks are shifted left by s bits and the s MSB bits of the previous iteration's ciphertext 
 * are moved to the s LSB bits of input for the next round.
 * 
 * Move the s MSB bits of the previous iteration's ciphertext to the s LSB bits of the next
 * iteration's input block here.
 *
 * @param s     [in]    Number of ciphertext bits to add to the input block.
 */

void AES::UpdateInputBlock(const uint32_t s)
{
    assert(s == 1 || s == 8 || s == 128);

    uint32_t update = 0;

    switch (s)
    {
    case 1:

        iv[3] &= 0xFFFFFFFE;
        iv[3] |= ((state[0] & 0x80000000) >> 31);
        break;

    case 8:

        iv[3] &= 0xFFFFFFF0;
        iv[3] |= ((state[0] & 0xFF000000) >> 24);
        break;

    case 128:

        memcpy(&iv[0], &state[0], 16);
        break;
    }
}

/**
 * AES::XORPlainText - For CFB mode, XOR s bits from the plaintext into the AES
 * after each cipher iteration.
 *
 * @param plainText     [in]    Plaintext to XOR.
 * @param s             [in]    Number of bits to XOR from the plaintext.
 */

void AES::XORPlainText(uint32_t plainTxt[4], const uint32_t s)
{
    assert(s == 1 || s == 8 || s == 128);

    switch (s)
    {
    case 1:

        state[0] ^= (plainTxt[0] << 31);
        break;

    case 8:

        state[0] ^= (plainTxt[0] << 24);
        break;

    case 128:

        state[0] ^= plainTxt[0];
        state[1] ^= plainTxt[1];
        state[2] ^= plainTxt[2];
        state[3] ^= plainTxt[3];
        break;
    }
}

/**
 * AES::EncryptCFB - Encrypt input plaintext in CFB mode.
 *
 * @param plainTxt      [in]        Plaintext to be encrypted.
 * @param s             [in]        Segment size for CFB mode.
 * @param ciphTxtOut    [in/out]    Output ciphertext.
 * @param key           [in]        AES key for encryption.
 */

void AES::EncryptCFB(const vector<uint8_t>& plainTxtIn, const uint32_t s,
    vector<uint8_t>& ciphTxtOut, const vector<uint32_t>& key)
{
    assert(ciphTxtOut.size() == 0);
    assert(s == 1 || s == 8 || s == 128);

    ciphTxtOut.resize(plainTxtIn.size());
    stream.SetData(plainTxtIn, false);
    ExpandKey(key);

    uint32_t writeOffset = 0;

    while (!stream.End())
    {
        ClearState();
        uint32_t plainTxt[4];
        stream.Next(plainTxt);

        state[0] ^= iv[0];
        state[1] ^= iv[1];
        state[2] ^= iv[2];
        state[3] ^= iv[3];

        AddRoundKey(0);

        for (uint32_t i = 1; i < nr; i++)
        {
            SubBytes();
            ShiftRows();
            MixColumns();
            AddRoundKey(i);
        }

        SubBytes();
        ShiftRows();
        AddRoundKey(nr);
        XORPlainText(plainTxt, s);

        WriteBits(s, ciphTxtOut, writeOffset);
        RotateIVLeft(s);
        UpdateInputBlock(s);

        writeOffset += s;
    }
}

/**
 * AES::DecryptCFB - Decrypt input ciphertext in CFB mode
 *
 * @param ciphTxtIn     [in]        Ciphertext to be decrypted.
 * @param s             [in]        Segment size for CFB mode.
 * @param plainTxtOut   [in/out]    Output plaintext.
 * @param key           [in]        AES key for encryption.
 */

void AES::DecryptCFB(const vector<uint8_t>& ciphTxtIn, const uint32_t s,
    vector<uint8_t>& plainTxtOut, const vector<uint32_t>& key)
{

}