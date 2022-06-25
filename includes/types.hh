#include <string>
#include "sockpp/tcp_socket.h"

/**
 * @brief Used for storing VarInts
 */
struct VarInt
{
    int len, num;
};

struct EncVarInt
{
    int len;
    char *enc;
};

// Alias VarLong to VarInt
using VarLong = VarInt;

struct UTF8String
{
    int len;
    std::string str;
};

using UShort = unsigned short;

/**
 * @brief Read VarInt from buffer
 *
 * @param buf buffer containing the data
 * @param off optional offset from buf
 * @return VarInt
 */
VarInt readVarInt(char *buf, const int off = 0);

/**
 * @brief Read VarLong from buffer
 *
 * @param buf buffer containing data
 * @param off optional offset from buf
 * @return VarLong
 */
VarLong readVarLong(char *buf, const int off = 0);

/**
 * @brief Read UTF8String from buffer
 *
 * @param buf buffer containing data
 * @param off optional offset from buf
 * @return UTF8String
 */
UTF8String readUTF8String(char *buf, const int off = 0);
UShort readUShort(char *buf, const int off = 0);
EncVarInt encodeVarInt(int x);
UTF8String encodeUTF8String(std::string str);