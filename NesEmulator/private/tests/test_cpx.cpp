#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 检查 C, Z, N 标志
static void check_flags(const CPU& cpu, bool carry, bool zero, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 立即数模式 ----------
static void test_cpx_immediate() {
    // 测试1：X == M
    {
        CPU cpu;
        cpu.SetRegisterX(0x42);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x42);
        cpu.ResetStatus();
        OpCode::cpx(cpu, Immediate);
        assert(cpu.GetRegisterX() == 0x42);
        check_flags(cpu, true, true, false);
        std::cout << "[CPX] Immediate test 1 (equal) passed\n";
    }
    // 测试2：X > M
    {
        CPU cpu;
        cpu.SetRegisterX(0x50);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x30);
        cpu.ResetStatus();
        OpCode::cpx(cpu, Immediate);
        check_flags(cpu, true, false, false);
        std::cout << "[CPX] Immediate test 2 (X > M) passed\n";
    }
    // 测试3：X < M
    {
        CPU cpu;
        cpu.SetRegisterX(0x20);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x80);
        cpu.ResetStatus();
        OpCode::cpx(cpu, Immediate);
        check_flags(cpu, false, false, true);
        std::cout << "[CPX] Immediate test 3 (X < M) passed\n";
    }
    // 边界：X=0x80, M=0x80
    {
        CPU cpu;
        cpu.SetRegisterX(0x80);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x80);
        cpu.ResetStatus();
        OpCode::cpx(cpu, Immediate);
        check_flags(cpu, true, true, false);
        std::cout << "[CPX] Immediate test 4 (boundary equal) passed\n";
    }
}

// ---------- 零页模式 ----------
static void test_cpx_zero_page() {
    word addr = 0x10;
    // X < M
    {
        CPU cpu;
        cpu.SetRegisterX(0x10);
        cpu.mem_write(addr, 0x20);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        OpCode::cpx(cpu, ZeroPage);
        check_flags(cpu, false, false, true);
        std::cout << "[CPX] ZeroPage test 1 (X < M) passed\n";
    }
    // X == M
    {
        CPU cpu;
        cpu.SetRegisterX(0x30);
        cpu.mem_write(addr, 0x30);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        OpCode::cpx(cpu, ZeroPage);
        check_flags(cpu, true, true, false);
        std::cout << "[CPX] ZeroPage test 2 (equal) passed\n";
    }
    // X > M
    {
        CPU cpu;
        cpu.SetRegisterX(0x40);
        cpu.mem_write(addr, 0x30);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        OpCode::cpx(cpu, ZeroPage);
        check_flags(cpu, true, false, false);
        std::cout << "[CPX] ZeroPage test 3 (X > M) passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_cpx_absolute() {
    word addr = 0x1234;
    // X == M
    {
        CPU cpu;
        cpu.SetRegisterX(0x55);
        cpu.mem_write(addr, 0x55);
        cpu.SetPC(0x2000);
        cpu.mem_write_u16(cpu.GetPC(), addr);
        cpu.ResetStatus();
        OpCode::cpx(cpu, Absolute);
        check_flags(cpu, true, true, false);
        std::cout << "[CPX] Absolute test 1 (equal) passed\n";
    }
    // X < M
    {
        CPU cpu;
        cpu.SetRegisterX(0x10);
        cpu.mem_write(addr, 0x20);
        cpu.SetPC(0x2000);
        cpu.mem_write_u16(cpu.GetPC(), addr);
        cpu.ResetStatus();
        OpCode::cpx(cpu, Absolute);
        check_flags(cpu, false, false, true);
        std::cout << "[CPX] Absolute test 2 (X < M) passed\n";
    }
    // X > M
    {
        CPU cpu;
        cpu.SetRegisterX(0x70);
        cpu.mem_write(addr, 0x60);
        cpu.SetPC(0x2000);
        cpu.mem_write_u16(cpu.GetPC(), addr);
        cpu.ResetStatus();
        OpCode::cpx(cpu, Absolute);
        check_flags(cpu, true, false, false);
        std::cout << "[CPX] Absolute test 3 (X > M) passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_cpx() {
    std::cout << "[CPX] Running tests...\n";
    test_cpx_immediate();
    test_cpx_zero_page();
    test_cpx_absolute();
    std::cout << "[CPX] All tests passed.\n\n";
}