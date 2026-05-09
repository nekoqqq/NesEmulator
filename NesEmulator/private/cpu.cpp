#include "../public/cpu.h"
#include "../public/op_code.h"


CPU::CPU(byte program_counter, byte stack_pointer, byte register_a, byte register_x, byte register_y, byte status) : program_counter(program_counter),
                                                                                                                     stack_pointer(stack_pointer), register_a(register_a),
                                                                                                                     register_x(register_x), register_y(register_y), status(status)
{
}

void CPU::load_and_run(vector<byte>& program)
{
    load(program);
    reset();
    run();
}

void CPU::load_and_run_no_reset(program& prog)
{
    load(prog);
    program_counter = mem_read_u16(0xFFFC);
    run();
}

byte CPU::mem_read(word addr) const
{
    return memory[addr];
}

void CPU::mem_write(word addr, byte data)
{
    memory[addr] = data;
}

word CPU::mem_read_u16(word pos) const
{
    byte lo = mem_read(pos);
    byte hi = mem_read(pos + 1);
    return static_cast<word>((hi << 8) | lo);
}

void CPU::mem_write_u16(word pos, word data)
{
    byte hi = (data >> 8); // 高8位
    byte lo = (data & 0xff); // 低8位
    mem_write(pos, lo);
    mem_write(pos + 1, hi);
}

ostream& operator<<(ostream& out, CPU* cpu)
{
    printf("current CPU state:\nprogram counter: %#X, stack pointer: %#X, register A: %d, register X: %d, register Y: %d\n", cpu->program_counter, cpu->stack_pointer,
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

bool CPU::interpret()
{
    // 取指
    byte code = mem_read(program_counter++);
    uint16_t program_counter_state = program_counter;
    if (OpCode::OPCODES_MAP.find(code) == OpCode::OPCODES_MAP.end())
    {
        printf("unsupported code: 0x%X\n", code);
        return false;
    }
    OpCode op = OpCode::OPCODES_MAP[code];
    if (!op.execute(*this))
    {
        std::printf("execute failed opcode: %x", code);
        return false;
    }
    if (op.isHalt())
    {
        printf("halt executed...\n");
        return false;
    }
    if (program_counter_state == program_counter)
    {
        program_counter += op.size - 1;
    }
    return true;
}

void CPU::load(vector<byte>& program)
{
    // 从32 KB处开始加载程序,程序计数器也设置为此,ROM加载到此内存里面，并不代表实际就从0x8000开始执行了
    memcpy(memory + 0x8000, program.data(), program.size() * sizeof(byte));

    // 在NES插入卡带后，会重置程序状态，并将程序计数器的位置设置为存储在0xFFFC处的值,[X,X,X,X] 也就是最后4个字节
    // 将0xFFFC处存储的值设置为0x8000;
    mem_write_u16(0xFFFC, 0x8000);
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
        if (!interpret())
            break;
    }
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

word CPU::get_operand_address(AddressingMode& mode) const
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
            address = (pos + register_x) & 0xff; // 只取低8位
        }
        break;

    // 用Y里面存储的值+一个byte的数据做索引
    case AddressingMode::ZeroPage_Y:
        {
            byte pos = mem_read(program_counter);
            address = (pos + register_y) & 0xff; // 只取低8位
        }
        break;

    case AddressingMode::Absolute_X:
        {
            uint16_t base = mem_read_u16(program_counter);
            address = base + register_x; // 指针偏移
        }
        break;

    case AddressingMode::Absolute_Y:
        {
            uint16_t base = mem_read_u16(program_counter);
            address = base + register_y; // 指针偏移
        }
        break;


    // 先加上X寄存器的值，得到一个地址，再取地址里面保存的两个值，按照小端存储的方式，构成新的地址
    case AddressingMode::Indirect_X:
        {
            byte base = mem_read(program_counter);

            byte ptr = base + register_x;
            byte lo = mem_read(ptr);
            byte hi = mem_read(ptr + 1);
            address = static_cast<word>(hi << 8 | lo);
        }
        break;

    case AddressingMode::Indirect_D_Y:
        {
            byte base = mem_read(program_counter); // 零页基址
            word ptr = static_cast<word>(mem_read(base) | (mem_read(base + 1) << 8));
            address = ptr + register_y;
        }
        break;


    case AddressingMode::NoneAddressing:
    default:
        {
            printf("not supported mode: %d", mode);
            throw "not supported mode";
        }
    }

    return address;
}

void CPU::add_to_register_a(byte value)
{
    int s = register_a + (status & CARRY_FLAG) + value;

    // 三种判断方式
    // 1. 判断两个符号位相同并且和结果的符号位相同则溢出
    // 2. c6^c7 来判断是否溢出
    // 2. 判断结果是不是比其中的一个数大，因为溢出的时候一定满足result < X AND result < y 

    // carry flag
    SetFlag(CARRY_FLAG, s > 0xff);

    byte res = s;

    // overflow flag
    bool bOverflow = (res ^ value) & (res ^ register_a) & 0x80;
    SetFlag(OVERFLOW_FLAG, bOverflow);

    register_a = res;
    update_zero_and_negative_flags(res);
}
