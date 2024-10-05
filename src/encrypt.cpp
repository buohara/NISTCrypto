#include "encrypt.h"

static const uint32_t rcs[10] =
{
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

AES::AES(AESSize sz) : w{0}
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

void AES::ClearState()
{
    state[0] = 0;
    state[1] = 0;
    state[2] = 0;
    state[3] = 0;
}

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
    printf("%02x\n",    (state[3] & 0xFF000000) >> 24);
}

inline static uint32_t SBox(uint32_t val)
{
    uint32_t tmp = 0;

    tmp |= sbox[(val & 0xF0) >> 4][val & 0x0F];
    tmp |= sbox[(val & 0xF000) >> 12][val & 0x0F00 >> 8] << 8;
    tmp |= sbox[(val & 0xF00000) >> 20][val & 0x0F0000 >> 16] << 16;
    tmp |= sbox[(val & 0xF0000000) >> 28][val & 0x0F000000 >> 24] << 24;

    return tmp;
}

inline static uint32_t RotLeft32(uint32_t val, uint32_t n)
{
    assert(n < 4);

    if (n == 0)
        return;

    const uint32_t shift    = 8 * n;
    return ((val << shift) | (val >> (32 - shift)));
}

void AES::SubBytes()
{
    state[0] = SBox(state[0]);
    state[1] = SBox(state[1]);
    state[2] = SBox(state[2]);
    state[3] = SBox(state[3]);
}

void AES::ShiftRows()
{
    state[1] = RotLeft32(state[1], 1);
    state[2] = RotLeft32(state[2], 2);
    state[3] = RotLeft32(state[3], 3);
}

void AES::MixColumns()
{
    uint32_t tmp[4] = { 0 };

    tmp[0] |= 0x02 * state[0] & 0xFF;
    tmp[0] |= 0x03 * (state[1] & 0xFF00 >> 8);
    tmp[0] |= state[2] & 0xFF0000 >> 16;
    tmp[0] |= state[3] & 0xFF000000 >> 24;

    tmp[1] |= state[0] & 0xFF;
    tmp[1] |= 0x02 * (state[1] & 0xFF00 >> 8);
    tmp[1] |= 0x03 * (state[2] & 0xFF0000 >> 16);
    tmp[1] |= state[3] & 0xFF000000 >> 24;

    tmp[2] |= state[0] & 0xFF;
    tmp[2] |= state[1] & 0xFF00 >> 8;
    tmp[2] |= 0x2 * (state[2] & 0xFF0000 >> 16);
    tmp[2] |= 0x3 * (state[3] & 0xFF000000 >> 24);

    tmp[3] |= 0x03 * (state[0] & 0xFF);
    tmp[3] |= state[1] & 0xFF00 >> 8;
    tmp[3] |= state[2] & 0xFF0000 >> 16;
    tmp[3] |= 0x02 * (state[3] & 0xFF000000 >> 24);

    state[0] = tmp[0];
    state[1] = tmp[1];
    state[2] = tmp[2];
    state[3] = tmp[3];
}

void AES::AddRoundKey(const uint32_t round)
{
    uint32_t offset = 4 * round;

    state[0] ^= w[offset];
    state[1] ^= w[offset + 1];
    state[2] ^= w[offset + 2];
    state[3] ^= w[offset + 3];
}

void AES::ExpandKey(vector<uint32_t>& key)
{
    assert(key.size() / 32 == nk);
    memcpy(w, &key[0], nk * 4);

    for (uint32_t i = nk; i < 4 * nr + 3; i++)
    {
        uint32_t tmp = w[i - 1];

        if ((i % nk) == 0)
        {
            tmp = RotLeft32(tmp, 1);
            tmp = SBox(tmp);
            tmp ^= rcs[i / nk];
        }
        else if ((nk == 8) && (i % nk == 4))
        {
            tmp = SBox(tmp);
        }

        w[i] = w[i - nk] ^ tmp;
    }
}

void AES::Encrypt(vector<uint8_t>& msgIn, vector<uint8_t>& msgOut, vector<uint32_t>& key)
{
    assert(msgOut.size() == 0);
    uint32_t offset = 0;

    stream.SetData(msgIn);
    ExpandKey(key);

    while (!stream.End())
    {
        AddRoundKey(0);

        for (uint32_t i = 1; i <= nr; i++)
        {
            SubBytes();
            ShiftRows();
            MixColumns();
            AddRoundKey(i);
        }

        SubBytes();
        ShiftRows();
        AddRoundKey(4 * nr);

        memcpy(&msgOut[offset], state, 16);
        offset += 16;
    }
}

void AES::Decrypt(vector<uint8_t>& msgIn, vector<uint8_t>& msgOut, vector<uint32_t>& key)
{
    ExpandKey(key);
}