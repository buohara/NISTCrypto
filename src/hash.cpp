#include "hash.h"

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

/**
 * MsgStreamer::SetData - Set data to be hashed in the message
 * streamer. The streamer feeds message blocks whose bit size is
 * the SHA3 rate (see FIPS 202. Rate = 1600 bits - 2 * capacity = 
 * 1600 - 2 * (output MD output length in bits)).
 * 
 * The FIPS 202 spec says messages are to be padded using the "Pad10*1" rule.
 * Official NIST test messages, however, add a 0x6 suffix after the last message bit.
 * No explanation for this discrepency has been given. For testing conformance, 
 * append 0x6 here.
 * 
 * @param dataIn    [in] Message data to be hashed as a little endian byte array.
 */

void MsgStreamer::SetData(vector<uint8_t>& dataIn)
{
    const uint64_t rateBytes    = r / 8;
    uint64_t padBytes           = (rateBytes - dataIn.size() % rateBytes);

    if (padBytes == 0)
        padBytes += rateBytes;

    data.resize(dataIn.size() + padBytes);

    if (dataIn.size())
        memcpy(&data[0], &dataIn[0], dataIn.size());

    data[dataIn.size()]         |= 0x06;
    data[data.size() - 1]       |= 0x80;
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
 * MsgStreamer::Next - Get the next message block to feed into the
 * SHA3 sponge.
 *
 * @param blockOut    [in/out] 
 */

void MsgStreamer::Next(vector<uint64_t>& blockOut)
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
 * MsgStreamer::End - Return true of input all message blocks have been
 * processed.
 *
 * @return True if at the end of the message, false otherwise.
 */

bool MsgStreamer::End()
{
    if (offset == data.size())
        return true;

    return false;
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
    stream      = MsgStreamer(params.r);

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

void SHA3::PrintState(PrintMode mode)
{
    if (1)
    {
        for (uint64_t x = 0; x < STATE_W; x++)
        {
            for (uint64_t y = 0; y < STATE_H; y++)
                printf("(%llu, %llu) = 0x%016llx\n", x, y, state[LANE(x, y)]);

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
 * @param data    [in]      Input message to be hashed.
 * @param md      [in/out]  MD to populate as a byte vector. Assumed empty on input.
 */

void SHA3::Hash(vector<uint8_t>& data, vector<uint8_t>& md)
{
    assert(md.size() == 0);

    ClearState();
    stream.SetData(data);

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