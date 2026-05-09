// test_ldx.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool zero, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 立即数模式 ----------
static void test_ldx_immediate() {
    {
        CPU cpu;
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x42);
        cpu.ResetStatus();
        OpCode::ldx(cpu, Immediate);
        assert(cpu.GetRegisterX() == 0x42);
        check_flags(cpu, false, false);
        std::cout << "[LDX] Immediate test 1 passed\n";
    }
    {
        CPU cpu;
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x00);
        cpu.ResetStatus();
        OpCode::ldx(cpu, Immediate);
        assert(cpu.GetRegisterX() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[LDX] Immediate test 2 (zero) passed\n";
    }
    {
        CPU cpu;
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x80);
        cpu.ResetStatus();
        OpCode::ldx(cpu, Immediate);
        assert(cpu.GetRegisterX() == 0x80);
        check_flags(cpu, false, true);
        std::cout << "[LDX] Immediate test 3 (negative) passed\n";
    }
}

// ---------- 零页模式 ----------
static void test_ldx_zero_page() {
    word addr = 0x10;
    {
        CPU cpu;
        cpu.mem_write(addr, 0x55);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        OpCode::ldx(cpu, ZeroPage);
        assert(cpu.GetRegisterX() == 0x55);
        check_flags(cpu, false, false);
        std::cout << "[LDX] ZeroPage test 1 passed\n";
    }
    {
        CPU cpu;
        cpu.mem_write(addr, 0x00);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        OpCode::ldx(cpu, ZeroPage);
        assert(cpu.GetRegisterX() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[LDX] ZeroPage test 2 (zero) passed\n";
    }
}

// ---------- 零页 Y 模式 ----------
static void test_ldx_zero_page_y() {
    byte base = 0x10;
    byte y_val = 5;
    word target = (base + y_val) & 0xFF; // 0x15
    {
        CPU cpu;
        cpu.SetRegisterY(y_val);
        cpu.mem_write(target, 0xAA);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), base);
        cpu.ResetStatus();
        OpCode::ldx(cpu, ZeroPage_Y);
        assert(cpu.GetRegisterX() == 0xAA);
        check_flags(cpu, false, true);
        std::cout << "[LDX] ZeroPageY test 1 passed\n";
    }
    // 绕回测试：0xFF + 1 = 0x00
    {
        CPU cpu;
        cpu.SetRegisterY(1);
        cpu.mem_write(0x00, 0x80);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0xFF);
        cpu.ResetStatus();
        OpCode::ldx(cpu, ZeroPage_Y);
        assert(cpu.GetRegisterX() == 0x80);
        check_flags(cpu, false, true);
        std::cout << "[LDX] ZeroPageY test 2 (wrap) passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_ldx_absolute() {
    word addr = 0x1234;
    {
        CPU cpu;
        cpu.mem_write(addr, 0x33);
        cpu.SetPC(0x2000);
        cpu.mem_write_u16(cpu.GetPC(), addr);
        cpu.ResetStatus();
        OpCode::ldx(cpu, Absolute);
        assert(cpu.GetRegisterX() == 0x33);
        check_flags(cpu, false, false);
        std::cout << "[LDX] Absolute test 1 passed\n";
    }
}

// ---------- 绝对 Y 模式 ----------
static void test_ldx_absolute_y() {
    word base = 0x1200;
    byte y_val = 0x34;
    word target = base + y_val; // 0x1234
    {
        CPU cpu;
        cpu.SetRegisterY(y_val);
        cpu.mem_write(target, 0x7F);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base);
        cpu.ResetStatus();
        OpCode::ldx(cpu, Absolute_Y);
        assert(cpu.GetRegisterX() == 0x7F);
        check_flags(cpu, false, false);
        std::cout << "[LDX] AbsoluteY test 1 passed\n";
    }
    // 跨页测试
    {
        CPU cpu;
        word base2 = 0x12FF;
        byte y_val2 = 2;
        word target2 = base2 + y_val2; // 0x1301
        cpu.SetRegisterY(y_val2);
        cpu.mem_write(target2, 0x01);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base2);
        cpu.ResetStatus();
        OpCode::ldx(cpu, Absolute_Y);
        assert(cpu.GetRegisterX() == 0x01);
        check_flags(cpu, false, false);
        std::cout << "[LDX] AbsoluteY test 2 (page cross) passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_ldx() {
    std::cout << "[LDX] Running tests...\n";
    test_ldx_immediate();
    test_ldx_zero_page();
    test_ldx_zero_page_y();
    test_ldx_absolute();
    test_ldx_absolute_y();
    std::cout << "[LDX] All tests passed.\n\n";
}