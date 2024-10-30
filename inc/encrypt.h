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

enum AESMode
{
    ECB,
    CBC,
    CFB1,
    CFB8,
    CFB128,
    OFB
};

struct AESStreamer
{
    uint32_t bitOffset;
    uint32_t r;
    AESMode mode;
    vector<uint8_t> data;

    AESStreamer() : bitOffset(0), r(128), mode(ECB) {};
    AESStreamer(AESMode modeIn);

    void SetData(const vector<uint8_t>& dataIn, bool bLittleEndian = true);
    void Reset();
    void Next(uint32_t block[4]);
    bool End();
};

struct AES
{
    uint32_t state[4];
    AESStreamer stream;
    AESMode mode;

    uint32_t nk;
    uint32_t nr;

    uint32_t w[64];

    vector<uint32_t> iv;

    AES(AESSize sz, AESMode modeIn);

    void ClearState();
    void PrintState();

    void SubBytes();
    void ShiftRows();
    void MixColumns();
    void AddRoundKey(const uint32_t round);

    void InvSubBytes();
    void InvShiftRows();
    void InvMixColumns();

    void SetIV(const vector<uint32_t>& iv);
    void RotateIVLeft(const uint32_t s);
    void XORText(uint32_t txt[4], uint32_t s);
    void ExpandKey(const vector<uint32_t>& key);
    void WriteBits(const uint32_t s, vector<uint8_t> &msgOut, const uint32_t offset);
    void UpdateInputBlock(const uint32_t s);
    void UpdateInputBlock(const uint32_t s, const uint32_t txt[4]);

    void Encrypt(const vector<uint8_t> &plainTxtIn,
        vector<uint8_t> &ciphTxtOut, const vector<uint32_t> &key
    );

    void Decrypt(const vector<uint8_t> &ciphTxtIn,
        vector<uint8_t> &plainTxtOut, const vector<uint32_t> &key
    );

private:

    void EncryptECBCBC(const vector<uint8_t>& plainTxtIn,
        vector<uint8_t>& ciphTxtOut, const vector<uint32_t>& key
    );

    void DecryptECBCBC(const vector<uint8_t>& ciphTxtIn,
        vector<uint8_t>& plainTxtOut, const vector<uint32_t>& key
    );

    void EncryptCFB(const vector<uint8_t>& plainTxtIn, const uint32_t s,
        vector<uint8_t>& ciphTxtOut, const vector<uint32_t>& key
    );

    void DecryptCFB(const vector<uint8_t>& ciphTxtIn, const uint32_t s,
        vector<uint8_t>& plainTxtOut, const vector<uint32_t>& key
    );
};