#include "../../incs/nm_otool.h"

/*
** For signed integer
** & on right shift because MSB of 1 set 1 on every bit
*/

/*
** Swap endianness function for unsigned 16 bits integer
*/
uint16_t ifSwapuInt16(int8_t swap, uint16_t val)
{
    if (swap)
        val = (val << 8) | (val >> 8);
    return (val);
}

/*
** Swap endianness function for signed 16 bits integer
*/
int16_t ifSwapInt16(int8_t swap, int16_t val)
{
    if (swap)
        val = (val << 8) | ((val >> 8) & 0x00FF);
    return (val);
}

/*
** Swap endianness function for unsigned 32 bits integer
*/
uint32_t ifSwapuInt32(int8_t swap, uint32_t val)
{
    if (swap) {
        val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0x00FF00FF);
        val = (val << 16) | (val >> 16);
    }
    return (val);
}

/*
** Swap endianness function for signed 32 bits integer
*/
int32_t ifSwapInt32(int8_t swap, int32_t val)
{
    if (swap) {
        val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0x00FF00FF);
        val = (val << 16) | ((val >> 16) & 0x0000FFFF );
    }
    return (val);
}

/*
** Swap endianness function for unsigned 64 bits integer
*/
uint64_t ifSwapuInt64(int8_t swap, uint64_t val)
{
    if (swap) {
        val = ((val << 8) & 0xFF00FF00FF00) | ((val >> 8) & 0x00FF00FF00FF);
        val = ((val << 16) & 0xFFFF0000FFFF0000) | ((val >> 16) & 0x0000FFFF0000FFFF);
        val = (val << 32) | (val >> 32);
    }
    return (val);
}

/*
** Swap endianness function for signed 64 bits integer
*/
int64_t ifSwapInt64(int8_t swap, int64_t val)
{
    if (swap) {
        val = ((val << 8) & 0xFF00FF00FF00) | ((val >> 8) & 0x00FF00FF00FF);
        val = ((val << 16) & 0xFFFF0000FFFF0000) | ((val >> 16) & 0x0000FFFF0000FFFF);
        val = (val << 32) | ((val >> 32) & 0x00000000FFFFFFFF);
    }
    return (val);
}