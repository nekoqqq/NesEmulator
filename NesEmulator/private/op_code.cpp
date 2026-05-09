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

        OpCode(0x00, "BRK", 1, 7, NoneAddressing, &brk),
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

        // INX - Increment X Register
        OpCode(0xe8, "INX", 1, 2, NoneAddressing, &inx),

        // INY - Increment Y Register
        OpCode(0xc8, "INY", 1, 2, NoneAddressing, &iny),

        // LDA - Load Accumulator
        OpCode(0xa9, "LDA", 2, 2, Immediate, &lda),
        OpCode(0xa5, "LDA", 2, 3, ZeroPage, &lda),
        OpCode(0xb5, "LDA", 2, 4, ZeroPage_X, &lda),
        OpCode(0xad, "LDA", 3, 4, Absolute, &lda),
        OpCode(0xbd, "LDA", 3, 4, Absolute_X, &lda), // +1 if page crossed
        OpCode(0xb9, "LDA", 3, 4, Absolute_Y, &lda), // +1 if page crossed
        OpCode(0xa1, "LDA", 2, 6, Indirect_X, &lda),
        OpCode(0xb1, "LDA", 2, 5, Indirect_D_Y, &lda), // +1 if page crossed

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
        OpCode(0x24, "BIT", 2, 3, ZeroPage, &bit), // +1 if branch succeeds +2 if to a new page
        OpCode(0x2C, "BIT", 3, 4, Absolute, &bit), // +1 if branch succeeds +2 if to a new page

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
    return true; // halt指令退出
}

bool OpCode::lda(CPU& cpu, AddressingMode mode)
{
    if (mode == Immediate)
    {
        cpu.register_a = cpu.mem_read(cpu.program_counter++);
        cpu.update_zero_and_negative_flags(cpu.register_a);
    }
    else
    {
        word addr = cpu.get_operand_address(mode);
        byte value = cpu.mem_read(addr);

        cpu.register_a = value;
        cpu.update_zero_and_negative_flags(cpu.register_a);
    }

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
    if ((cpu.status & CPU::CARRY_FLAG) == 1)
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
    if ((cpu.status &CPU::NEGATIVE_FLAG) == 0)
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
    if ((cpu.status & 1 << 6) == 0)
    {
        word jump_addr = cpu.program_counter + jump;
        cpu.program_counter = jump_addr;
    }
    return true;
}

bool OpCode::bvs(CPU& cpu, AddressingMode mode)
{
    int8_t jump = cpu.mem_read(cpu.program_counter++);
    if ((cpu.status & 1 << 6) == 1)
    {
        word jump_addr = cpu.program_counter + jump;
        cpu.program_counter = jump_addr;
    }
    return true;
}
