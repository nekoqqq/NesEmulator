#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 辅助：检查 Z, N 标志（DEC 不影响 C, V, I, D, B）
static void check_flags(const CPU& cpu, bool zero, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 零页模式 ----------
static void test_dec_zero_page() {
    word addr = 0x10;

    // 测试1：从 0x01 减到 0x00，Z=1, N=0
    {
        CPU cpu;
        cpu.mem_write(addr, 0x01);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        OpCode::dec(cpu, ZeroPage);
        assert(cpu.mem_read(addr) == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[DEC] ZeroPage test 1 passed\n";
    }

    // 测试2：从 0x00 减到 0xFF，Z=0, N=1
    {
        CPU cpu;
        cpu.mem_write(addr, 0x00);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        OpCode::dec(cpu, ZeroPage);
        assert(cpu.mem_read(addr) == 0xFF);
        check_flags(cpu, false, true);
        std::cout << "[DEC] ZeroPage test 2 passed\n";
    }

    // 测试3：从 0x80 减到 0x7F，Z=0, N=0（bit7 清零）
    {
        CPU cpu;
        cpu.mem_write(addr, 0x80);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        OpCode::dec(cpu, ZeroPage);
        assert(cpu.mem_read(addr) == 0x7F);
        check_flags(cpu, false, false);
        std::cout << "[DEC] ZeroPage test 3 passed\n";
    }
}

// ---------- 零页 X 模式 ----------
static void test_dec_zero_page_x() {
    byte base = 0x10;
    byte x_val = 5;
    word target = (base + x_val) & 0xFF; // 0x15

    // 测试1：递减后结果为 0x00
    {
        CPU cpu;
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0x01);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), base);
        cpu.ResetStatus();
        OpCode::dec(cpu, ZeroPage_X);
        assert(cpu.mem_read(target) == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[DEC] ZeroPageX test 1 passed\n";
    }

    // 测试2：绕回，0x00 -> 0xFF
    {
        CPU cpu;
        cpu.SetRegisterX(1);
        cpu.mem_write(0x00, 0x00);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0xFF);
        cpu.ResetStatus();
        OpCode::dec(cpu, ZeroPage_X);
        assert(cpu.mem_read(0x00) == 0xFF);
        check_flags(cpu, false, true);
        std::cout << "[DEC] ZeroPageX test 2 (wrap) passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_dec_absolute() {
    word addr = 0x1234;

    // 测试1：0x02 -> 0x01
    {
        CPU cpu;
        cpu.mem_write(addr, 0x02);
        cpu.SetPC(0x2000);
        cpu.mem_write_u16(cpu.GetPC(), addr);
        cpu.ResetStatus();
        OpCode::dec(cpu, Absolute);
        assert(cpu.mem_read(addr) == 0x01);
        check_flags(cpu, false, false);
        std::cout << "[DEC] Absolute test 1 passed\n";
    }

    // 测试2：0x01 -> 0x00，Z=1
    {
        CPU cpu;
        cpu.mem_write(addr, 0x01);
        cpu.SetPC(0x2000);
        cpu.mem_write_u16(cpu.GetPC(), addr);
        cpu.ResetStatus();
        OpCode::dec(cpu, Absolute);
        assert(cpu.mem_read(addr) == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[DEC] Absolute test 2 passed\n";
    }
}

// ---------- 绝对 X 模式 ----------
static void test_dec_absolute_x() {
    word base = 0x1200;
    byte x_val = 0x34;
    word target = base + x_val; // 0x1234

    // 测试1：正常递减
    {
        CPU cpu;
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0x7F);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base);
        cpu.ResetStatus();
        OpCode::dec(cpu, Absolute_X);
        assert(cpu.mem_read(target) == 0x7E);
        check_flags(cpu, false, false);
        std::cout << "[DEC] AbsoluteX test 1 passed\n";
    }

    // 测试2：递减后负标志
    {
        CPU cpu;
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0x80);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base);
        cpu.ResetStatus();
        OpCode::dec(cpu, Absolute_X);
        assert(cpu.mem_read(target) == 0x7F);
        check_flags(cpu, false, false); // 0x7F 最高位为 0，N=0
        std::cout << "[DEC] AbsoluteX test 2 passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_dec() {
    std::cout << "[DEC] Running tests...\n";
    test_dec_zero_page();
    test_dec_zero_page_x();
    test_dec_absolute();
    test_dec_absolute_x();
    std::cout << "[DEC] All tests passed.\n\n";
}