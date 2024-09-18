#include "hash.h"

#define STATE_IDX(x, y, z, w) ((w) * (5 * (y) + (x)) + (z))

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
    state.resize(5 * 5 * params.w / 8, 0);
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

    state.resize(5 * 5 * params.w / 8, 0);
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
        Theta(state);
        Rho(state);
        Pi(state);
        Chi(state);
        Iota(state);
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

void SHA3::Theta(vector<uint8_t>& block)
{
    for (uint64_t x = 0; x < 5; x++)
    {
        for (uint64_t z = 0; z < params.w; z++)
        {
            uint8_t c = 0;

            for (uint64_t y = 0; y < 5; y++)
            {
                uint64_t arrayIdx   = STATE_IDX(x, y, z, params.w);
                uint64_t byte       = arrayIdx / 8;
                uint64_t shift      = arrayIdx % 8;

                uint8_t mask        = (1 << shift);

                if (y == 0)
                    c = state[byte] & mask;
                else
                    c ^= state[byte] & mask;
            }
        }
    }
}

/**
 * SHA3::Rho - Rho step transformtion for SHA3.
 *
 * @param block    [in] Input block to scramble.
 */

void SHA3::Rho(vector<uint8_t>& block)
{
}

/**
 * SHA3::Pi - Pi step transformtion for SHA3.
 *
 * @param block    [in] Input block to scramble.
 */

void SHA3::Pi(vector<uint8_t>& block)
{

}

/**
 * SHA3::Chi - Chi step transformtion for SHA3.
 *
 * @param block    [in] Input block to scramble.
 */

void SHA3::Chi(vector<uint8_t>& block)
{

}

/**
 * SHA3::Iota - Iota step transformtion for SHA3.
 *
 * @param block    [in] Input block to scramble.
 */

void SHA3::Iota(vector<uint8_t>& block)
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