/*
 *指令码
 */
#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "platform_common.h"
class CPU;
using std::string;
using std::vector;
using std::unordered_map;
using Handler = std::function<bool(CPU& cpu, AddressingMode mode)>;

// 每一种类型的操作指令
struct OpCode
{
    OpCode(byte code = 0, string mnemonic = "", int size = 0, int cycles = 0, AddressingMode mode = NoneAddressing, Handler handler = nullptr);

    byte code;
    string mnemonic; // todo 如何只做一份string
    int size;
    int cycles;
    AddressingMode mode;
    Handler handler;

    bool execute(CPU& cpu);
    bool isHalt() const;
    const char* to_string();

    // 所有指令都共享的数据
    static vector<OpCode> CPU_OPS_CODES; // 所有指令共享的操作码，其实就是打个表，可以根据操作码查出对应的映射，以及相应的一些属性，如果用python实现，就是一个大map
    static unordered_map<byte, OpCode> OPCODES_MAP; // 所有成员共享的东西，这个当然可以放在外面定义，但是和指令是有关系的，所以放在这里

    static bool op_and(CPU& cpu, AddressingMode mode);
    static bool nop(CPU& cpu, AddressingMode mode);
    static bool brk(CPU& cpu, AddressingMode mode);
    static bool lda(CPU& cpu, AddressingMode mode);
    static bool sta(CPU& cpu, AddressingMode mode);
    static bool tax(CPU& cpu, AddressingMode mode);
    static bool inx(CPU& cpu, AddressingMode mode);
    static bool iny(CPU& cpu, AddressingMode mode);
    static bool adc(CPU& cpu, AddressingMode mode);
    static bool sbc(CPU& cpu, AddressingMode mode);
    static bool op_eor(CPU& cpu, AddressingMode mode);
    static bool op_ora(CPU& cpu, AddressingMode mode);
    static bool asl(CPU& cpu, AddressingMode mode);
    static bool asr(CPU& cpu, AddressingMode mode);
    static bool bcc(CPU& cpu, AddressingMode mode);
    static bool bcs(CPU& cpu, AddressingMode mode);

    // Branch On Equal
    static bool beq(CPU& cpu, AddressingMode mode);

    // Branch On Not Equal
    static bool bne(CPU& cpu, AddressingMode mode);
    static bool bpl(CPU& cpu, AddressingMode mode);
    static bool bmi(CPU& cpu, AddressingMode mode);
    static bool bvc(CPU& cpu, AddressingMode mode);
    static bool bvs(CPU& cpu, AddressingMode mode);
};
