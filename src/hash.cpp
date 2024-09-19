#include "hash.h"

#define STATE_IDX(x, y, z, w) ((w) * (5 * (y) + (x)) + (z))
#define STATE_W 5
#define STATE_H 5

/**
 * SHA3::GetIdx - Get a value from the state array at specified
 * x, y, z position.
 *
 * @param x    [in] x coord.
 * @param y    [in] y coord.
 * @param z    [in] z coord.
 * 
 * @return Value of state array at specified location.
 */

uint8_t SHA3::GetIdx(uint64_t x, uint64_t y, uint64_t z)
{
    uint64_t idx    = params.w * (5 * y + x) + z;
    uint64_t byte   = idx / 8;
    uint64_t shift  = idx % 8;
    uint8_t mask    = 1 << shift;
    uint8_t val     = (state[byte] & mask) >> shift;

    return val;
}

/**
 * SHA3::SetIdx - Set a value in the state array at specified
 * x, y, z position.
 *
 * @param x    [in] x coord.
 * @param y    [in] y coord.
 * @param z    [in] z coord.
 * @param val  [in] Value to set.
 */

void SHA3::SetIdx(uint64_t x, uint64_t y, uint64_t z, uint8_t val)
{
    if (val != 0 || val != 1)
        throw invalid_argument("Expected 0 or 1 input when setting SHA3 data");

    uint64_t idx    = params.w * (5 * y + x) + z;
    uint64_t byte   = idx / 8;
    uint64_t shift  = idx % 8;

    state[byte] |= (val << shift);
}

/**
 * MsgStreamer::SetData - Set data to be hashed in the message
 * streamer.
 *
 * @param dataIn    [in] Data to be hashed.
 */

void MsgStreamer::SetData(vector<uint8_t>& dataIn)
{
    data.resize(dataIn.size());
    memcpy(&data[0], &dataIn[0], data.size());
}

/**
 * MsgStreamer::Reset - Reset the streamer to the beginning of the
 * message.
 */

void MsgStreamer::Reset()
{
    offset = 0;
}

/**
 * MsgStreamer::Next - Grab the next data block from the stream and pad
 * with zeros to fit the sponge input size.
 *
 * @param blockOut    [in/out]  Byte array to fill with padded msg block.
 */

void MsgStreamer::Next(vector<uint8_t>& blockOut)
{
    if (blockOut.size() != b / 8)
        throw invalid_argument("Wrong output block size passed in while streaming SHA3 message blocks.");

    memset(&blockOut[0], 0, b / 8);

    if (offset + (r / 8) < data.size())
    {
        memcpy(&blockOut[0], &data[offset], r / 8);
        offset += r / 8;
    }
    else
    {
        memcpy(&blockOut[0], &data[offset], data.size() - offset - 1);
        offset = data.size() - 1;
    }
}

/**
 * MsgStreamer::End - True if the streamer has reached the end of the
 * message.
 * 
 * @return True if at the end of the message, false otherwise.
 */

bool MsgStreamer::End()
{
    if (offset == data.size() - 1)
        return true;

    return false;
}

/**
 * SHA3::SHA3 - Constructor. Params will be initialized to Keccak[1600] with
 * d = 512, r = 576, c = 1024, w = 64, and n = 24. Iniitialize the state
 * array size to 5 x 5 x 64 bits by default.
 *
 * @param dataIn    [in]
 */

SHA3::SHA3()
{
    state.resize(STATE_W * STATE_H * params.w / 8, 0);
}

/**
 * SHA3::SHA3 - Constructor. Take user-specified SHA3 params and initialize
 * the state array accordingly.
 *
 * @param paramsIn    [in]
 */

SHA3::SHA3(SHA3Params& paramsIn) : params(paramsIn), stream(params.r, params.b)
{
    if (!(params.sz == SHA512 &&
          params.d  == 512 &&
          params.r  == 576 &&
          params.c  == 1024 &&
          params.b  == 1600 &&
          params.w  == 64 &&
          params.l  == 6))
    {
        throw invalid_argument("Unexpected SHA3 config params. Only SHA3-512 currently" \
            "supported");
    }

    state.resize(STATE_W * STATE_H * params.w / 8, 0);
}

/**
 * SHA3::SpongeAbsorbBlock - XOR a message block into the state array
 * and run Keccak permutations on it.
 *
 * @param block    [in] New block to absorb into the sponge.
 */

void SHA3::SpongeAbsorbBlock(vector<uint8_t>& block)
{
    if (block.size() != state.size())
        throw invalid_argument("SHA3 sponge input block doesn't match state array size");

    for (uint64_t i = 0; i < state.size(); i++)
        state[i] ^= block[i];

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
        Theta();
        Rho();
        Pi();
        Chi();
        Iota();
    }
}

/**
 * SHA3::SpongeSqueezeBlock - For each output hash block requested,
 * apply the Keccak permutations before reading out bits.
 */

void SHA3::SpongeSqueezeBlock()
{
    ApplyKeccak();
}

/**
 * SHA3::Theta - Theta step transformtion for SHA3.
 *
 * @param block    [in] Input block to scramble.
 */

void SHA3::Theta()
{
    vector<uint8_t> tmp(STATE_W * STATE_H * params.w / 8);

    for (uint64_t x = 0; x < STATE_W; x++)
    {
        for (uint64_t z = 0; z < params.w; z++)
        {
            uint8_t xHi = (x == STATE_W - 1) ? 0 : x + 1;
            uint8_t xLo = (x == 0) ? STATE_W - 1 : x - 1;

            uint8_t zHi = z;
            uint8_t zLo = (z == 0) ? params.w : z - 1;

            uint8_t c1  = 0;
            uint8_t c2  = 0;

            for (uint64_t y = 0; y < STATE_H; y++)
            {
                c1 ^= GetIdx(xHi, y, zLo);
                c2 ^= GetIdx(xLo, y, z);
            }

            uint8_t d = c1 ^ c2;
        }
    }
}

/**
 * SHA3::Rho - Rho step transformtion for SHA3.
 *
 * @param block    [in] Input block to scramble.
 */

void SHA3::Rho()
{
    const uint64_t laneOffsets[5][5] =
    {
        0,    1, 190,  28,  91,
        36, 300,   6,  55, 276,
        3,   10, 171, 153, 231,
        105, 45,  15,  21, 136,
        210, 66, 253, 120,  78
    };

    const uint8_t hiMasks[8] = { 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
    const uint8_t loMasks[8] = { 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01 };

    for (uint64_t x = 0; x < STATE_W; x++)
    {
        for (uint64_t y = 0; y < STATE_H; y++)
        {
            uint64_t laneShift      = laneOffsets[x][y] % params.w;
            uint64_t laneStart      = STATE_IDX(x, y, 0, params.w) / 8;

            uint64_t shiftBytes     = laneShift / 8;
            uint64_t shiftCarry     = laneShift % 8;

            uint8_t maskHi          = hiMasks[shiftCarry];
            uint8_t maskLo          = loMasks[shiftCarry];

            vector<uint8_t> rotatedLane(8);

            for (uint64_t i = 0; i < params.w / 8; i++)
            {
                uint64_t targetHi = (shiftBytes + i + ((laneShift % 8) ? 1 : 0) % params.w);
                uint64_t targetLo = (shiftBytes + i) % params.w;

                rotatedLane[targetHi] |= ((state[laneStart + i] & maskHi) >> (8 - shiftCarry));
                rotatedLane[targetLo] |= (state[laneStart + i] & maskLo) << shiftCarry;
            }

            memcpy(&state[laneStart], &rotatedLane[0], params.w / 8);
        }
    }
}

/**
 * SHA3::Pi - Pi step transformtion for SHA3.
 *
 * @param block    [in] Input block to scramble.
 */

void SHA3::Pi()
{
    vector<uint8_t> tmp(params.b / 8);

    for (uint64_t x = 0; x < STATE_W; x++)
    {
        for (uint64_t y = 0; y < STATE_H; y++)
        {
            for (uint64_t z = 0; z < params.w; z++)
            {
                uint64_t idxIn      = STATE_IDX(x, y, z, params.w);
                uint64_t byteIn     = idxIn / 8;
                uint64_t shiftIn    = idxIn % 8;
                uint8_t maskIn      = (1 << shiftIn);

                uint64_t idxOut     = STATE_IDX((x + 3 * y) % STATE_W, x, z, params.w);
                uint64_t byteOut    = idxIn / 8;
                uint64_t shiftOut   = idxIn % 8;
                uint8_t maskOut     = (1 << shiftOut);

                if (shiftIn > shiftOut)
                    tmp[byteOut] |= ((state[byteIn] & maskIn) << (shiftOut - shiftIn));
                else
                    tmp[byteOut] |= ((state[byteIn] & maskIn) >> (shiftIn - shiftOut));
            }
        }
    }

    memcpy(&state[0], & tmp[0], params.b / 8);
}

/**
 * SHA3::Chi - Chi step transformtion for SHA3.
 *
 * @param block    [in] Input block to scramble.
 */

void SHA3::Chi()
{
    vector<uint8_t> tmp(params.b / 8);

    for (uint64_t x = 0; x < STATE_W; x++)
    {
        for (uint64_t y = 0; y < STATE_H; y++)
        {
            for (uint64_t z = 0; z < params.w; z++)
            {
                uint64_t idx1       = STATE_IDX(x, y, z, params.w);
                uint64_t byte1      = idx1 / 8;
                uint64_t shift1     = idx1 % 8;
                uint8_t mask1       = (1 << shift1);
                uint8_t a1;

                uint64_t idx2       = STATE_IDX((x + 1) % STATE_W, y, z, params.w);
                uint64_t byte2      = idx2 / 8;
                uint64_t shift2     = idx2 % 8;
                uint8_t mask2       = (1 << shift2);

                uint64_t idx3       = STATE_IDX((x + 2) % STATE_W, y, z, params.w);
                uint64_t byte3      = idx3 / 8;
                uint64_t shift3     = idx3 % 8;
                uint8_t mask3       = (1 << shift3);
            }
        }
    }

    memcpy(&state[0], &tmp[0], params.b / 8);
}

/**
 * SHA3::Iota - Iota step transformtion for SHA3.
 *
 * @param block    [in] Input block to scramble.
 */

void SHA3::Iota()
{

}

/**
 * SHA3::Hash - Apply a SHA3 hash to an input message.
 *
 * @param data    [in]      Input message to be hashed.
 * @param hashOut [in/out]  Computed output hash.
 */

void SHA3::Hash(vector<uint8_t>& data, vector<uint8_t>& hashOut)
{
    if (hashOut.size() != 0)
        throw invalid_argument("Expected output hash array to be empty when calling SHA3.");

    stream.SetData(data);
    vector<uint8_t> curBlock(params.b / 8);

    while (!stream.End())
    {
        stream.Next(curBlock);
        SpongeAbsorbBlock(curBlock);
    }

    ApplyKeccak();

    for (uint64_t i = 0; i < params.d / 8; i++)
        hashOut.push_back(state[i]);

    return;
}