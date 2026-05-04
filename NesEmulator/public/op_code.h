/*
 *指令码
 */

#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "platform_common.h"
using std::string;
using std::vector;
using std::unordered_map;

// 每一种类型的操作指令
struct OpCode
{
    OpCode(byte code = 0, string mnemonic = "", int size = 0, int cycles = 0, AddressingMode mode = NoneAddressing);

    byte code;
    string mnemonic; // todo 如何只做一份string
    int size;
    int cycles;
    AddressingMode mode;

    // 所有指令都共享的数据
    static vector<OpCode> CPU_OPS_CODES; // 所有指令共享的操作码，其实就是打个表，可以根据操作码查出对应的映射，以及相应的一些属性，如果用python实现，就是一个大map
    static unordered_map<byte, OpCode> OPCODES_MAP; // 所有成员共享的东西，这个当然可以放在外面定义，但是和指令是有关系的，所以放在这里
};
