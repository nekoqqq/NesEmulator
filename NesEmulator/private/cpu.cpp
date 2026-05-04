#include "../public/cpu.h"
#include "../public/op_code.h"


CPU::CPU(byte program_counter, byte stack_pointer, byte register_a, byte register_x, byte register_y, byte status) : program_counter(program_counter),
                                                                                                                     stack_pointer(stack_pointer), register_a(register_a),
                                                                                                                     register_x(register_x), register_y(register_y), status(status)
{
}

void CPU::interpret(vector<byte>& program)
{
    while (true)
    {
        // 取指
        byte opscode = program[program_counter++];

        switch (opscode)
        {
        // LDA Immediate
        case 0xA9: lda(program, Immediate);
            break;

        // TAX Immediate
        case 0xAA: tax(program, NoneAddressing);
            break;

        case 0xE8: inx(program, NoneAddressing);
            break;


        case 0xC8:
            iny(program, NoneAddressing);
            break;

        // BRK 
        case 0x00:
            {
                brk(program, NoneAddressing);
                return;
                break;
            }

        default:
            break;
        }
    }
}

byte CPU::mem_read(uint16_t addr) const
{
    return memory[addr];
}

void CPU::mem_write(uint16_t addr, byte data)
{
    memory[addr] = data;
}

void CPU::load(vector<byte>& program)
{
    // 从32 KB处开始加载程序,程序计数器也设置为此,ROM加载到此内存里面，并不代表实际就从0x8000开始执行了
    memcpy(memory + 0x8000, &program[0], program.size() * sizeof(byte));

    // 在NES插入卡带后，会重置程序状态，并将程序计数器的位置设置为存储在0xFFFC处的值,[X,X,X,X] 也就是最后4个字节
    // 将0xFFFC处存储的值设置为0x8000;
    mem_write_u16(0xFFFC, 0x8000);
}

void CPU::load_and_run(vector<byte>& program)
{
    load(program);
    reset();
    run();
}

uint16_t CPU::mem_read_u16(uint16_t pos) const
{
    uint16_t lo = mem_read(pos);
    uint16_t hi = mem_read(pos + 1);
    return hi << 8 | lo;
}

void CPU::mem_write_u16(uint16_t pos, uint16_t data)
{
    byte hi = (data >> 8); // 高8位
    byte lo = (data & 0xff); // 低8位
    mem_write(pos, lo);
    mem_write(pos + 1, hi);
}

void CPU::reset()
{
    register_a = 0;
    register_x = 0;
    status = 0;

    program_counter = mem_read_u16(0xFFFC);
}

void CPU::run()
{
    while (true)
    {
        // 取指
        byte code = mem_read(program_counter++);
        uint16_t program_counter_state = program_counter;

        OpCode op_code = OpCode::OPCODES_MAP.at(code);


        program prog;
        switch (code)
        {
        // LDA
        case 0xa9:
        case 0xa5:
        case 0xb5:
        case 0xad:
        case 0xbd:
        case 0xb9:
        case 0xa1:
        case 0xb1: lda(prog, op_code.mode);
            break;

        // STA
        case 0x85:
        case 0x95:
        case 0x8d:
        case 0x9d:
        case 0x99:
        case 0x81:
        case 0x91: sta(prog, op_code.mode);
            break;


        // ADC
        case 0x69:
        case 0x65:
        case 0x75:
        case 0x6d:
        case 0x7d:
        case 0x79:
        case 0x61:
        case 0x71: adc(prog, op_code.mode);
            break;

        // SBC
        case 0xe9:
        case 0xe5:
        case 0xf5:
        case 0xed:
        case 0xfd:
        case 0xf9:
        case 0xe1:
        case 0xf1: sbc(prog, op_code.mode);
            break;

        // AND
        case 0x29:
        case 0x25:
        case 0x35:
        case 0x2d:
        case 0x3d:
        case 0x39:
        case 0x21:
        case 0x31: op_and(prog, op_code.mode);
            break;

        // EOR
        case 0x49:
        case 0x45:
        case 0x55:
        case 0x4d:
        case 0x5d:
        case 0x59:
        case 0x41:
        case 0x51: op_eor(prog, op_code.mode);
            break;

        // ORA
        case 0x09:
        case 0x05:
        case 0x15:
        case 0x0d:
        case 0x1d:
        case 0x19:
        case 0x01:
        case 0x11: op_ora(prog, op_code.mode);
            break;

        // ASL
        case 0x0A:
        case 0x06:
        case 0x16:
        case 0x0e:
        case 0x1e: asl(prog, op_code.mode);
            break;


        // Branch Family
        // BPL
        case 0x10: bpl(prog, op_code.mode);
            break;
        // BMI
        case 0x30: bmi(prog, op_code.mode);
            break;
        // BVC
        case 0x50: bvc(prog, op_code.mode);
            break;
        // BVS
        case 0x70: bvc(prog, op_code.mode);
            break;
        // BCC
        case 0x90: bcc(prog, op_code.mode);
            break;
        // BCS
        case 0xB0: bcs(prog, op_code.mode);
            break;
        // BNE
        case 0xD0: bne(prog, op_code.mode);
            break;
        // BEQ
        case 0xF0: beq(prog, op_code.mode);
            break;


        // TAX
        case 0xaa: tax(prog, op_code.mode);
            break;

        // INX
        case 0xe8: inx(prog, op_code.mode);
            break;

        // INY
        case 0xC8: iny(prog, op_code.mode);
            break;

        // BRK 
        case 0x00:
            {
                brk(prog, op_code.mode);
                return;
                break;
            }


        default:
            break;
        }
        if (program_counter_state == program_counter)
        {
            program_counter += op_code.size - 1;
        }
    }
}

bool CPU::brk(program& prog, AddressingMode mode)
{
    std::printf("halt\n");
    return true;
}

bool CPU::lda(program& prog, AddressingMode mode)
{
    if (mode == Immediate)
    {
        register_a = prog[program_counter++];
        update_zero_and_negative_flags(register_a);

        return true;
    }
    else
    {
        uint16_t addr = get_operand_address(mode);
        byte value = mem_read(addr);

        register_a = value;
        update_zero_and_negative_flags(register_a);
        return true;
    }
}

bool CPU::sta(program& prog, AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    mem_write(addr, register_a);
    return true;
}

bool CPU::tax(program& prog, AddressingMode mode)
{
    register_x = register_a;
    update_zero_and_negative_flags(register_x);

    return true;
}

bool CPU::inx(program& prog, AddressingMode mode)
{
    register_x += 1;
    update_zero_and_negative_flags(register_x);

    return true;
}

bool CPU::iny(program& prog, AddressingMode mode)
{
    register_y += 1;
    update_zero_and_negative_flags(register_y);

    return true;
}

bool CPU::adc(program& prog, AddressingMode mode)
{
    // adc(y) 定义为x+y+c
    uint16_t address = get_operand_address(mode);
    byte value = mem_read(address);

    add_to_register_a(value);

    return true;
}

bool CPU::sbc(program& prog, AddressingMode mode)
{
    uint16_t address = get_operand_address(mode);
    byte value = mem_read(address);
    // sbc(y) 定义为x-y-(1-c)
    // 推导 x-y-(1-c) = x-y-1+c = x+(~y+1)-1+c = x+~y+c=add(~y)
    // -y-1的操作需要注意，对于-128 这个特殊情况
    // 如果保存在unsigned char中，有一个转换，转换为128，则机器数为1000_0000,取了负数的操作，仍然是1000_0000(取负操作是各位取反，末位+1，则变为128
    // 如果保存在signed char中，机器数仍然是1000_0000，取了负数的操作，仍然是1000_0000(取负操作是各位取反，末位+1），解释仍然为-128
    // 取负操作的时候，需要注意，计算机都是各位取反，末位+1
    // 所以其实在函数传递参数的时候，传-y-1传的是一个字面量，取负操作，需要把y当作有符号数据对待？如果不进行转换呢？
    add_to_register_a(~value);
    return true;
}

bool CPU::op_and(program& prog, AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    byte data = mem_read(addr);
    register_a = data & register_a;
    update_zero_and_negative_flags(register_a);
    return true;
}

bool CPU::op_eor(program& prog, AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    byte data = mem_read(addr);
    register_a = data ^ register_a;
    update_zero_and_negative_flags(register_a);
    return true;
}

bool CPU::op_ora(program& prog, AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    byte data = mem_read(addr);
    register_a = data | register_a;
    update_zero_and_negative_flags(register_a);
    return true;
}

bool CPU::asl(program& prog, AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    byte data = mem_read(addr);

    int carry = data | 0x80;

    if (data >> 7 == 1)
    {
        status = status | 0x01;
    }
    else
    {
        status = status & 0xfe;
    }

    data = data << 1;
    mem_write(addr, data);
    update_zero_and_negative_flags(data);
    return true;
}

bool CPU::asr(program& prog, AddressingMode mode)
{
    uint16_t addr = get_operand_address(mode);
    byte data = mem_read(addr);

    int carry = data | 0x80;

    if (data >> 7 == 1)
    {
        status = status | 0x01;
    }
    else
    {
        status = status & 0xfe;
    }

    data = data << 1;
    mem_write(addr, data);
    update_zero_and_negative_flags(data);
    return true;
}

bool CPU::bcc(program& prog, AddressingMode mode)
{
    // if carry clear
    if ((status & 1 << 0) == 0)
    {
        int8_t jump = mem_read(program_counter);
        uint16_t jump_addr = program_counter + 1 + jump; // +1是因为要从下个指令的地址开始，加上这个跳转的数值
        program_counter = jump_addr;
    }
    return true;
}

bool CPU::bcs(program& prog, AddressingMode mode)
{
    // if carry clear
    if ((status & 1 << 0) == 1)
    {
        int8_t jump = mem_read(program_counter);
        uint16_t jump_addr = program_counter + 1 + jump; // +1是因为要从下个指令的地址开始，加上这个跳转的数值
        program_counter = jump_addr;
    }
    return true;
}

bool CPU::beq(program& prog, AddressingMode mode)
{
    // if zero clear
    if ((status & 1 << 1) == 0)
    {
        int8_t jump = mem_read(program_counter);
        uint16_t jump_addr = program_counter + 1 + jump; // +1是因为要从下个指令的地址开始，加上这个跳转的数值
        program_counter = jump_addr;
    }
    return true;
}

bool CPU::bne(program& prog, AddressingMode mode)
{
    // if zero set
    if ((status & 1 << 1) == 1)
    {
        int8_t jump = mem_read(program_counter);
        uint16_t jump_addr = program_counter + 1 + jump; // +1是因为要从下个指令的地址开始，加上这个跳转的数值
        program_counter = jump_addr;
    }
    return true;
}

bool CPU::bpl(program& prog, AddressingMode mode)
{
    if ((status & 1 << 7) == 0)
    {
        int8_t jump = mem_read(program_counter);
        uint16_t jump_addr = program_counter + 1 + jump;
        program_counter = jump_addr;
    }
    return true;
}

bool CPU::bmi(program& prog, AddressingMode mode)
{
    if ((status & 1 << 7) == 1)
    {
        int8_t jump = mem_read(program_counter);
        uint16_t jump_addr = program_counter + 1 + jump;
        program_counter = jump_addr;
    }
    return true;
}

bool CPU::bvc(program& prog, AddressingMode mode)
{
    if ((status & 1 << 6) == 0)
    {
        int8_t jump = mem_read(program_counter);
        uint16_t jump_addr = program_counter + 1 + jump;
        program_counter = jump_addr;
    }
    return true;
}

bool CPU::bvs(program& prog, AddressingMode mode)
{
    if ((status & 1 << 6) == 1)
    {
        int8_t jump = mem_read(program_counter);
        uint16_t jump_addr = program_counter + 1 + jump;
        program_counter = jump_addr;
    }
    return true;
}

void CPU::update_zero_and_negative_flags(byte res)
{
    if (res == 0)
    {
        status = status | 0x02; // 0000_0010
    }
    else
    {
        status = status & 0xfd; // 1111_1101
    }

    // set flag N,置成1就用|，置成0就用&
    if (res & 0x80)
    {
        // 10000000
        status = status | 0x80; // 1000_0000 
    }
    else
    {
        status = status & 0x7f; // 0111_1111
    }
}

uint16_t CPU::get_operand_address(AddressingMode& mode) const
{
    uint16_t address;
    switch (mode)
    {
    // 立即数，当前内存地址为PC指向
    case AddressingMode::Immediate:
        address = program_counter;
        break;

    // 只有一个byte，可以索引0-255的地址
    case AddressingMode::ZeroPage:
        address = mem_read(program_counter);
        break;

    // 两个字节，索引0-65535的整个地址空间
    case AddressingMode::Absolute:
        address = mem_read_u16(program_counter);
        break;

    // 用X里面存储的值+一个byte的数据做索引
    case AddressingMode::ZeroPage_X:
        {
            byte pos = mem_read(program_counter);
            address = pos + register_x;
        }
        break;

    // 用Y里面存储的值+一个byte的数据做索引
    case AddressingMode::ZeroPage_Y:
        {
            byte pos = mem_read(program_counter);
            address = pos + (int)register_y; // 指针偏移
        }
        break;

    case AddressingMode::Absolute_X:
        {
            uint16_t base = mem_read_u16(program_counter);
            address = base + (int)register_x; // 指针偏移
        }
        break;

    case AddressingMode::Absolute_Y:
        {
            uint16_t base = mem_read_u16(program_counter);
            address = base + (int)register_y; // 指针偏移
        }
        break;


    // 先加上X寄存器的值，得到一个地址，再取地址里面保存的两个值，按照小端存储的方式，构成新的地址
    case AddressingMode::Indirect_X:
        {
            byte base = mem_read(program_counter);

            byte ptr = base + (int)register_x;
            byte lo = mem_read(ptr);
            byte hi = mem_read(ptr + 1);
            address = hi << 8 | lo;
        }
        break;

    case AddressingMode::Indirect_Y:
        {
            byte base = mem_read(program_counter);

            byte ptr = base + (int)register_y;
            byte lo = mem_read(ptr);
            byte hi = mem_read(ptr + 1);
            address = hi << 8 | lo;
        }
        break;


    case AddressingMode::NoneAddressing:
        {
            printf("not supported mode: %d", mode);
            throw "not supported mode";
        }
        break;

    default:
        break;
    }

    return static_cast<uint16_t>(AddressingMode::NoneAddressing);
}

void CPU::add_to_register_a(byte value)
{
    int s = register_a + (status & 0x01) + value;

    // 三种判断方式
    // 1. 判断两个符号位相同并且和结果的符号位相同则溢出
    // 2. c6^c7 来判断是否溢出
    // 2. 判断结果是不是比其中的一个数大，因为溢出的时候一定满足result < X AND result < y 

    // carry flag
    if (s > 0xff)
    {
        status = status | 0x01;
    }
    else
    {
        status = status & 0xfe;
    }

    byte res = s;

    // overflow flag
    if ((res ^ value) & (res ^ register_a) & 0x80)
    {
        status = status | 0x40;
    }
    else
    {
        status = status & 0xBF;
    }

    register_a = res;
    update_zero_and_negative_flags(res);
}

ostream& operator<<(ostream& out, CPU& cpu)
{
    printf("current cpu state: programe counter: %d, stack pointer: %d, register A: %d, register X: %d, register Y: %d, status: %d", cpu.program_counter, cpu.stack_pointer,
           cpu.register_a, cpu.register_x, cpu.register_y, cpu.status);
    return out;
}

ostream& operator<<(ostream& out, CPU* cpu)
{
    printf("current CPU state: programe counter: %#X, stack pointer: %#X, register A: %d, register X: %d, register Y: %d\n", cpu->program_counter, cpu->stack_pointer,
           cpu->register_a, cpu->register_x, cpu->register_y);
    int n = 8;
    int x = cpu->status;
    vector<int> res;
    while (n-- > 0)
    {
        res.push_back(x & 0x01);
        x >>= 1;
    }
    std::printf("CPU status: ");

    unordered_map<int, char> status_ch_map{
        {0, 'C'},
        {1, 'Z'},
        {2, 'I'},
        {3, 'D'},
        {4, 'B'},
        {5, ' '},
        {6, 'V'},
        {7, 'N'}
    };
    for (int i = res.size() - 1; i >= 0; i--)
    {
        std::printf("%c:%d ", status_ch_map[i], res[i]);
    }

    return out;
}
