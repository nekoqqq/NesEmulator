#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 辅助：检查 Z、N、V 标志（C 不影响）
static void check_flags(const CPU& cpu, bool zero, bool negative, bool overflow) {
    byte status = cpu.get_status();
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
    assert(((status >> 6) & 1) == overflow);
}

// ---------- 零页模式 ----------
static void test_bit_zero_page() {
    word addr = 0x10;

    // 测试1：A & M = 0（Z=1），M bit7=0，bit6=0
    {
        CPU cpu;
        cpu.set_a(0x00);
        cpu.mem_write(addr, 0x00);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        OpCode::bit(cpu, ZeroPage);
        assert(cpu.get_a() == 0x00);          // A 不变
        check_flags(cpu, true, false, false);
        std::cout << "[BIT] ZeroPage test 1 passed\n";
    }

    // 测试2：A & M != 0（Z=0），M bit7=1，bit6=0
    {
        CPU cpu;
        cpu.set_a(0x80);
        cpu.mem_write(addr, 0x80);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        OpCode::bit(cpu, ZeroPage);
        check_flags(cpu, false, true, false);
        std::cout << "[BIT] ZeroPage test 2 passed\n";
    }

    // 测试3：A & M != 0（Z=0），M bit7=0，bit6=1
    {
        CPU cpu;
        cpu.set_a(0x40);
        cpu.mem_write(addr, 0x40);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        OpCode::bit(cpu, ZeroPage);
        check_flags(cpu, false, false, true);
        std::cout << "[BIT] ZeroPage test 3 passed\n";
    }

    // 测试4：A & M = 0，但 M bit7=1，bit6=1（Z=1，N=1，V=1）
    {
        CPU cpu;
        cpu.set_a(0x00);
        cpu.mem_write(addr, 0xC0);   // 1100 0000
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        OpCode::bit(cpu, ZeroPage);
        check_flags(cpu, true, true, true);
        std::cout << "[BIT] ZeroPage test 4 passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_bit_absolute() {
    word addr = 0x1234;

    // 测试1：A & M = 0，M bit7=1，bit6=1
    {
        CPU cpu;
        cpu.set_a(0x00);
        cpu.mem_write(addr, 0xC0);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        OpCode::bit(cpu, Absolute);
        assert(cpu.get_a() == 0x00);
        check_flags(cpu, true, true, true);
        std::cout << "[BIT] Absolute test 1 passed\n";
    }

    // 测试2：A & M != 0，M bit7=0，bit6=1
    {
        CPU cpu;
        cpu.set_a(0x40);
        cpu.mem_write(addr, 0x40);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        OpCode::bit(cpu, Absolute);
        check_flags(cpu, false, false, true);
        std::cout << "[BIT] Absolute test 2 passed\n";
    }

    // 测试3：A & M != 0，M bit7=1，bit6=0
    {
        CPU cpu;
        cpu.set_a(0x80);
        cpu.mem_write(addr, 0x80);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        OpCode::bit(cpu, Absolute);
        check_flags(cpu, false, true, false);
        std::cout << "[BIT] Absolute test 3 passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_bit() {
    std::cout << "[BIT] Running tests...\n";
    test_bit_zero_page();
    test_bit_absolute();
    std::cout << "[BIT] All tests passed.\n\n";
}