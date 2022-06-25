#include "types.hh"
#include <cstring>
#include <iostream>

EncVarInt encodeVarInt(int x)
{
    unsigned int value = 0;
    int i;
    char buf[5];
    for (i = 0; i < 5; i++)
    {
        if ((value & ~0x7F) == 0)
        {
            buf[i] = value;
            break;
        }
        buf[i] = ((value & 0x7F) | 0x80);
        value >>= 7;
    }
    return EncVarInt{
        .len = i,
        .enc = buf
    };
}

UTF8String encodeUTF8String(std::string str) {
    return UTF8String{
        .len = static_cast<int>(str.size()),
        .str = str
    };
}

VarInt readVarInt(char *buf, const int off)
{
    int val, i, pos;
    val = 0;
    pos = 0;
    for (i = 0; i < 5; i++)
    {
        char curByte = buf[i + off];
        val |= (curByte & 0x7F) << pos;
        if ((curByte & 0x80) == 0)
            break;
        pos += 7;
        if (pos >= 32)
            throw "VarInt too big";
    }
    return VarInt{
        .len = i + off,
        .num = val};
}

VarLong readVarLong(char *buf, const int off)
{
    int val, i, pos;
    val = 0;
    pos = 0;
    for (i = 0; i < 5; i++)
    {
        char curByte = buf[i + off];
        val |= (curByte & 0x7F) << pos;
        if ((curByte & 0x80) == 0)
            break;
        pos += 7;
        if (pos >= 32)
            throw "VarLong too big";
    }
    return VarLong{
        .len = i + off,
        .num = val};
}

UTF8String readUTF8String(char *buf, const int off)
{
    auto stringSize = readVarInt(buf, off);
    char str[stringSize.num + 1];
    std::memset(str, 0, sizeof(str));
    for (int i = 0; i < 9; i++)
    {
        str[i] = buf[stringSize.len + 1 + i];
    }
    return UTF8String{
        .len = stringSize.len + stringSize.num,
        .str = std::string(str)};
}

UShort readUShort(char *buf, const int off)
{
    // swap if other endianess
    return ((unsigned short)buf[off] << 8) | (unsigned char)buf[off + 1];
}