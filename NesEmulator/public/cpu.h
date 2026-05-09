/*	6502 CPU模拟
* 采用小端方式，也就是说一个内存地址0x8000
* 在大端下是80 00，而在小端下是00 80
* 例如 LDA $8000 <=> ad 00 80
 */
#pragma once

#include<iostream>
#include<vector>
#include<unordered_map>
#include "platform_common.h"

using std::vector;
using std::ostream;

class CPU
{
    friend struct OpCode;

public:
    CPU(byte program_counter = 0, byte stack_pointer = 0, byte register_a = 0, byte register_x = 0, byte register_y = 0, byte status = 0);

    // 加载ROM并运行程序
    void load_and_run(program& prog);
    // 不清空直接运行
    void load_and_run_no_reset(program& prog);
    byte mem_read(uint16_t addr) const;
    void mem_write(uint16_t addr, byte data);
    // 给一个16位的地址，需要读取连续两个字节的数据，也就是连续两行的数据，这两行的数据再根据小端的方式拼接
    uint16_t mem_read_u16(uint16_t pos) const;
    // 给一个16位的地址，写入一个16位的操作数，用小端的方式，先写低八位的数据，再写高8位的数据
    // 记住，内存总共64KB大小，总共有64K行，每行都是8位，存一个Byte，数据就存在这些cell里面
    void mem_write_u16(uint16_t pos, uint16_t data);

protected:
    // 输出的处理函数
    friend ostream& operator<<(ostream& out, CPU* cpu);

private:
    bool interpret(); // 单步执行指令
    // 将ROM加载到内存中
    void load(program& prog);
    // 重置寄存器的状态，将program_counter的值设置为在内存0xFFFC处存储的2个字节的值
    void reset();
    // 运行内存中的程序
    void run();
    // 通用的设置寄存器的函数
    void update_zero_and_negative_flags(byte res);
    uint16_t get_operand_address(AddressingMode& mode) const;
    void add_to_register_a(byte value);

    // 程序计数器
    uint16_t program_counter;
    byte stack_pointer;

    // 通用寄存器
    byte register_a;
    byte register_x;
    byte register_y;

    // 程序状态字寄存器, N V _ B D I Z C
    //				   7 6 5 4 3 2 1 0
    byte status;

    // 内存 address space,虚拟地址空间,CPU只有2KB的RAM，其他的都是用来做内存映射
    byte memory[MEM_SIZE];

public: // public getter
    const byte& GetStatus() const { return status; }
    const byte& GetRegisterA() const { return register_a; }
    void SetRegisterA(byte newRegisterA) { register_a = newRegisterA; }
    const byte& GetRegisterX() const { return register_x; }
    void SetRegisterX(byte newRegisterX) { register_x = newRegisterX; }
    const byte& GetRegisterY() const { return register_y; }
    void SetRegisterY(byte newRegisterY) { register_y = newRegisterY; }
    
};
