#include "../public/op_code.h"

OpCode::OpCode(byte code, string mnemonic, int size, int cycles, AddressingMode mode) : code(code), mnemonic(mnemonic), size(size), cycles(cycles), mode(mode)
{
}

// 指令集初始化函数
vector<OpCode> OpCode::CPU_OPS_CODES = []()
{
    vector<OpCode> cpu_ops_code = {
        OpCode(0x00, "BRK", 1, 7, NoneAddressing),
        OpCode(0xea, "NOP", 1, 2, NoneAddressing),

        // AND
        OpCode(0x29, "AND", 2, 2, Immediate),
        OpCode(0x25, "AND", 2, 3, ZeroPage),
        OpCode(0x35, "AND", 2, 4, ZeroPage_X),
        OpCode(0x2d, "AND", 3, 4, Absolute),
        OpCode(0x3d, "AND", 3, 4, Absolute_X), // +1 if page crossed
        OpCode(0x39, "AND", 3, 4, Absolute_Y), // +1 if page crossed
        OpCode(0x21, "AND", 2, 6, Indirect_X),
        OpCode(0x31, "AND", 2, 5, Indirect_Y), // +1 if page crossed

        // EOR
        OpCode(0x49, "EOR", 2, 2, Immediate),
        OpCode(0x45, "EOR", 2, 3, ZeroPage),
        OpCode(0x55, "EOR", 2, 4, ZeroPage_X),
        OpCode(0x4d, "EOR", 3, 4, Absolute),
        OpCode(0x5d, "EOR", 3, 4, Absolute_X), // +1 if page crossed
        OpCode(0x59, "EOR", 3, 4, Absolute_Y), // +1 if page crossed
        OpCode(0x41, "EOR", 2, 6, Indirect_X),
        OpCode(0x51, "EOR", 2, 5, Indirect_Y), // +1 if page crossed

        // ORA
        OpCode(0x09, "ORA", 2, 2, Immediate),
        OpCode(0x05, "ORA", 2, 3, ZeroPage),
        OpCode(0x15, "ORA", 2, 4, ZeroPage_X),
        OpCode(0x0d, "ORA", 3, 4, Absolute),
        OpCode(0x1d, "ORA", 3, 4, Absolute_X), // +1 if page crossed
        OpCode(0x19, "ORA", 3, 4, Absolute_Y), // +1 if page crossed
        OpCode(0x01, "ORA", 2, 6, Indirect_X),
        OpCode(0x11, "ORA", 2, 5, Indirect_Y), // +1 if page crossed

        // ASL
        OpCode(0x0a, "ASL", 1, 2, NoneAddressing),
        OpCode(0x06, "ASL", 2, 5, ZeroPage),
        OpCode(0x16, "ASL", 2, 6, ZeroPage_X),
        OpCode(0x0e, "ASL", 3, 6, Absolute),
        OpCode(0x1e, "ASL", 3, 7, Absolute_X),

        // ASR
        OpCode(0x4a, "ASR", 1, 2, NoneAddressing),
        OpCode(0x46, "ASR", 2, 5, ZeroPage),
        OpCode(0x56, "ASR", 2, 6, ZeroPage_X),
        OpCode(0x4e, "ASR", 3, 6, Absolute),
        OpCode(0x5e, "ASR", 3, 7, Absolute_X),

        // TAX
        OpCode(0xaa, "TAX", 1, 2, NoneAddressing),

        // INX
        OpCode(0xe8, "INX", 1, 2, NoneAddressing),

        // LDA
        OpCode(0xa9, "LDA", 2, 2, Immediate),
        OpCode(0xa5, "LDA", 2, 3, ZeroPage),
        OpCode(0xb5, "LDA", 2, 4, ZeroPage_X),
        OpCode(0xad, "LDA", 3, 4, Absolute),
        OpCode(0xbd, "LDA", 3, 4, Absolute_X), // +1 if page crossed
        OpCode(0xb9, "LDA", 3, 4, Absolute_Y), // +1 if page crossed
        OpCode(0xa1, "LDA", 2, 6, Indirect_X),
        OpCode(0xb1, "LDA", 2, 5, Indirect_Y), // +1 if page crossed

        // STA
        OpCode(0x85, "STA", 2, 3, ZeroPage),
        OpCode(0x95, "STA", 2, 4, ZeroPage_X),
        OpCode(0x8d, "STA", 3, 4, Absolute),
        OpCode(0x9d, "STA", 3, 5, Absolute_X),
        OpCode(0x99, "STA", 3, 5, Absolute_Y),
        OpCode(0x81, "STA", 2, 6, Indirect_X),
        OpCode(0x91, "STA", 2, 6, Indirect_Y),

        // ADC, add with carray
        OpCode(0x69, "ADC", 2, 2, Immediate),
        OpCode(0x65, "ADC", 2, 3, ZeroPage),
        OpCode(0x75, "ADC", 2, 4, ZeroPage_X),
        OpCode(0x6d, "ADC", 3, 4, Absolute),
        OpCode(0x7d, "ADC", 3, 4, Absolute_X), // +1 if page crossed
        OpCode(0x79, "ADC", 3, 4, Absolute_Y), // +1 if page crossed
        OpCode(0x61, "ADC", 2, 6, Indirect_X),
        OpCode(0x71, "ADC", 2, 5, Indirect_Y), // +1 if page crossed

        // SBC, add with not carry
        OpCode(0xe9, "SBC", 2, 2, Immediate),
        OpCode(0xe5, "SBC", 2, 3, ZeroPage),
        OpCode(0xf5, "SBC", 2, 4, ZeroPage_X),
        OpCode(0xed, "SBC", 3, 4, Absolute),
        OpCode(0xfd, "SBC", 3, 4, Absolute_X), // +1 if page crossed
        OpCode(0xf9, "SBC", 3, 4, Absolute_Y), // +1 if page crossed
        OpCode(0xe1, "SBC", 2, 6, Indirect_X),
        OpCode(0xf1, "SBC", 2, 5, Indirect_Y), // +1 if page crossed

        // BCC
        OpCode(0x90, "BCC", 2, 2, NoneAddressing), // +1 if branch succeeds	+2 if to a new page

        // BCS
        OpCode(0xB0, "BCS", 2, 2, NoneAddressing), // +1 if branch succeeds +2 if to a new 

        // BEQ
        OpCode(0x90, "BEQ", 2, 2, NoneAddressing), // +1 if branch succeeds	+2 if to a new page

        // BEQ
        OpCode(0xD0, "BNE", 2, 2, NoneAddressing), // +1 if branch succeeds	+2 if to a new page

        // BPL, Branch On Plus
        OpCode(0xD0, "BPL", 2, 2, NoneAddressing), // +1 if branch succeeds +2 if to a new page
    };
    return cpu_ops_code;
}();

// 指令映射函数初始化
unordered_map<byte, OpCode> OpCode::OPCODES_MAP = []()
{
    unordered_map<byte, OpCode> op_code_map;
    for (auto cpu_ops_code : CPU_OPS_CODES)
    {
        op_code_map[cpu_ops_code.code] = cpu_ops_code;
    }
    return op_code_map;
}();
