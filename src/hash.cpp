#include "hash.h"

/**
 * MsgStreamer::SetData - Set data to be hashed in the message
 * streamer.
 *
 * @param dataIn    [in] Data to be hashed.
 */

void MsgStreamer::SetData(vector<uint8_t>& dataIn)
{
    const uint8_t padBytes  = dataIn.size() % 200;

    data.resize(dataIn.size() + padBytes);
    memcpy(&data[0], &dataIn[0], data.size());
    
    data[dataIn.size()]     = 0x80;
    data[data.size() - 1]   = 0x1;
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
 * with zeros to fit sponge input size.
 *
 * @param blockOut    [in/out]  Byte array to fill with padded msg block.
 */

void MsgStreamer::Next(vector<uint8_t>& blockOut)
{
    uint8_t rateBytes = r / 8;

    if (blockOut.size() != rateBytes)
        throw invalid_argument("Wrong output block size passed in while streaming SHA3 message blocks.");

    if (offset == data.size())
        throw out_of_range("Tried accessing past end of SHA3 input message.");

    memcpy(&blockOut[0], &data[offset], rateBytes);
    offset += rateBytes;
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
 * SHA3::Get - Get a value from the state array at specified
 * x, y, z position.
 *
 * @param x    [in] x coord.
 * @param y    [in] y coord.
 * @param z    [in] z coord.
 * 
 * @return Value of state array at specified location.
 */

uint8_t SHA3::GetBit(uint64_t x, uint64_t y, uint64_t z)
{
    uint64_t idx    = STATE_IDX(x, y, z);
    uint64_t byte   = idx / 8;
    uint64_t shift  = idx % 8;
    uint8_t mask    = 1 << shift;
    uint8_t val     = (state[byte] & mask) >> shift;

    return val;
}

/**
 * SHA3::SetBit - Set a bit in the state array at specified
 * x, y, z position.
 *
 * @param x    [in] x coord.
 * @param y    [in] y coord.
 * @param z    [in] z coord.
 */

void SHA3::SetBit(uint64_t x, uint64_t y, uint64_t z)
{
    uint64_t idx    = STATE_IDX(x, y, z);
    uint64_t byte   = idx / 8;
    uint64_t shift  = idx % 8;

    state[byte]     |= (1 << shift);
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

void SHA3::SetBit(vector<uint8_t> &arrayIn, uint64_t x, uint64_t y, uint64_t z)
{
    if (arrayIn.size() != state.size())
        throw invalid_argument("Unexpected input array size when setting x, y, z val.");

    uint64_t idx    = STATE_IDX(x, y, z);
    uint64_t byte   = idx / 8;
    uint64_t shift  = idx % 8;

    arrayIn[byte]   |= (1 << shift);
}

/**
 * SHA3::ClearBit - Clear a bit in the state array at specified
 * x, y, z position.
 *
 * @param x    [in] x coord.
 * @param y    [in] y coord.
 * @param z    [in] z coord.
 */

void SHA3::ClearBit(uint64_t x, uint64_t y, uint64_t z)
{
    uint64_t idx    = STATE_IDX(x, y, z);
    uint64_t byte   = idx / 8;
    uint64_t shift  = idx % 8;

    state[byte] &= ~(1 << shift);
}

/**
 * SHA3::ClearBit - Clear a bit in a user-provided array. Array
 * assumed to match state array size on input.
 *
 * @param arrayIn   [in/out]    Set value into this array.
 * @param x         [in]        x coord.
 * @param y         [in]        y coord.
 * @param z         [in]        z coord.
 */

void SHA3::ClearBit(vector<uint8_t>& arrayIn, uint64_t x, uint64_t y, uint64_t z)
{
    if (arrayIn.size() != state.size())
        throw invalid_argument("Unexpected input array size when setting x, y, z val.");

    uint64_t idx = STATE_IDX(x, y, z);
    uint64_t byte = idx / 8;
    uint64_t shift = idx % 8;

    arrayIn[byte] &= ~(1 << shift);
}

/**
 * SHA3::GetRow - Get row from state array at specified
 * (y, z) position.
 *
 * @param y     [in]    Row y coord.
 * @param z     [in]    Row z coord.
 *
 * @return  Bits in specified row.
 */

uint8_t SHA3::GetRow(const uint64_t y, const uint64_t z)
{
    if (y >= STATE_H || z >= STATE_L)
        throw out_of_range("Specified (y,z) position out of range when accessing state array row.");

    uint8_t out = 0;

    for (uint64_t x = 0; x < STATE_W; x++)
        out |= GetBit(x, y, z) << x;

    return out;
}

/**
 * SHA3::GetColumn - Get column from state array at specified
 * (x, z) position.
 *
 * @param x     [in]    Column x coord.
 * @param z     [in]    Column z coord.
 */

uint8_t SHA3::GetColumn(const uint64_t x, const uint64_t z)
{
    if (x >= STATE_W || z >= STATE_L)
        throw out_of_range("Specified (x,y) position out of range when accessing state array column.");

    uint8_t out = 0;

    for (uint64_t y = 0; y < STATE_W; y++)
        out |= GetBit(x, y, z) << y;

    return out;
}

/**
 * SHA3::GetLane - Get lane from state array at specified
 * (x, y) position.
 *
 * @param x         [in]        x coord.
 * @param y         [in]        y coord.
 * @param laneOut   [in/out]    Output lane.
 */

void SHA3::GetLane(const uint64_t x, const uint64_t y, vector<uint8_t>& laneOut)
{
    if (x >= STATE_W || y >= STATE_H)
        throw out_of_range("Specified (x,y) position out of range when accessing state array lane.");

    laneOut.resize(STATE_L / 8, 0);

    for (uint64_t z = 0; z < STATE_L; z++)
    {
        uint8_t byte    = (uint8_t)z / 8;
        uint8_t shift   = (uint8_t)z % 8;

        laneOut[byte] |= (GetBit(x, y, z) << shift);
    }
}

/**
 * SHA3::SetRow - Set row values in the state array at specified
 * (y, z) position.
 *
 * @param y         [in]        y coord.
 * @param z         [in]        z coord.
 * @param val       [in]        Value to set.
 */

void SHA3::SetRow(const uint64_t y, const uint64_t z, uint8_t val)
{
    if (y >= STATE_H || z >= STATE_L)
        throw out_of_range("Specified (y,z) position out of range when setting state array row.");

    if (val >= 0x20)
        throw invalid_argument("Invalid row value specified when setting state row.");

    for (uint64_t x = 0; x < STATE_H; x++)
        if (val & (1 << x))
            SetBit(x, y, z);
}

/**
 * SHA3::SetColumn - Set a value in a user-provided array. Array
 * assumed to match state array size on input.
 *
 * @param x         [in]        x coord.
 * @param z         [in]        z coord.
 * @param val       [in]        Value to set.
 */

void SHA3::SetColumn(const uint64_t x, const uint64_t z, uint8_t val)
{
    if (x >= STATE_W || z >= STATE_L)
        throw out_of_range("Specified (x,y) position out of range when setting state array column.");

    if (val >= 0x20)
        throw invalid_argument("Invalid column value specified when setting state column.");

    for (uint64_t y = 0; y < STATE_H; y++)
        if (val & (1 << y))
            SetBit(x, y, z);
}

/**
 * SHA3::SetLane - Set a value in a user-provided array. Array
 * assumed to match state array size on input.
 *
 * @param x         [in]        x coord.
 * @param y         [in]        y coord.
 * @param val       [in]        Value to set.
 */

void SHA3::SetLane(const uint64_t x, const uint64_t y, vector<uint8_t>& val)
{
    if (x >= STATE_W || y >= STATE_H)
        throw out_of_range("Specified (x,y) position out of range when setting state array lane.");

    if (val.size() > STATE_L / 8)
        throw invalid_argument("Invalid lane value specified when setting state lane.");

    for (uint64_t z = 0; z < STATE_L; z++)
    {
        uint8_t byte    = (uint8_t)z / 8;
        uint8_t shift   = (uint8_t)z % 8;
        uint8_t bit     = val[byte] & (1 << shift);

        if (bit)
            SetBit(x, y, z);
    }
}

/**
 * SHA3::SHA3 - Constructor. Params will be initialized to Keccak[1600] with
 * d = 512, r = 576, c = 1024, w = 64, and n = 24. Iniitialize the state
 * array size to 5 x 5 x 64 bits by default.
 */

SHA3::SHA3()
{
    state.resize(STATE_W * STATE_H * STATE_L / 8, 0);
}

/**
 * SHA3::SHA3 - Constructor. Initialize default parameters based on input
 * size;
 */

SHA3::SHA3(SHASize sz)
{
    if (sz == SHA224)
        params.d = 224;

    if (sz == SHA256)
        params.d = 256;

    if (sz == SHA384)
        params.d = 384;

    if (sz == SHA512)
        params.d = 512;

    params.b    = 1600;
    params.c    = 2 * params.d;
    params.r    = params.b - params.c;
    params.sz   = sz;
    params.w    = 64;
    params.l    = 6;
    params.n    = 12 + 2 * params.l;

    state.resize(STATE_W * STATE_H * params.w / 8, 0);
}

/**
 * SHA3::SHA3 - Constructor. Take user-specified SHA3 params and initialize
 * the state array accordingly.
 *
 * @param paramsIn    [in] List if SHA3 hashing parameters.
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
 * SHA3::Size - Return the size of the state array in bytes.
 */

uint64_t SHA3::Size()
{
    return STATE_W * STATE_H * STATE_L / 8;
}

/**
 * SHA3::ClearState - Clear the state array to zeros.
 */

void SHA3::ClearState()
{
    memset(&state[0], 0, Size());
}

/**
 * SHA3::PrintState - Print the state array.
 */

void SHA3::PrintState()
{
    printf("SHA3 State Array:\n\n");

    for (uint8_t x = 0; x < STATE_W; x++)
    {
        printf("x[%lu]\n\n", x);
        for (uint8_t y = 0; y < STATE_H; y++)
        {
            printf("y[%lu]: ", y);
            for (uint8_t z = 0; z < STATE_L; z++)
            {
                uint64_t idx    = STATE_IDX(x, y, z);
                uint8_t byte    = idx / 8;
                uint8_t bit     = idx % 8;

                printf("%lu", (state[byte] & (1 << bit)) >> bit);
            }
            printf("\n");

        }
        printf("\n");

    }
}

/**
 * SHA3::SpongeAbsorbBlock - XOR a message block into the state array
 * and run Keccak permutations on it.
 *
 * @param block    [in] New block to absorb into the sponge.
 */

void SHA3::SpongeAbsorbBlock(vector<uint8_t>& block)
{
    const uint8_t rateBytes = params.r / 8;

    if (block.size() != rateBytes)
        throw invalid_argument("SHA3 sponge input block doesn't match state array size");

    for (uint64_t i = 0; i < rateBytes; i++)
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
        Iota(i);
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
 */

void SHA3::Theta()
{
    vector<uint8_t> tmp = state;

    for (uint8_t x = 0; x < STATE_W; x++)
    {
        for (uint8_t z = 0; z < STATE_L; z++)
        {
            uint8_t xHi = (x == STATE_W - 1) ? 0 : x + 1;
            uint8_t xLo = (x == 0) ? STATE_W - 1 : x - 1;
            uint8_t zLo = (z == 0) ? (uint8_t)STATE_L - 1 : z - 1;

            uint8_t c1  = 0;
            uint8_t c2  = 0;

            for (uint64_t y = 0; y < STATE_H; y++)
            {
                c1 ^= GetBit(xHi, y, zLo);
                c2 ^= GetBit(xLo, y, z);
            }

            uint8_t d = c1 ^ c2;

            for (uint64_t y = 0; y < STATE_H; y++)
            {
                uint8_t a = GetBit(x, y, z);
                a ^= d;

                if (a)
                    SetBit(tmp, x, y, z);
                else
                    ClearBit(tmp, x, y, z);
            }
        }
    }

    state = tmp;
}

/**
 * SHA3::Rho - Rho step transformtion for SHA3.
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
            uint64_t laneShift      = laneOffsets[x][y] % STATE_L;
            uint64_t laneStart      = STATE_IDX(x, y, 0) / 8;

            uint64_t shiftBytes     = laneShift / 8;
            uint64_t shiftCarry     = laneShift % 8;

            uint8_t maskHi          = hiMasks[shiftCarry];
            uint8_t maskLo          = loMasks[shiftCarry];

            vector<uint8_t> rotatedLane(8);

            for (uint64_t i = 0; i < STATE_L / 8; i++)
            {
                uint64_t targetHi = (i + shiftBytes + (shiftCarry ? 1 : 0)) % (STATE_L / 8);
                uint64_t targetLo = (i + shiftBytes) % (STATE_L / 8);

                rotatedLane[targetHi] |= ((state[laneStart + i] & maskHi) >> (8 - shiftCarry));
                rotatedLane[targetLo] |= (state[laneStart + i] & maskLo) << shiftCarry;
            }

            memcpy(&state[laneStart], &rotatedLane[0], STATE_L / 8);
        }
    }
}

/**
 * SHA3::Pi - Pi step transformtion for SHA3.
 */

void SHA3::Pi()
{
    vector<uint8_t> tmp = state;

    for (uint64_t x = 0; x < STATE_W; x++)
    {
        for (uint64_t y = 0; y < STATE_H; y++)
        {
            for (uint64_t z = 0; z < STATE_L; z++)
            {
                uint8_t in = GetBit(x, y, z);

                if (in)
                    SetBit(tmp, (x + 3 * y) % STATE_W, x, z);
                else
                    ClearBit(tmp, (x + 3 * y) % STATE_W, x, z);
            }
        }
    }

    state = tmp;
}

/**
 * SHA3::Chi - Chi step transformtion for SHA3.
 */

void SHA3::Chi()
{
    vector<uint8_t> tmp = state;

    for (uint64_t x = 0; x < STATE_W; x++)
    {
        for (uint64_t y = 0; y < STATE_H; y++)
        {
            for (uint64_t z = 0; z < STATE_L; z++)
            {
                uint8_t a1 = GetBit(x, y, z);
                uint8_t a2 = GetBit((x + 1) % STATE_W, y, z);
                uint8_t a3 = GetBit((x + 2) % STATE_W, y, z);

                a1 ^= a2;
                a1 *= a3;

                if (a1)
                    SetBit(tmp, x, y, z);
                else
                    ClearBit(tmp, x, y, z);
            }
        }
    }

    state = tmp;
}

/**
 * RC - Round constant (RC) function used in Iota step transformation.
 *
 * @param t    [in] Input parameter for RC.
 * 
 * @return RC(t).
 */

static uint8_t RC(const uint64_t t)
{
    if (!(t % 255))
        return 1;

    vector<uint8_t> r = { 1, 0, 0, 0, 0, 0, 0, 0 };

    for (uint64_t i = 0; i < t % 255; i++)
    {
        r.insert(r.begin(), 0);
        
        r[0] = r[0] ^ r[8];
        r[4] = r[4] ^ r[8];
        r[5] = r[5] ^ r[8];
        r[6] = r[6] ^ r[8];
        
        r.resize(8);
    }

    return r[0];
}

/**
 * SHA3::Iota - Iota step transformtion for SHA3.
 *
 * @param round    [in] Round number needed in this step transformation.
 */

void SHA3::Iota(uint64_t round)
{
    vector<uint8_t> rcs(params.w, 0);
    vector<uint8_t> tmp         = state;
    const uint8_t indices[7]    = { 0, 1, 3, 7, 15, 31, 63 };

    for (uint64_t j = 0; j <= params.l; j++)
        rcs[indices[j]] = RC(j + 7 * round);

    for (uint64_t z = 0; z < params.w; z++)
    {
        uint8_t a   = GetBit(0, 0, z);
        a           ^= rcs[z];

        if (a)
            SetBit(tmp, 0, 0, z);
        else
            ClearBit(tmp, 0, 0, z);
    }

    state = tmp;
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
    vector<uint8_t> curBlock(params.r / 8);

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