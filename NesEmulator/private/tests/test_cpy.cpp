#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 检查 C, Z, N 标志
static void check_flags(const CPU& cpu, bool carry, bool zero, bool negative) {
    byte status = cpu.get_status();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 立即数模式 ----------
static void test_cpy_immediate() {
    // 测试1：Y == M
    {
        CPU cpu;
        cpu.set_y(0x42);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0x42);
        cpu.reset_stats();
        OpCode::cpy(cpu, Immediate);
        assert(cpu.get_y() == 0x42);
        check_flags(cpu, true, true, false);
        std::cout << "[CPY] Immediate test 1 (equal) passed\n";
    }
    // 测试2：Y > M
    {
        CPU cpu;
        cpu.set_y(0x50);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0x30);
        cpu.reset_stats();
        OpCode::cpy(cpu, Immediate);
        check_flags(cpu, true, false, false);
        std::cout << "[CPY] Immediate test 2 (Y > M) passed\n";
    }
    // 测试3：Y < M
    {
        CPU cpu;
        cpu.set_y(0x20);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0x80);
        cpu.reset_stats();
        OpCode::cpy(cpu, Immediate);
        check_flags(cpu, false, false, true);
        std::cout << "[CPY] Immediate test 3 (Y < M) passed\n";
    }
    // 边界：Y=0x80, M=0x80
    {
        CPU cpu;
        cpu.set_y(0x80);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0x80);
        cpu.reset_stats();
        OpCode::cpy(cpu, Immediate);
        check_flags(cpu, true, true, false);
        std::cout << "[CPY] Immediate test 4 (boundary equal) passed\n";
    }
}

// ---------- 零页模式 ----------
static void test_cpy_zero_page() {
    word addr = 0x10;
    // Y < M
    {
        CPU cpu;
        cpu.set_y(0x10);
        cpu.mem_write(addr, 0x20);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        OpCode::cpy(cpu, ZeroPage);
        check_flags(cpu, false, false, true);
        std::cout << "[CPY] ZeroPage test 1 (Y < M) passed\n";
    }
    // Y == M
    {
        CPU cpu;
        cpu.set_y(0x30);
        cpu.mem_write(addr, 0x30);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        OpCode::cpy(cpu, ZeroPage);
        check_flags(cpu, true, true, false);
        std::cout << "[CPY] ZeroPage test 2 (equal) passed\n";
    }
    // Y > M
    {
        CPU cpu;
        cpu.set_y(0x40);
        cpu.mem_write(addr, 0x30);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        OpCode::cpy(cpu, ZeroPage);
        check_flags(cpu, true, false, false);
        std::cout << "[CPY] ZeroPage test 3 (Y > M) passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_cpy_absolute() {
    word addr = 0x1234;
    // Y == M
    {
        CPU cpu;
        cpu.set_y(0x55);
        cpu.mem_write(addr, 0x55);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        OpCode::cpy(cpu, Absolute);
        check_flags(cpu, true, true, false);
        std::cout << "[CPY] Absolute test 1 (equal) passed\n";
    }
    // Y < M
    {
        CPU cpu;
        cpu.set_y(0x10);
        cpu.mem_write(addr, 0x20);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        OpCode::cpy(cpu, Absolute);
        check_flags(cpu, false, false, true);
        std::cout << "[CPY] Absolute test 2 (Y < M) passed\n";
    }
    // Y > M
    {
        CPU cpu;
        cpu.set_y(0x70);
        cpu.mem_write(addr, 0x60);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        OpCode::cpy(cpu, Absolute);
        check_flags(cpu, true, false, false);
        std::cout << "[CPY] Absolute test 3 (Y > M) passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_cpy() {
    std::cout << "[CPY] Running tests...\n";
    test_cpy_immediate();
    test_cpy_zero_page();
    test_cpy_absolute();
    std::cout << "[CPY] All tests passed.\n\n";
}