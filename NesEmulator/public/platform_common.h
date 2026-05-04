#pragma once

#include <vector>
using std::vector;

using byte = unsigned char;
using program = vector<byte>;

// 寻址方式
enum AddressingMode:byte
{
    Immediate,
    ZeroPage,
    ZeroPage_X,
    ZeroPage_Y,
    Absolute,
    Absolute_X,
    Absolute_Y,
    Indirect_X,
    Indirect_Y,
    NoneAddressing
};

const int MEM_SIZE = 0x10000; // 6502CPU是16位的CPU，最多可以使用2**16 次方，寻址用2个字节，最多65536字节，也就是64 KB内存
