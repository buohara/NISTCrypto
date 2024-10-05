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

struct AES
{
    uint32_t state[4];
    AESStreamer stream;

    uint32_t nk;
    uint32_t nr;

    uint32_t w[64];

    AES(AESSize sz);

    void ClearState();
    void PrintState();

    void SubBytes();
    void ShiftRows();
    void MixColumns();
    void AddRoundKey(const uint32_t round);

    void ExpandKey(vector<uint32_t>& key);

    void Encrypt(vector<uint8_t> &msgIn, vector<uint8_t> &msgOut, vector<uint32_t> &key);
    void Decrypt(vector<uint8_t> &msgIn, vector<uint8_t> &msgOut, vector<uint32_t> &key);
};