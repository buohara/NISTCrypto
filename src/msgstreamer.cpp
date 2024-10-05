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
 * @param dataIn    [in] Message data to be hashed as a little endian byte array.
 */

void SHAStreamer::SetData(vector<uint8_t>& dataIn)
{
    const uint64_t rateBytes    = r / 8;
    uint64_t padBytes           = (rateBytes - dataIn.size() % rateBytes);

    if (padBytes == 0)
        padBytes += rateBytes;

    data.resize(dataIn.size() + padBytes);

    if (dataIn.size())
        memcpy(&data[0], &dataIn[0], dataIn.size());

    data[dataIn.size()]     |= 0x06;
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
 * AESStreamer::SetData - Set message data in the AES streamer, which feeds
 * 16 byte blocks into encryption/decryption routines. Pad to 16 byte boundaries,
 * each pad byte is the number of pad bytes added.
 *
 * @return True if at the end of the message, false otherwise.
 */

void AESStreamer::SetData(vector<uint8_t>& dataIn)
{
    const uint32_t rateBytes    = 16;
    const uint8_t padBytes      = (uint8_t)(rateBytes - dataIn.size() % rateBytes);

    if (dataIn.size())
        memcpy(&data[0], &dataIn[0], dataIn.size());

    for (uint32_t i = 0; i < padBytes; i++)
        data.push_back(padBytes);
}

/**
 * AESStreamer::Reset - Set the streamer back to the beginning of the message.
 */

void AESStreamer::Reset()
{
    offset = 0;
}

/**
 * AESStreamer::Next - Get the next 16 message bytes.
 */

void AESStreamer::Next(uint32_t block[4])
{
    assert((offset + 16) <= data.size());
    memcpy(block, &data[offset], 16);
    offset += 16;
}

/**
 * AESStreamer::End - Return true if input all message blocks have been
 * processed.
 *
 * @return True if at the end of the message, false otherwise.
 */

bool AESStreamer::End()
{
    if (offset == data.size())
        return true;

    return false;
}