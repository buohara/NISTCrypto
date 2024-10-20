#include "msgstreamer.h"

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
        if (bLittleEndian)
            memcpy(&data[0], &dataIn[0], sizeIn);
        else
            for (uint64_t i = sizeIn; i-- > 0;)
                data[sizeIn - i - 1] = dataIn[i];

    data[sizeIn]        |= 0x06;
    data[sizeIn - 1]    |= 0x80;
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