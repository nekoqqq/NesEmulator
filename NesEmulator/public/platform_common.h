#pragma once

#include <vector>
using std::vector;

using byte = unsigned char;
using word = uint16_t;
using program = vector<byte>;

// 寻址方式
enum AddressingMode:byte
{
    Implied, // 隐含寻址，操作数隐含在指令中，无需其他指定
    Accumulator, // 直接操作ACC，比如LSR A，左移操作数
    Immediate, // 立即寻址，数据在操作码中
    ZeroPage,
    ZeroPage_X,
    ZeroPage_Y,
    Absolute,
    Absolute_X,
    Absolute_Y,
    Indirect_X,
    Indirect_D_Y, // 不是和Indirect,X对应的，而是(Indirect),Y
    NoneAddressing
};

const int MEM_SIZE = 0x10000; // 6502CPU是16位的CPU，最多可以使用2**16 次方，寻址用2个字节，最多65536字节，也就是64 KB内存
