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
public:
    CPU(byte programe_counter = 0, byte stack_pointer = 0, byte register_a = 0, byte register_x = 0, byte register_y = 0, byte status = 0);

    void interpret(vector<byte>& program);

    byte mem_read(uint16_t addr) const;

    void mem_write(uint16_t addr, byte data);

    // 将ROM加载到内存中
    void load(vector<byte>& program);

    // 加载ROM并运行程序
    void load_and_run(vector<byte>& program);

    // 给一个16位的地址，需要读取连续两个字节的数据，也就是连续两行的数据，这两行的数据再根据小端的方式拼接
    uint16_t mem_read_u16(uint16_t pos) const;

    // 给一个16位的地址，写入一个16位的操作数，用小端的方式，先写低八位的数据，再写高8位的数据
    // 记住，内存总共64KB大小，总共有64K行，每行都是8位，存一个Byte，数据就存在这些cell里面
    void mem_write_u16(uint16_t pos, uint16_t data);

    // 重置寄存器的状态，将program_counter的值设置为在内存0xFFFC处存储的2个字节的值
    void reset();

    void run();

protected:
    // 输出的处理函数
    friend ostream& operator<<(ostream& out, CPU& cpu);

    friend ostream& operator<<(ostream& out, CPU* cpu);

    bool brk(vector<byte>& program);
    bool lda(vector<byte>& program);
    bool lda(AddressingMode mode);
    bool sta(AddressingMode mode);
    bool tax(vector<byte>& program);
    bool inx(vector<byte>& program);
    bool iny(vector<byte>& program);
    bool adc(AddressingMode mode);
    bool sbc(AddressingMode mode);
    bool op_and(AddressingMode mode);
    bool op_eor(AddressingMode mode);
    bool op_ora(AddressingMode mode);
    bool asl(AddressingMode mode);
    bool asr(AddressingMode mode);
    bool bcc(AddressingMode mode);
    bool bcs(AddressingMode mode);

    // Branch On Equal
    bool beq(AddressingMode);

    // Branch On Not Equal
    bool bne(AddressingMode);
    bool bpl(AddressingMode);
    bool bmi(AddressingMode);
    bool bvc(AddressingMode);
    bool bvs(AddressingMode);

private:
    // 通用的设置寄存器的函数
    void update_zero_and_negative_flags(byte res);
    uint16_t get_operand_address(AddressingMode& mode) const;
    void add_to_register_a(byte value);

public: // 暂时不封装，TODO后面还是需要封装下
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
};
