#pragma once

#include "commoninc.h"
#include "msgstreamer.h"

using namespace std;

enum AESSize
{
    AES128,
    AES192,
    AES256
};

enum CipherMode
{
    ECB,
    CBC,
    CFB,
    OFB
};

struct AES
{
    uint32_t state[4];
    AESStreamer stream;
    CipherMode mode;

    uint32_t nk;
    uint32_t nr;

    uint32_t w[64];

    AES(AESSize sz, CipherMode modeIn);

    void ClearState();
    void PrintState();

    void SubBytes();
    void ShiftRows();
    void MixColumns();
    void AddRoundKey(const uint32_t round);

    void ExpandKey(const vector<uint32_t>& key);

    void Encrypt(const vector<uint8_t> &msgIn,
        vector<uint8_t> &msgOut, const vector<uint32_t> &key
    );

    void Decrypt(const vector<uint8_t> &msgIn,
        vector<uint8_t> &msgOut, const vector<uint32_t> &key
    );
};