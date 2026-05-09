#include "../public/op_code.h"

#include "../public/cpu.h"

OpCode::OpCode(byte code, string mnemonic, int size, int cycles, AddressingMode mode, Handler handler) : code(code), mnemonic(mnemonic), size(size), cycles(cycles), mode(mode),
                                                                                                         handler(std::move(handler))
{
}

bool OpCode::execute(CPU& cpu)
{
    if (handler)
        return handler(cpu, mode);
    printf("not initialized op:%d \n", code);
    return false;
}

bool OpCode::isHalt() const
{
    return code == 0x00;
}

const char* OpCode::to_string()
{
    return mnemonic.c_str();
}

// 指令集初始化函数
vector<OpCode> OpCode::CPU_OPS_CODES = []()
{
    vector<OpCode> cpu_ops_code = {
        // ADC - Add with Carry
        OpCode(0x69, "ADC", 2, 2, Immediate, &adc),
        OpCode(0x65, "ADC", 2, 3, ZeroPage, &adc),
        OpCode(0x75, "ADC", 2, 4, ZeroPage_X, &adc),
        OpCode(0x6d, "ADC", 3, 4, Absolute, &adc),
        OpCode(0x7d, "ADC", 3, 4, Absolute_X, &adc), // +1 if page crossed
        OpCode(0x79, "ADC", 3, 4, Absolute_Y, &adc), // +1 if page crossed
        OpCode(0x61, "ADC", 2, 6, Indirect_X, &adc),
        OpCode(0x71, "ADC", 2, 5, Indirect_D_Y, &adc), // +1 if page crossed

        // AND - Logical AND
        OpCode(0x29, "AND", 2, 2, Immediate, &op_and),
        OpCode(0x25, "AND", 2, 3, ZeroPage, &op_and),
        OpCode(0x35, "AND", 2, 4, ZeroPage_X, &op_and),
        OpCode(0x2d, "AND", 3, 4, Absolute, &op_and),
        OpCode(0x3d, "AND", 3, 4, Absolute_X, &op_and), // +1 if page crossed
        OpCode(0x39, "AND", 3, 4, Absolute_Y, &op_and), // +1 if page crossed
        OpCode(0x21, "AND", 2, 6, Indirect_X, &op_and),
        OpCode(0x31, "AND", 2, 5, Indirect_D_Y, &op_and), // +1 if page crossed

        // ASL
        OpCode(0x0a, "ASL", 1, 2, Accumulator, &asl),
        OpCode(0x06, "ASL", 2, 5, ZeroPage, &asl),
        OpCode(0x16, "ASL", 2, 6, ZeroPage_X, &asl),
        OpCode(0x0e, "ASL", 3, 6, Absolute, &asl),
        OpCode(0x1e, "ASL", 3, 7, Absolute_X, &asl),

        // BRK - Force Interrupt
        OpCode(0x00, "BRK", 1, 7, Implied, &brk),

        OpCode(0xea, "NOP", 1, 2, NoneAddressing, &nop),


        // EOR
        OpCode(0x49, "EOR", 2, 2, Immediate, &op_eor),
        OpCode(0x45, "EOR", 2, 3, ZeroPage, &op_eor),
        OpCode(0x55, "EOR", 2, 4, ZeroPage_X, &op_eor),
        OpCode(0x4d, "EOR", 3, 4, Absolute, &op_eor),
        OpCode(0x5d, "EOR", 3, 4, Absolute_X, &op_eor), // +1 if page crossed
        OpCode(0x59, "EOR", 3, 4, Absolute_Y, &op_eor), // +1 if page crossed
        OpCode(0x41, "EOR", 2, 6, Indirect_X, &op_eor),
        OpCode(0x51, "EOR", 2, 5, Indirect_D_Y, &op_eor), // +1 if page crossed

        // ORA
        OpCode(0x09, "ORA", 2, 2, Immediate, &op_ora),
        OpCode(0x05, "ORA", 2, 3, ZeroPage, &op_ora),
        OpCode(0x15, "ORA", 2, 4, ZeroPage_X, &op_ora),
        OpCode(0x0d, "ORA", 3, 4, Absolute, &op_ora),
        OpCode(0x1d, "ORA", 3, 4, Absolute_X, &op_ora), // +1 if page crossed
        OpCode(0x19, "ORA", 3, 4, Absolute_Y, &op_ora), // +1 if page crossed
        OpCode(0x01, "ORA", 2, 6, Indirect_X, &op_ora),
        OpCode(0x11, "ORA", 2, 5, Indirect_D_Y, &op_ora), // +1 if page crossed


        // ASR
        OpCode(0x4a, "ASR", 1, 2, NoneAddressing, &asr),
        OpCode(0x46, "ASR", 2, 5, ZeroPage, &asr),
        OpCode(0x56, "ASR", 2, 6, ZeroPage_X, &asr),
        OpCode(0x4e, "ASR", 3, 6, Absolute, &asr),
        OpCode(0x5e, "ASR", 3, 7, Absolute_X, &asr),

        // TAX - Transfer Acuumulator to X
        OpCode(0xaa, "TAX", 1, 2, NoneAddressing, &tax),

        // LDA - Load Accumulator
        OpCode(0xa9, "LDA", 2, 2, Immediate, &lda),
        OpCode(0xa5, "LDA", 2, 3, ZeroPage, &lda),
        OpCode(0xb5, "LDA", 2, 4, ZeroPage_X, &lda),
        OpCode(0xad, "LDA", 3, 4, Absolute, &lda),
        OpCode(0xbd, "LDA", 3, 4, Absolute_X, &lda), // +1 if page crossed
        OpCode(0xb9, "LDA", 3, 4, Absolute_Y, &lda), // +1 if page crossed
        OpCode(0xa1, "LDA", 2, 6, Indirect_X, &lda),
        OpCode(0xb1, "LDA", 2, 5, Indirect_D_Y, &lda), // +1 if page crossed

        // LDX - Load X Register
        OpCode(0xA2, "LDX", 2, 2, Immediate, &ldx),
        OpCode(0xA6, "LDX", 2, 3, ZeroPage, &ldx),
        OpCode(0xB6, "LDX", 2, 4, ZeroPage_Y, &ldx),
        OpCode(0xAE, "LDX", 3, 4, Absolute, &ldx),
        OpCode(0xBE, "LDX", 3, 4, Absolute_Y, &ldx), // +1 if page crossed


        // LDY - Load Y Register
        OpCode(0xA0, "LDY", 2, 2, Immediate, &ldy),
        OpCode(0xA4, "LDY", 2, 3, ZeroPage, &ldy),
        OpCode(0xB4, "LDY", 2, 4, ZeroPage_X, &ldy),
        OpCode(0xAC, "LDY", 3, 4, Absolute, &ldy),
        OpCode(0xBC, "LDY", 3, 4, Absolute_X, &ldy), // +1 if page crossed

        // STA - Store Accumulator
        OpCode(0x85, "STA", 2, 3, ZeroPage, &sta),
        OpCode(0x95, "STA", 2, 4, ZeroPage_X, &sta),
        OpCode(0x8d, "STA", 3, 4, Absolute, &sta),
        OpCode(0x9d, "STA", 3, 5, Absolute_X, &sta),
        OpCode(0x99, "STA", 3, 5, Absolute_Y, &sta),
        OpCode(0x81, "STA", 2, 6, Indirect_X, &sta),
        OpCode(0x91, "STA", 2, 6, Indirect_D_Y, &sta),


        // SBC - Subtract with Carray
        OpCode(0xe9, "SBC", 2, 2, Immediate, &sbc),
        OpCode(0xe5, "SBC", 2, 3, ZeroPage, &sbc),
        OpCode(0xf5, "SBC", 2, 4, ZeroPage_X, &sbc),
        OpCode(0xed, "SBC", 3, 4, Absolute, &sbc),
        OpCode(0xfd, "SBC", 3, 4, Absolute_X, &sbc), // +1 if page crossed
        OpCode(0xf9, "SBC", 3, 4, Absolute_Y, &sbc), // +1 if page crossed
        OpCode(0xe1, "SBC", 2, 6, Indirect_X, &sbc),
        OpCode(0xf1, "SBC", 2, 5, Indirect_D_Y, &sbc), // +1 if page crossed

        // BCC
        OpCode(0x90, "BCC", 2, 2, NoneAddressing, &bcc), // +1 if branch succeeds	+2 if to a new page

        // BCS
        OpCode(0xB0, "BCS", 2, 2, NoneAddressing, &bcs), // +1 if branch succeeds +2 if to a new 

        // BEQ
        OpCode(0x90, "BEQ", 2, 2, NoneAddressing, &beq), // +1 if branch succeeds	+2 if to a new page

        // BNE
        OpCode(0xD0, "BNE", 2, 2, NoneAddressing, &bne), // +1 if branch succeeds	+2 if to a new page

        // BPL - Branch on Plus
        OpCode(0xD0, "BPL", 2, 2, NoneAddressing, &bpl), // +1 if branch succeeds +2 if to a new page

        // BIT - Bit Test
        OpCode(0x24, "BIT", 2, 3, ZeroPage, &bit),
        OpCode(0x2C, "BIT", 3, 4, Absolute, &bit),

        // BVC - Branch if Overflow Clear
        OpCode(0x50, "BVC", 2, 2, Relative, &bvc), // +1 if branch succeeds +2 if to a new page

        // BVS - Branch if Overflow Set
        OpCode(0x70, "BVS", 2, 2, Relative, &bvs), // +1 if branch succeeds +2 if to a new page

        // CLC - Clear Carry Flag
        OpCode(0x18, "CLC", 1, 2, Implied, &clc),

        // CLD - Clear Decimal Mode
        OpCode(0xD8, "CLD", 1, 2, Implied, &cld),

        // CLI - Clear Interrupt Disable
        OpCode(0x58, "CLI", 1, 2, Implied, &cli),

        // CLV - Clear Overflow Flag
        OpCode(0xB8, "CLV", 1, 2, Implied, &clv),

        // CMP - Compare
        OpCode(0xC9, "CMP", 2, 2, Immediate, &cmp),
        OpCode(0xC5, "CMP", 2, 3, ZeroPage, &cmp),
        OpCode(0xD5, "CMP", 2, 4, ZeroPage_X, &cmp),
        OpCode(0xCD, "CMP", 3, 4, Absolute, &cmp),
        OpCode(0xDD, "CMP", 3, 4, Absolute_X, &cmp), // (+1 if page crossed)
        OpCode(0xD9, "CMP", 3, 4, Absolute_Y, &cmp), // (+1 if page crossed)
        OpCode(0xC1, "CMP", 2, 6, Indirect_X, &cmp),
        OpCode(0xD1, "CMP", 2, 5, Indirect_D_Y, &cmp), // (+1 if page crossed)

        // CPX - Compare X Register
        OpCode(0xE0, "CPX", 2, 2, Immediate, &cpx),
        OpCode(0xE4, "CPX", 2, 3, ZeroPage, &cpx),
        OpCode(0xEC, "CPX", 3, 4, Absolute, &cpx),

        // CPY - Compare Y Register
        OpCode(0xC0, "CPY", 2, 2, Immediate, &cpy),
        OpCode(0xC4, "CPY", 2, 3, ZeroPage, &cpy),
        OpCode(0xCC, "CPY", 3, 4, Absolute, &cpy),

        // DEC - Decrement Memory
        OpCode(0xC6, "DEC", 2, 5, ZeroPage, &dec),
        OpCode(0xD6, "DEC", 2, 6, ZeroPage_X, &dec),
        OpCode(0xCE, "DEC", 3, 6, Absolute, &dec),
        OpCode(0xDE, "DEC", 3, 7, Absolute_X, &dec),

        // DEX - Decrement X Register
        OpCode(0xCA, "DEX", 2, 5, Implied, &dex),

        // DEY - Decrement Y Register
        OpCode(0x88, "DEY", 2, 5, Implied, &dey),

        // INC - Increment Memory
        OpCode(0xE6, "INC", 2, 5, ZeroPage, &inc),
        OpCode(0xF6, "INC", 2, 6, ZeroPage_X, &inc),
        OpCode(0xEE, "INC", 3, 6, Absolute, &inc),
        OpCode(0xFE, "INC", 3, 7, Absolute_X, &inc),

        // INX - Increment X Register
        OpCode(0xE8, "INX", 1, 2, Implied, &inx),

        // INY - Increment Y Register
        OpCode(0xC8, "INY", 1, 2, Implied, &iny),

        // JMP - Jump
        OpCode(0xC8, "JMP", 3, 3, Absolute, &jmp),
        OpCode(0xC8, "JMP", 3, 5, Indirect, &jmp),

        // JSR - Jump to Subroutine
        OpCode(0x20, "JSR", 3, 6, Absolute, &jsr),


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

bool OpCode::bit(CPU& cpu, AddressingMode mode)
{
    word addr = cpu.get_operand_address(mode);
    byte value = cpu.mem_read(addr);

    byte result = cpu.register_a & value;

    // 设置Z标志
    cpu.SetFlag(CPU::ZERO_FLAG, result == 0);

    cpu.SetFlag(CPU::NEGATIVE_FLAG, value & CPU::NEGATIVE_FLAG);

    cpu.SetFlag(CPU::OVERFLOW_FLAG, value & CPU::OVERFLOW_FLAG);

    return true;
}

bool OpCode::op_and(CPU& cpu, AddressingMode mode)
{
    word addr = cpu.get_operand_address(mode);
    byte data = cpu.mem_read(addr);
    cpu.register_a = data & cpu.register_a;
    cpu.update_zero_and_negative_flags(cpu.register_a);
    return true;
}

bool OpCode::nop(CPU& cpu, AddressingMode mode)
{
    return true;
}

bool OpCode::brk(CPU& cpu, AddressingMode mode)
{
    word next_pc = cpu.program_counter + 2;

    // 1. 存下调指令地址到栈中
    // 下一条指令地址高字节
    cpu.mem_write(cpu.stack_pointer + 0x0100, (next_pc >> 8) & BYTE_MAX);
    cpu.stack_pointer--;

    // 下一条指令地址低字节
    cpu.mem_write(cpu.stack_pointer + 0x0100, next_pc & BYTE_MAX);
    cpu.stack_pointer--;

    // 2. 设置中断标志
    cpu.SetFlag(CPU::BREAK_FLAG, true);

    // 3. CPU状态压栈
    cpu.mem_write(cpu.stack_pointer + 0x0100, cpu.status);
    cpu.stack_pointer--;

    // 4. 设置中断禁用标志I（表示正在中断中）
    cpu.SetFlag(CPU::INTERRUPT_FLAG, true);

    // 5. 读取中断向量表（$FFFE低字节，$FFFF高字节）并跳转
    word int_vector = cpu.mem_read(0xFFFF) << 8 | cpu.mem_read(0xFFFE);
    cpu.program_counter = int_vector;

    return true;
}

bool OpCode::lda(CPU& cpu, AddressingMode mode)
{
    cpu.register_a = cpu.mem_read(cpu.get_operand_address(mode));
    cpu.update_zero_and_negative_flags(cpu.register_a);
    return true;
}

bool OpCode::sta(CPU& cpu, AddressingMode mode)
{
    word addr = cpu.get_operand_address(mode);
    cpu.mem_write(addr, cpu.register_a);
    return true;
}

bool OpCode::tax(CPU& cpu, AddressingMode mode)
{
    cpu.register_x = cpu.register_a;
    cpu.update_zero_and_negative_flags(cpu.register_x);

    return true;
}

bool OpCode::inx(CPU& cpu, AddressingMode mode)
{
    cpu.register_x += 1;
    cpu.update_zero_and_negative_flags(cpu.register_x);

    return true;
}

bool OpCode::iny(CPU& cpu, AddressingMode mode)
{
    cpu.register_y += 1;
    cpu.update_zero_and_negative_flags(cpu.register_y);

    return true;
}

bool OpCode::jmp(CPU& cpu, AddressingMode mode)
{
    word addr = cpu.get_operand_address(mode);
    cpu.program_counter = addr;

    return true;
}

bool OpCode::jsr(CPU& cpu, AddressingMode mode)
{
    word addr = cpu.get_operand_address(mode);
    word return_addr = cpu.program_counter + 2;

    cpu.mem_write(cpu.stack_pointer + 0x0100, (return_addr >> 8) & 0xFF);
    cpu.stack_pointer--;
    cpu.mem_write(0x0100 + cpu.stack_pointer, return_addr & 0xFF);
    cpu.stack_pointer--;
    cpu.program_counter = addr;

    return true;
}

bool OpCode::ldx(CPU& cpu, AddressingMode mode)
{
    cpu.register_x = cpu.mem_read(cpu.get_operand_address(mode));
    cpu.update_zero_and_negative_flags(cpu.register_x);
    return true;
    return true;
}

bool OpCode::ldy(CPU& cpu, AddressingMode mode)
{
    cpu.register_y = cpu.mem_read(cpu.get_operand_address(mode));
    cpu.update_zero_and_negative_flags(cpu.register_y);
    return true;
    return true;
}


bool OpCode::adc(CPU& cpu, AddressingMode mode)
{
    // adc(y) 定义为x+y+c
    word address = cpu.get_operand_address(mode);
    byte value = cpu.mem_read(address);

    cpu.add_to_register_a(value);

    return true;
}

bool OpCode::sbc(CPU& cpu, AddressingMode mode)
{
    word address = cpu.get_operand_address(mode);
    byte value = cpu.mem_read(address);
    // sbc(y) 定义为x-y-(1-c)
    // 推导 x-y-(1-c) = x-y-1+c = x+(~y+1)-1+c = x+~y+c=add(~y)
    // -y-1的操作需要注意，对于-128 这个特殊情况
    // 如果保存在unsigned char中，有一个转换，转换为128，则机器数为1000_0000,取了负数的操作，仍然是1000_0000(取负操作是各位取反，末位+1，则变为128
    // 如果保存在signed char中，机器数仍然是1000_0000，取了负数的操作，仍然是1000_0000(取负操作是各位取反，末位+1），解释仍然为-128
    // 取负操作的时候，需要注意，计算机都是各位取反，末位+1
    // 所以其实在函数传递参数的时候，传-y-1传的是一个字面量，取负操作，需要把y当作有符号数据对待？如果不进行转换呢？
    cpu.add_to_register_a(~value);
    return true;
}


bool OpCode::op_eor(CPU& cpu, AddressingMode mode)
{
    word addr = cpu.get_operand_address(mode);
    byte data = cpu.mem_read(addr);
    cpu.register_a = data ^ cpu.register_a;
    cpu.update_zero_and_negative_flags(cpu.register_a);
    return true;
}

bool OpCode::op_ora(CPU& cpu, AddressingMode mode)
{
    word addr = cpu.get_operand_address(mode);
    byte data = cpu.mem_read(addr);
    cpu.register_a = data | cpu.register_a;
    cpu.update_zero_and_negative_flags(cpu.register_a);
    return true;
}

bool OpCode::asl(CPU& cpu, AddressingMode mode)
{
    if (mode == Accumulator)
    {
        byte data = cpu.register_a;
        bool bOldCarry = data >> 7;
        byte result = static_cast<byte>((data << 1) & 0xff);
        cpu.SetFlag(CPU::CARRY_FLAG, bOldCarry);
        cpu.update_zero_and_negative_flags(result);
        cpu.register_a = result;
    }
    else
    {
        word addr = cpu.get_operand_address(mode);
        byte data = cpu.mem_read(addr);
        bool bOldCarry = data >> 7;
        byte result = static_cast<byte>((data << 1) & 0xff);
        cpu.SetFlag(CPU::CARRY_FLAG, bOldCarry);
        cpu.update_zero_and_negative_flags(result);
        cpu.mem_write(addr, result);
    }
    return true;
}

bool OpCode::asr(CPU& cpu, AddressingMode mode)
{
    word addr = cpu.get_operand_address(mode);
    byte data = cpu.mem_read(addr);

    int carry = data | 0x80;

    if (data >> 7 == 1)
    {
        cpu.status = cpu.status | 0x01;
    }
    else
    {
        cpu.status = cpu.status & 0xfe;
    }

    data = data << 1;
    cpu.mem_write(addr, data);
    cpu.update_zero_and_negative_flags(data);
    return true;
}

bool OpCode::bcc(CPU& cpu, AddressingMode mode)
{
    int8_t jump = cpu.mem_read(cpu.program_counter++); // 注意分支指令的这个操作
    // if carry clear
    if ((cpu.status & CPU::CARRY_FLAG) == 0)
    {
        word jump_addr = cpu.program_counter + jump; // +1是因为要从下个指令的地址开始，加上这个跳转的数值
        cpu.program_counter = jump_addr;
    }
    return true;
}

bool OpCode::bcs(CPU& cpu, AddressingMode mode)
{
    int8_t jump = cpu.mem_read(cpu.program_counter++);
    // if carry clear
    if (cpu.status & CPU::CARRY_FLAG)
    {
        word jump_addr = cpu.program_counter + jump; // +1是因为要从下个指令的地址开始，加上这个跳转的数值
        cpu.program_counter = jump_addr;
    }
    return true;
}

bool OpCode::beq(CPU& cpu, AddressingMode mode)
{
    int8_t jump = cpu.mem_read(cpu.program_counter++);
    // if zero clear
    if (cpu.status & CPU::ZERO_FLAG)
    {
        word jump_addr = cpu.program_counter + jump; // +1是因为要从下个指令的地址开始，加上这个跳转的数值
        cpu.program_counter = jump_addr;
    }
    return true;
}

bool OpCode::bne(CPU& cpu, AddressingMode mode)
{
    int8_t jump = cpu.mem_read(cpu.program_counter++);
    // if zero set
    if ((cpu.status & CPU::ZERO_FLAG) == 0)
    {
        word jump_addr = cpu.program_counter + jump; // +1是因为要从下个指令的地址开始，加上这个跳转的数值
        cpu.program_counter = jump_addr;
    }
    return true;
}

bool OpCode::bpl(CPU& cpu, AddressingMode mode)
{
    int8_t jump = cpu.mem_read(cpu.program_counter++);
    if ((cpu.status & CPU::NEGATIVE_FLAG) == 0)
    {
        word jump_addr = cpu.program_counter + jump;
        cpu.program_counter = jump_addr;
    }
    return true;
}

bool OpCode::bmi(CPU& cpu, AddressingMode mode)
{
    int8_t jump = cpu.mem_read(cpu.program_counter++);
    if (cpu.status & CPU::NEGATIVE_FLAG)
    {
        word jump_addr = cpu.program_counter + jump;
        cpu.program_counter = jump_addr;
    }
    return true;
}

bool OpCode::bvc(CPU& cpu, AddressingMode mode)
{
    int8_t jump = cpu.mem_read(cpu.program_counter++);
    if ((cpu.status & CPU::OVERFLOW_FLAG) == 0)
    {
        word jump_addr = cpu.program_counter + jump;
        cpu.program_counter = jump_addr;
    }
    return true;
}

bool OpCode::bvs(CPU& cpu, AddressingMode mode)
{
    int8_t jump = cpu.mem_read(cpu.program_counter++);
    if (cpu.status & CPU::OVERFLOW_FLAG)
    {
        word jump_addr = cpu.program_counter + jump;
        cpu.program_counter = jump_addr;
    }
    return true;
}

bool OpCode::clc(CPU& cpu, AddressingMode mode)
{
    cpu.SetFlag(CPU::CARRY_FLAG, false);
    return true;
}

bool OpCode::cld(CPU& cpu, AddressingMode mode)
{
    cpu.SetFlag(CPU::DECIMAL_FLAG, false);
    return true;
}

bool OpCode::cli(CPU& cpu, AddressingMode mode)
{
    cpu.SetFlag(CPU::INTERRUPT_FLAG, false);
    return true;
}

bool OpCode::clv(CPU& cpu, AddressingMode mode)
{
    cpu.SetFlag(CPU::OVERFLOW_FLAG, false);
    return true;
}

bool OpCode::cmp_common(CPU& cpu, AddressingMode mode, byte register_data)
{
    word addr = cpu.get_operand_address(mode);
    byte data = cpu.mem_read(addr);

    int result = register_data - data;

    cpu.SetFlag(CPU::CARRY_FLAG, register_data >= data);
    cpu.SetFlag(CPU::ZERO_FLAG, result == 0);
    cpu.SetFlag(CPU::NEGATIVE_FLAG, result & CPU::NEGATIVE_FLAG);

    return true;
}

bool OpCode::cmp(CPU& cpu, AddressingMode mode)
{
    return cmp_common(cpu, mode, cpu.register_a);
}

bool OpCode::cpx(CPU& cpu, AddressingMode mode)
{
    return cmp_common(cpu, mode, cpu.register_x);
}

bool OpCode::cpy(CPU& cpu, AddressingMode mode)
{
    return cmp_common(cpu, mode, cpu.register_y);
}

bool OpCode::dec(CPU& cpu, AddressingMode mode)
{
    word addr = cpu.get_operand_address(mode);
    byte data = cpu.mem_read(addr);
    byte result = data - 1;
    cpu.mem_write(addr, result);
    cpu.SetFlag(CPU::ZERO_FLAG, result == 0);
    cpu.SetFlag(CPU::NEGATIVE_FLAG, result & CPU::NEGATIVE_FLAG);

    return true;
}

bool OpCode::dex(CPU& cpu, AddressingMode mode)
{
    cpu.register_x--;
    cpu.SetFlag(CPU::ZERO_FLAG, cpu.register_x == 0);
    cpu.SetFlag(CPU::NEGATIVE_FLAG, cpu.register_x & CPU::NEGATIVE_FLAG);
    return true;
}

bool OpCode::dey(CPU& cpu, AddressingMode mode)
{
    cpu.register_y--;
    cpu.SetFlag(CPU::ZERO_FLAG, cpu.register_y == 0);
    cpu.SetFlag(CPU::NEGATIVE_FLAG, cpu.register_y & CPU::NEGATIVE_FLAG);
    return true;
}

bool OpCode::inc(CPU& cpu, AddressingMode mode)
{
    word addr = cpu.get_operand_address(mode);
    byte data = cpu.mem_read(addr);
    byte result = data + 1;
    cpu.mem_write(addr, result);
    cpu.SetFlag(CPU::ZERO_FLAG, result == 0);
    cpu.SetFlag(CPU::NEGATIVE_FLAG, result & CPU::NEGATIVE_FLAG);

    return true;
}
