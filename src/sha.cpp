#include "sha.h"

#define VERBOSE 0

static const uint64_t rcs[24] =
{
    0x0000000000000001,
    0x0000000000008082,
    0X800000000000808A,
    0x8000000080008000,
    0x000000000000808B,
    0x0000000080000001,
    0x8000000080008081,
    0x8000000000008009,
    0x000000000000008A,
    0x0000000000000088,
    0x0000000080008009,
    0x000000008000000A,
    0x000000008000808B,
    0x800000000000008B,
    0x8000000000008089,
    0x8000000000008003,
    0x8000000000008002,
    0x8000000000000080,
    0x000000000000800A,
    0x800000008000000A,
    0x8000000080008081,
    0x8000000000008080,
    0x0000000080000001,
    0x8000000080008008
};

static const uint32_t shaConst32[64] =
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static const uint64_t shaConst64[80] =
{
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
    0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
    0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
    0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
    0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
    0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
    0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
    0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
    0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
    0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
    0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
    0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
    0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
    0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
    0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
    0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
    0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

/**
 * SHAStreamer::SetData - Set data to be hashed in the message
 * streamer. The streamer feeds message blocks whose bit size is
 * the SHA3 rate (see FIPS 202. Rate = 1600 bits - 2 * capacity =
 * 1600 - 2 * (output MD output length in bits)).
 *
 * The FIPS 202 spec says messages are to be padded using the "Pad10*1" rule.
 * Official NIST test messages, however, add a 0x6 suffix after the last message bit.
 * No explanation for this discrepency has been given. For testing conformance,
 * append 0x6 here.
 *
 * @param dataIn            [in] Message data to be hashed as a little endian byte array.
 * @param bLittleEndian     [in] True if input data is in little endian order.
 */

void SHAStreamer::SetData(vector<uint8_t>& dataIn, bool bLittleEndian)
{
    const uint64_t rateBytes    = r / 8;
    uint64_t padBytes           = (rateBytes - dataIn.size() % rateBytes);
    const uint64_t sizeIn       = dataIn.size();

    if (padBytes == 0)
        padBytes += rateBytes;

    data.resize(sizeIn + padBytes);

    if (sizeIn)
    {
        if (bLittleEndian)
            memcpy(&data[0], &dataIn[0], sizeIn);
        else
            for (uint64_t i = sizeIn; i-- > 0;)
                data[sizeIn - i - 1] = dataIn[i];
    }

    data[sizeIn]            |= 0x06;
    data[data.size() - 1]   |= 0x80;
}

/**
 * SHAStreamer::Reset - Reset the streamer to the beginning of the
 * message.
 */

void SHAStreamer::Reset()
{
    offset = 0;
}

/**
 * SHAStreamer::Next - Get the next message block to feed into the
 * SHA3 sponge.
 *
 * @param blockOut    [in/out]
 */

void SHAStreamer::Next(vector<uint64_t>& blockOut)
{
    const uint64_t rateWords = r / 64;

    assert(r != 0);
    assert(blockOut.size() == rateWords);
    assert(offset != data.size());

    if (offset + 8 * rateWords <= data.size())
    {
        memcpy(&blockOut[0], &data[offset], 8 * rateWords);
        offset += 8 * rateWords;
    }
    else
    {
        uint64_t rem = data.size() - offset;
        memcpy(&blockOut[0], &data[offset], rem);
        offset = data.size();
    }
}

/**
 * SHAStreamer::End - Return true if input all message blocks have been
 * processed.
 *
 * @return True if at the end of the message, false otherwise.
 */

bool SHAStreamer::End()
{
    if (offset == data.size())
        return true;

    return false;
}

/**
 * SHA2::ch32 - 
 *
 * @param x    [in]
 * @param y    [in]
 * @param z    [in]
 *
 * @return 
 */

uint32_t SHA2::ch32(uint32_t x, uint32_t y, uint32_t z)
{
    return ((x & y) ^ ((~x) & z));
}

/**
 * SHA2::maj32 -
 *
 * @param x    [in]
 * @param y    [in]
 * @param z    [in]
 *
 * @return
 */

uint32_t SHA2::maj32(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

/**
 * SHA2::Sig032 -
 *
 * @param x    [in]
 *
 * @return
 */

uint32_t SHA2::Sig032(uint32_t x)
{
    return ROTR32(x, 2) ^ ROTR32(x, 13) ^ ROTR32(x, 22);
}

/**
 * SHA2::Sig132 -
 *
 * @param x    [in]
 *
 * @return
 */

uint32_t SHA2::Sig132(uint32_t x)
{
    return ROTR32(x, 6) ^ ROTR32(x, 11) ^ ROTR32(x, 25);
}

/**
 * SHA2::sig032 -
 *
 * @param x    [in]
 *
 * @return
 */

uint32_t SHA2::sig032(uint32_t x)
{
    return ROTR32(x, 7) ^ ROTR32(x, 18) ^ (x >> 3);
}

/**
 * SHA2::sig132 -
 *
 * @param x    [in]
 *
 * @return
 */

uint32_t SHA2::sig132(uint32_t x)
{
    return ROTR32(x, 17) ^ ROTR32(x, 19) ^ (x >> 10);
}

/**
 * SHA2::ch64 -
 *
 * @param x    [in]
 * @param y    [in]
 * @param z    [in]
 *
 * @return
 */

uint64_t SHA2::ch64(uint64_t x, uint64_t y, uint64_t z)
{
    return ((x & y) ^ ((~x) & z));
}

/**
 * SHA2::maj32 -
 *
 * @param x    [in]
 * @param y    [in]
 * @param z    [in]
 *
 * @return
 */

uint64_t SHA2::maj64(uint64_t x, uint64_t y, uint64_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

/**
 * SHA2::Sig064 -
 *
 * @param x    [in]
 *
 * @return
 */

uint64_t SHA2::Sig064(uint64_t x)
{
    return ROTR64(x, 28) ^ ROTR64(x, 34) ^ ROTR64(x, 39);
}

/**
 * SHA2::Sig164 -
 *
 * @param x    [in]
 *
 * @return
 */

uint64_t SHA2::Sig164(uint64_t x)
{
    return ROTR64(x, 14) ^ ROTR64(x, 18) ^ ROTR64(x, 41);
}

/**
 * SHA2::sig064 -
 *
 * @param x    [in]
 *
 * @return
 */

uint64_t SHA2::sig064(uint64_t x)
{
    return ROTR64(x, 1) ^ ROTR64(x, 8) ^ (x >> 7);
}

/**
 * SHA2::sig164 -
 *
 * @param x    [in]
 *
 * @return
 */

uint64_t SHA2::sig164(uint64_t x)
{
    return ROTR64(x, 19) ^ ROTR64(x, 61) ^ (x >> 6);
}

void SHA2::Hash(SHASize sz, vector<uint8_t>& msg, vector<uint8_t>& md)
{
    switch (sz)
    {
        case SHA224:

            Hash224(msg, md);
            break;

        case SHA256:

            Hash256(msg, md);
            break;

        case SHA384:

            Hash384(msg, md);
            break;

        case SHA512:

            Hash512(msg, md);
            break;

        default:
            break;
    }
}

/**
 * SHA2::Hash224 - Compute the SHA224 of an input message.
 *
 * @param msg       [in]        Message to be hashed.
 * @param md        [in/out]    Computed MD of input message.
 */

void SHA2::Hash224(vector<uint8_t>& msg, vector<uint8_t>& md)
{
    assert(md.size() == 0);

    const uint64_t sz               = msg.size();
    const uint64_t l                = 8 * sz;
    const uint32_t blockBits        = 512;
    const uint32_t blockBytes       = 64;
    const uint32_t scheduleWords    = 64;
    uint32_t padBytes               = (blockBits - (l % blockBits)) / 8;

    if (padBytes <= 8)
        padBytes += blockBytes;

    vector<uint8_t> tmp(sz + padBytes);

    if (sz)
        memcpy(&tmp[0], &msg[0], sz);

    tmp[sz > 0 ? sz : 0] = 0x80;

    for (uint32_t i = 0; i < tmp.size() / 4; i++)
    {
        uint8_t a       = tmp[4 * i];
        tmp[4 * i]      = tmp[4 * i + 3];
        tmp[4 * i + 3]  = a;

        a               = tmp[4 * i + 1];
        tmp[4 * i + 1]  = tmp[4 * i + 2];
        tmp[4 * i + 2]  = a;
    }

    memcpy(&tmp[tmp.size() - 4], &l, 4);
    const uint64_t n = tmp.size() / blockBytes;

    uint32_t H[8] =
    {
        0xc1059ed8,
        0x367cd507,
        0x3070dd17,
        0xf70e5939,
        0xffc00b31,
        0x68581511,
        0x64f98fa7,
        0xbefa4fa4
    };

    for (uint32_t i = 0; i < n; i++)
    {
        uint32_t w[scheduleWords];

        memcpy(&w[0], &tmp[blockBytes * i], blockBytes);

        for (uint32_t t = 16; t < scheduleWords; t++)
            w[t] = sig132(w[t - 2]) + w[t - 7] + sig032(w[t - 15]) + w[t - 16];

        uint32_t a = H[0];
        uint32_t b = H[1];
        uint32_t c = H[2];
        uint32_t d = H[3];
        uint32_t e = H[4];
        uint32_t f = H[5];
        uint32_t g = H[6];
        uint32_t h = H[7];

        for (uint32_t t = 0; t < scheduleWords; t++)
        {
            uint32_t T1 = h + Sig132(e) + ch32(e, f, g) + shaConst32[t] + w[t];
            uint32_t T2 = Sig032(a) + maj32(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }

        H[0] = a + H[0];
        H[1] = b + H[1];
        H[2] = c + H[2];
        H[3] = d + H[3];
        H[4] = e + H[4];
        H[5] = f + H[5];
        H[6] = g + H[6];
        H[7] = h + H[7];
    }

    H[0] = REVERSE_ENDIAN32(H[0]);
    H[1] = REVERSE_ENDIAN32(H[1]);
    H[2] = REVERSE_ENDIAN32(H[2]);
    H[3] = REVERSE_ENDIAN32(H[3]);
    H[4] = REVERSE_ENDIAN32(H[4]);
    H[5] = REVERSE_ENDIAN32(H[5]);
    H[6] = REVERSE_ENDIAN32(H[6]);
    H[7] = REVERSE_ENDIAN32(H[7]);

    md.resize(28);
    memcpy(&md[0], &H[0], 28);
}

/**
 * SHA2::Hash256 - Compute the SHA256 of an input message.
 *
 * @param msg       [in]        Message to be hashed.
 * @param md        [in/out]    Computed MD of input message.
 */

void SHA2::Hash256(vector<uint8_t>& msg, vector<uint8_t>& md)
{
    assert(md.size() == 0);

    const uint64_t sz               = msg.size();
    const uint64_t l                = 8 * sz;
    const uint32_t blockBits        = 512;
    const uint32_t blockBytes       = 64;
    const uint32_t scheduleWords    = 64;
    uint32_t padBytes               = (blockBits - (l % blockBits)) / 8;

    if (padBytes <= 8)
        padBytes += blockBytes;

    vector<uint8_t> tmp(sz + padBytes);

    if (sz)
        memcpy(&tmp[0], &msg[0], sz);

    tmp[sz > 0 ? sz : 0] = 0x80;

    for (uint32_t i = 0; i < tmp.size() / 4; i++)
    {
        uint8_t a       = tmp[4 * i];
        tmp[4 * i]      = tmp[4 * i + 3];
        tmp[4 * i + 3]  = a;

        a               = tmp[4 * i + 1];
        tmp[4 * i + 1]  = tmp[4 * i + 2];
        tmp[4 * i + 2]  = a;
    }

    memcpy(&tmp[tmp.size() - 4], &l, 4);

    const uint64_t n = tmp.size() / blockBytes;

    uint32_t H[8] =
    {
        0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19,
    };

    for (uint32_t i = 0; i < n; i++)
    {
        uint32_t w[scheduleWords];

        memcpy(&w[0], &tmp[blockBytes * i], blockBytes);

        for (uint32_t t = 16; t < scheduleWords; t++)
            w[t] = sig132(w[t - 2]) + w[t - 7] + sig032(w[t - 15]) + w[t - 16];

        uint32_t a = H[0];
        uint32_t b = H[1];
        uint32_t c = H[2];
        uint32_t d = H[3];
        uint32_t e = H[4];
        uint32_t f = H[5];
        uint32_t g = H[6];
        uint32_t h = H[7];

        for (uint32_t t = 0; t < scheduleWords; t++)
        {
            uint32_t T1 = h + Sig132(e) + ch32(e, f, g) + shaConst32[t] + w[t];
            uint32_t T2 = Sig032(a) + maj32(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }

        H[0] = a + H[0];
        H[1] = b + H[1];
        H[2] = c + H[2];
        H[3] = d + H[3];
        H[4] = e + H[4];
        H[5] = f + H[5];
        H[6] = g + H[6];
        H[7] = h + H[7];
    }

    H[0] = REVERSE_ENDIAN32(H[0]);
    H[1] = REVERSE_ENDIAN32(H[1]);
    H[2] = REVERSE_ENDIAN32(H[2]);
    H[3] = REVERSE_ENDIAN32(H[3]);
    H[4] = REVERSE_ENDIAN32(H[4]);
    H[5] = REVERSE_ENDIAN32(H[5]);
    H[6] = REVERSE_ENDIAN32(H[6]);
    H[7] = REVERSE_ENDIAN32(H[7]);

    md.resize(32);
    memcpy(&md[0], &H[0], 32);
}

/**
 * SHA2::Hash384 - Compute the SHA384 of an input message.
 *
 * @param msg       [in]        Message to be hashed.
 * @param md        [in/out]    Computed MD of input message.
 */

void SHA2::Hash384(vector<uint8_t>& msg, vector<uint8_t>& md)
{
    assert(md.size() == 0);

    const uint64_t sz               = msg.size();
    const uint32_t scheduleWords    = 80;
    const uint32_t blockBits        = 1024;
    const uint32_t blockBytes       = 128;
    const uint64_t l                = 8 * sz;
    uint64_t padBytes               = (blockBits - (l % blockBits)) / 8;

    if (padBytes <= 16)
        padBytes += 128;

    vector<uint8_t> tmp(sz + padBytes);

    if (sz)
        memcpy(&tmp[0], &msg[0], sz);

    tmp[sz > 0 ? sz : 0] = 0x80;

    for (uint64_t i = 0; i < tmp.size() / 8; i++)
    {
        uint8_t a       = tmp[8 * i];
        tmp[8 * i]      = tmp[8 * i + 7];
        tmp[8 * i + 7]  = a;

        a               = tmp[8 * i + 1];
        tmp[8 * i + 1]  = tmp[8 * i + 6];
        tmp[8 * i + 6]  = a;

        a               = tmp[8 * i + 2];
        tmp[8 * i + 2]  = tmp[8 * i + 5];
        tmp[8 * i + 5]  = a;

        a               = tmp[8 * i + 3];
        tmp[8 * i + 3]  = tmp[8 * i + 4];
        tmp[8 * i + 4]  = a;
    }

    memcpy(&tmp[tmp.size() - 8], &l, 8);

    const uint64_t n = tmp.size() / blockBytes;

    uint64_t H[8] =
    {
        0xcbbb9d5dc1059ed8,
        0x629a292a367cd507,
        0x9159015a3070dd17,
        0x152fecd8f70e5939,
        0x67332667ffc00b31,
        0x8eb44a8768581511,
        0xdb0c2e0d64f98fa7,
        0x47b5481dbefa4fa4,
    };

    for (uint64_t i = 0; i < n; i++)
    {
        uint64_t w[scheduleWords];
        memcpy(&w[0], &tmp[blockBytes * i], blockBytes);

        for (uint64_t t = 16; t < scheduleWords; t++)
            w[t] = sig164(w[t - 2]) + w[t - 7] + sig064(w[t - 15]) + w[t - 16];

        uint64_t a = H[0];
        uint64_t b = H[1];
        uint64_t c = H[2];
        uint64_t d = H[3];
        uint64_t e = H[4];
        uint64_t f = H[5];
        uint64_t g = H[6];
        uint64_t h = H[7];

        for (uint64_t t = 0; t < scheduleWords; t++)
        {
            uint64_t T1 = h + Sig164(e) + ch64(e, f, g) + shaConst64[t] + w[t];
            uint64_t T2 = Sig064(a) + maj64(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }

        H[0] = a + H[0];
        H[1] = b + H[1];
        H[2] = c + H[2];
        H[3] = d + H[3];
        H[4] = e + H[4];
        H[5] = f + H[5];
        H[6] = g + H[6];
        H[7] = h + H[7];
    }

    H[0] = REVERSE_ENDIAN64(H[0]);
    H[1] = REVERSE_ENDIAN64(H[1]);
    H[2] = REVERSE_ENDIAN64(H[2]);
    H[3] = REVERSE_ENDIAN64(H[3]);
    H[4] = REVERSE_ENDIAN64(H[4]);
    H[5] = REVERSE_ENDIAN64(H[5]);
    H[6] = REVERSE_ENDIAN64(H[6]);
    H[7] = REVERSE_ENDIAN64(H[7]);

    md.resize(48);
    memcpy(&md[0], &H[0], 48);
}

/**
 * SHA2::Hash512 - Compute the SHA512 of an input message.
 *
 * @param msg       [in]        Message to be hashed.
 * @param md        [in/out]    Computed MD of input message.
 */

void SHA2::Hash512(vector<uint8_t>& msg, vector<uint8_t>& md)
{
    assert(md.size() == 0);

    const uint64_t sz               = msg.size();
    const uint32_t scheduleWords    = 80;
    const uint32_t blockBits        = 1024;
    const uint32_t blockBytes       = 128;
    const uint64_t l                = 8 * sz;
    uint64_t padBytes               = (blockBits - (l % blockBits)) / 8;

    if (padBytes <= 16)
        padBytes += 128;

    vector<uint8_t> tmp(sz + padBytes);

    if (sz)
        memcpy(&tmp[0], &msg[0], sz);

    tmp[sz > 0 ? sz : 0] = 0x80;

    for (uint64_t i = 0; i < tmp.size() / 8; i++)
    {
        uint8_t a       = tmp[8 * i];
        tmp[8 * i]      = tmp[8 * i + 7];
        tmp[8 * i + 7]  = a;

        a               = tmp[8 * i + 1];
        tmp[8 * i + 1]  = tmp[8 * i + 6];
        tmp[8 * i + 6]  = a;

        a               = tmp[8 * i + 2];
        tmp[8 * i + 2]  = tmp[8 * i + 5];
        tmp[8 * i + 5]  = a;

        a               = tmp[8 * i + 3];
        tmp[8 * i + 3]  = tmp[8 * i + 4];
        tmp[8 * i + 4]  = a;
    }

    memcpy(&tmp[tmp.size() - 8], &l, 8);

    const uint64_t n = tmp.size() / blockBytes;

    uint64_t H[8] =
    {
        0x6a09e667f3bcc908,
        0xbb67ae8584caa73b,
        0x3c6ef372fe94f82b,
        0xa54ff53a5f1d36f1,
        0x510e527fade682d1,
        0x9b05688c2b3e6c1f,
        0x1f83d9abfb41bd6b,
        0x5be0cd19137e2179
    };

    for (uint64_t i = 0; i < n; i++)
    {
        uint64_t w[scheduleWords];
        memcpy(&w[0], &tmp[blockBytes * i], blockBytes);

        for (uint64_t t = 16; t < scheduleWords; t++)
            w[t] = sig164(w[t - 2]) + w[t - 7] + sig064(w[t - 15]) + w[t - 16];

        uint64_t a = H[0];
        uint64_t b = H[1];
        uint64_t c = H[2];
        uint64_t d = H[3];
        uint64_t e = H[4];
        uint64_t f = H[5];
        uint64_t g = H[6];
        uint64_t h = H[7];

        for (uint64_t t = 0; t < scheduleWords; t++)
        {
            uint64_t T1 = h + Sig164(e) + ch64(e, f, g) + shaConst64[t] + w[t];
            uint64_t T2 = Sig064(a) + maj64(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }

        H[0] = a + H[0];
        H[1] = b + H[1];
        H[2] = c + H[2];
        H[3] = d + H[3];
        H[4] = e + H[4];
        H[5] = f + H[5];
        H[6] = g + H[6];
        H[7] = h + H[7];
    }

    H[0] = REVERSE_ENDIAN64(H[0]);
    H[1] = REVERSE_ENDIAN64(H[1]);
    H[2] = REVERSE_ENDIAN64(H[2]);
    H[3] = REVERSE_ENDIAN64(H[3]);
    H[4] = REVERSE_ENDIAN64(H[4]);
    H[5] = REVERSE_ENDIAN64(H[5]);
    H[6] = REVERSE_ENDIAN64(H[6]);
    H[7] = REVERSE_ENDIAN64(H[7]);

    md.resize(64);
    memcpy(&md[0], &H[0], 64);
}

/**
 * SHA3::Get - Get a value from the state array at specified
 * x, y, z indices.
 *
 * @param x    [in] x coord.
 * @param y    [in] y coord.
 * @param z    [in] z coord.
 * 
 * @return Value of state array at specified indices.
 */

uint64_t SHA3::GetBit(uint64_t x, uint64_t y, uint64_t z)
{
    uint64_t lane   = LANE(x, y);
    uint64_t val    = (state[lane] & (1LLU << z)) >> z;
    return val;
}

/**
 * SHA3::SetBit - Set a bit in the state array at specified
 * x, y, z indices.
 *
 * @param x    [in] x coord.
 * @param y    [in] y coord.
 * @param z    [in] z coord.
 */

void SHA3::SetBit(uint64_t x, uint64_t y, uint64_t z, uint64_t bit)
{
    uint64_t lane       = LANE(x, y);
    bit ? state[lane]   |= (1LLU << z) : state[lane] &= ~(1LLU << z);
}

/**
 * SHA3::SetBit - Set a bit in a user-provided array. Array
 * assumed to match state array size on input.
 *
 * @param arrayIn   [in/out]    Set value into this array.
 * @param x         [in]        x coord.
 * @param y         [in]        y coord.
 * @param z         [in]        z coord.
 */

void SHA3::SetBit(uint64_t arrayIn[STATE_W * STATE_H], uint64_t x, uint64_t y, uint64_t z, uint64_t bit)
{
    uint64_t lane       = LANE(x, y);
    bit ? arrayIn[lane] |= (1LLU << z) : arrayIn[lane] &= ~(1LLU << z);
}

/**
 * SHA3::SHA3 - Constructor. Initialize hash function parameters based on input
 * hash size.
 */

SHA3::SHA3(SHASize sz)
{
    switch(sz)
    {
        case SHA224:
            params.d = 224;
            break;

        case SHA256:
            params.d = 256;
            break;

        case SHA384:
            params.d = 384;
            break;

        case SHA512:
            params.d = 512;
            break;

        default:
            params.d = 512;
            break;
    }

    params.b    = 1600;
    params.c    = 2 * params.d;
    params.r    = params.b - params.c;
    params.sz   = sz;
    params.w    = 64;
    params.l    = 6;
    params.n    = 12 + 2 * params.l;
    stream      = SHAStreamer(params.r);

    memset(state, 0, STATE_W * STATE_H * STATE_L / 8);
}

/**
 * SHA3::ClearState - Clear the state array to zeros.
 */

void SHA3::ClearState()
{
    memset(state, 0, STATE_BYTES);
}

/**
 * SHA3::PrintLanesU64 - Print the state array.
 */

void SHA3::PrintState(PrintMode)
{
    if (1)
    {
        for (uint64_t x = 0; x < STATE_W; x++)
        {
            for (uint64_t y = 0; y < STATE_H; y++)
                printf("(%lu, %lu) = 0x%016lx\n", x, y, state[LANE(x, y)]);

            printf("\n");
        }

        return;
    }

    if (1)
    {
        for (uint64_t i = 0; i < STATE_W * STATE_H; i++)
        {
            uint64_t lane = state[i];

            for (uint64_t j = 0; j < 8; j++)
            {
                uint64_t byte = lane & (0xFFLLU << (8 * j));
                byte >>= (8 * j);
                printf("%02x ", (uint8_t)byte);
            }

            if (!((i + 1) % 2))
                printf("\n");
        }

        printf("\n\n");
        return;
    }
}

/**
 * SHA3::SpongeAbsorbBlock - XOR a message block into the state array
 * and run Keccak permutations on it.
 *
 * @param block    [in] New block to absorb into the sponge.
 */

void SHA3::SpongeAbsorbBlock(vector<uint64_t>& block)
{
    const uint8_t rateWords = params.r / 64;
    assert(block.size() == rateWords);

    for (uint64_t i = 0; i < rateWords; i++)
        state[i] ^= block[i];

#if VERBOSE

    static bool bInit = true;

    if (bInit)
    {
        printf("State with Initial Message Block:\n\n");
        PrintState(LINEAR);
        bInit = false;
    }

#endif

    ApplyKeccak();
}

/**
 * SHA3::ApplyKeccak - Apply the step transformtions to the state array
 * for the specified number of rounds.
 */

void SHA3::ApplyKeccak()
{
    for (uint64_t i = 0; i < params.n; i++)
    {

#if VERBOSE

        printf("Round %llu:\n", i);

        printf("Theta\n");
        Theta();
        PrintState(LINEAR);

        printf("Rho\n\n");
        Rho();
        PrintState(LINEAR);

        printf("Pi\n\n");
        Pi();
        PrintState(LINEAR);

        printf("Chi\n\n");
        Chi();
        PrintState(LINEAR);

        printf("Iota\n\n");
        Iota(i);
        PrintState(LINEAR);

#else

        Theta();
        Rho();
        Pi();
        Chi();
        Iota(i);

#endif
    }
}

/**
 * SHA3::SpongeSqueezeBlock - After absorbing all message inputs
 * and applying permutations, "squeeze" MD of desired length out
 * in blocks of size channel rate and put into an output MD byte
 * array.
 * 
 * @param md    [in/out] Hash of length d (512 for SHA512, 256 for
 *              SHA256) to "squeeze" out of the sponge function.
 */

void SHA3::SpongeSqueezeBlock(vector<uint8_t>& md)
{
    vector<uint64_t> z;
    const uint64_t rateWords    = params.r / 64;
    const uint64_t digestBytes  = params.d / 8;

    while (z.size() < digestBytes)
    {
        for (uint64_t i = 0; i < rateWords; i++)
        {
            for (uint64_t j = 0; j < 8; j++)
            {
                uint64_t byte   = state[i] & (0xFFLLU << (j * 8));
                byte            >>= (j * 8);

                z.push_back((uint8_t)byte);
            }
        }

        ApplyKeccak();
    }

    for (uint64_t i = 0; i < digestBytes; i++)
        md.push_back(z[i]);
}

/**
 * SHA3::Theta - Theta step transformtion for SHA3. Loop through
 * bits in the state array. For each bit, XOR the parity of
 * neighboring columns with this bit's value.
 */

void SHA3::Theta()
{
    uint64_t tmp[STATE_W * STATE_H];
    memcpy(tmp, state, STATE_BYTES);

    for (uint64_t x = 0; x < STATE_W; x++)
    {
        for (uint64_t z = 0; z < STATE_L; z++)
        {
            uint64_t xHi    = (x + 1) % STATE_W;
            uint64_t xLo    = (x + 4) % STATE_W;
            uint64_t zLo    = (z + 63) % STATE_L;
            uint64_t d      = 0;

            for (uint64_t y = 0; y < STATE_H; y++)
            {
                d ^= GetBit(xHi, y, zLo);
                d ^= GetBit(xLo, y, z);
            }

            for (uint64_t y = 0; y < STATE_H; y++)
                SetBit(tmp, x, y, z, (d ^ GetBit(x, y, z)));
        }
    }

    memcpy(state, tmp, STATE_BYTES);
}

/**
 * SHA3::Rho - Rho step transformtion for SHA3. Rotate bits within
 * each state lane by precomputed values.
 */

void SHA3::Rho()
{
    const uint64_t laneOffsets[5][5] =
    {
        0,   1, 62, 28, 27,
        36, 44,  6, 55, 20,
        3,  10, 43, 25, 39,
        41, 45, 15, 21,  8,
        18,  2, 61, 56, 14
    };

    for (uint64_t x = 0; x < STATE_W; x++)
    {
        for (uint64_t y = 0; y < STATE_W; y++)
        {
            state[LANE(x, y)] = (state[LANE(x, y)] << laneOffsets[y][x]) |
                (state[LANE(x, y)] >> (STATE_L - laneOffsets[y][x]));
        }
    }
}

/**
 * SHA3::Pi - Pi step transformtion for SHA3. Shuffle lanes
 * in the state to different x,y positions.
 */

void SHA3::Pi()
{
    uint64_t tmp[STATE_W * STATE_H];

    const uint64_t piRot[STATE_W][STATE_H] =
    {
        0, 10, 20, 5, 15,
        16, 1, 11, 21, 6,
        7, 17, 2, 12, 22,
        23, 8, 18, 3, 13,
        14, 24, 9, 19, 4
    };

    for (uint64_t x = 0; x < STATE_W; x++)
        for (uint64_t y = 0; y < STATE_H; y++)
            tmp[piRot[x][y]] = state[LANE(y, x)];

    memcpy(state, tmp, STATE_BYTES);
}

/**
 * SHA3::Chi - Chi step transformtion for SHA3. XOR and multiply
 * bits in each row with other values in the same row.
 */

void SHA3::Chi()
{
    uint64_t tmp[STATE_W * STATE_H];
    memcpy(tmp, state, STATE_BYTES);

    for (uint64_t x = 0; x < STATE_W; x++)
    {
        for (uint64_t y = 0; y < STATE_H; y++)
        {
            for (uint64_t z = 0; z < STATE_L; z++)
            {
                uint64_t a1  = GetBit(x, y, z);
                uint64_t a2  = GetBit((x + 1) % STATE_W, y, z);
                uint64_t a3  = GetBit((x + 2) % STATE_W, y, z);
                uint64_t out = a1 ^ (a2 ^ 1) * a3;

                SetBit(tmp, x, y, z, out);
            }
        }
    }

    memcpy(state, tmp, STATE_BYTES);
}

/**
 * SHA3::Iota - Iota step transformtion for SHA3. XOR bits in each line
 * with precomputed RC values that are indexed by the current round
 * index.
 *
 * @param round    [in] Round number needed in this step transformation.
 */

void SHA3::Iota(uint64_t round)
{
    assert(round < NUM_SHA3_ROUNDS);
    state[LANE(0, 0)] ^= rcs[round];
}

/**
 * SHA3::Hash - Apply a SHA3 hash to an input message.
 *
 * @param msg     [in]      Input message to be hashed.
 * @param md      [in/out]  MD to populate as a byte vector. Assumed empty on input.
 */

void SHA3::Hash(vector<uint8_t> &msg, vector<uint8_t> &md)
{
    assert(md.size() == 0);

    ClearState();
    stream.SetData(msg);

    vector<uint64_t> curBlock(params.r / 64);
    const uint64_t rateBytes = params.r / 8;

    while (!stream.End())
    {
        memset(&curBlock[0], 0, rateBytes);
        stream.Next(curBlock);
        SpongeAbsorbBlock(curBlock);
    }

    SpongeSqueezeBlock(md);
    stream.Reset();
    return;
}