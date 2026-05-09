// test_jmp.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 辅助：检查 PC 是否等于期望值
static void check_pc(const CPU& cpu, word expected)
{
    assert(cpu.get_pc() == expected);
}

// ---------- 绝对寻址 ----------
static void test_jmp_absolute()
{
    // 测试1：直接跳转到指定地址
    {
        CPU cpu;
        word target = 0x1234;
        cpu.set_pc(0x2000);
        // 在 PC 处写入操作数（绝对地址）
        cpu.mem_write_word(cpu.get_pc(), target);
        cpu.reset_stats();
        OpCode::jmp(cpu, Absolute);
        check_pc(cpu, target);
        std::cout << "[JMP] Absolute test 1 passed\n";
    }
    // 测试2：跳转到边界地址
    {
        CPU cpu;
        word target = 0xFFFC;
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), target);
        cpu.reset_stats();
        OpCode::jmp(cpu, Absolute);
        check_pc(cpu, target);
        std::cout << "[JMP] Absolute test 2 passed\n";
    }
}

// ---------- 间接寻址 ----------
static void test_jmp_indirect()
{
    // 测试1：从零页指针读取目标地址
    {
        CPU cpu;
        word ptr_addr = 0x0010; // 明确16位，高字节0x00
        word target = 0x5678;
        cpu.mem_write(ptr_addr, target & 0xFF);
        cpu.mem_write(ptr_addr + 1, target >> 8);
        cpu.set_pc(0x4000);
        cpu.mem_write_word(cpu.get_pc(), ptr_addr); // 写入16位操作数
        cpu.reset_stats();
        OpCode::jmp(cpu, Indirect);
        check_pc(cpu, target);
        std::cout << "[JMP] Indirect test 1 passed\n";
    }
    // 测试2：间接指针位于绝对地址范围
    {
        CPU cpu;
        word ptr_addr = 0x2000;
        word target = 0xDEAD;
        cpu.mem_write(ptr_addr, target & 0xFF);
        cpu.mem_write(ptr_addr + 1, target >> 8);
        cpu.set_pc(0x5000);
        // 操作数是 16 位指针地址
        cpu.mem_write_word(cpu.get_pc(), ptr_addr);
        cpu.reset_stats();
        OpCode::jmp(cpu, Indirect);
        check_pc(cpu, target);
        std::cout << "[JMP] Indirect test 2 passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_jmp()
{
    std::cout << "[JMP] Running tests...\n";
    test_jmp_absolute();
    test_jmp_indirect();
    std::cout << "[JMP] All tests passed.\n\n";
}
