// test_ldy.cpp
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
static void test_ldy_immediate() {
    {
        CPU cpu;
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x42);
        cpu.ResetStatus();
        OpCode::ldy(cpu, Immediate);
        assert(cpu.GetRegisterY() == 0x42);
        check_flags(cpu, false, false);
        std::cout << "[LDY] Immediate test 1 passed\n";
    }
    {
        CPU cpu;
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x00);
        cpu.ResetStatus();
        OpCode::ldy(cpu, Immediate);
        assert(cpu.GetRegisterY() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[LDY] Immediate test 2 (zero) passed\n";
    }
    {
        CPU cpu;
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x80);
        cpu.ResetStatus();
        OpCode::ldy(cpu, Immediate);
        assert(cpu.GetRegisterY() == 0x80);
        check_flags(cpu, false, true);
        std::cout << "[LDY] Immediate test 3 (negative) passed\n";
    }
}

// ---------- 零页模式 ----------
static void test_ldy_zero_page() {
    word addr = 0x10;
    {
        CPU cpu;
        cpu.mem_write(addr, 0x55);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        OpCode::ldy(cpu, ZeroPage);
        assert(cpu.GetRegisterY() == 0x55);
        check_flags(cpu, false, false);
        std::cout << "[LDY] ZeroPage test 1 passed\n";
    }
    {
        CPU cpu;
        cpu.mem_write(addr, 0x00);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        OpCode::ldy(cpu, ZeroPage);
        assert(cpu.GetRegisterY() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[LDY] ZeroPage test 2 (zero) passed\n";
    }
}

// ---------- 零页 X 模式 ----------
static void test_ldy_zero_page_x() {
    byte base = 0x10;
    byte x_val = 5;
    word target = (base + x_val) & 0xFF; // 0x15
    {
        CPU cpu;
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0xAA);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), base);
        cpu.ResetStatus();
        OpCode::ldy(cpu, ZeroPage_X);
        assert(cpu.GetRegisterY() == 0xAA);
        check_flags(cpu, false, true);
        std::cout << "[LDY] ZeroPageX test 1 passed\n";
    }
    // 绕回测试：0xFF + 1 = 0x00
    {
        CPU cpu;
        cpu.SetRegisterX(1);
        cpu.mem_write(0x00, 0x80);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0xFF);
        cpu.ResetStatus();
        OpCode::ldy(cpu, ZeroPage_X);
        assert(cpu.GetRegisterY() == 0x80);
        check_flags(cpu, false, true);
        std::cout << "[LDY] ZeroPageX test 2 (wrap) passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_ldy_absolute() {
    word addr = 0x1234;
    {
        CPU cpu;
        cpu.mem_write(addr, 0x33);
        cpu.SetPC(0x2000);
        cpu.mem_write_u16(cpu.GetPC(), addr);
        cpu.ResetStatus();
        OpCode::ldy(cpu, Absolute);
        assert(cpu.GetRegisterY() == 0x33);
        check_flags(cpu, false, false);
        std::cout << "[LDY] Absolute test 1 passed\n";
    }
}

// ---------- 绝对 X 模式 ----------
static void test_ldy_absolute_x() {
    word base = 0x1200;
    byte x_val = 0x34;
    word target = base + x_val; // 0x1234
    {
        CPU cpu;
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0x7F);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base);
        cpu.ResetStatus();
        OpCode::ldy(cpu, Absolute_X);
        assert(cpu.GetRegisterY() == 0x7F);
        check_flags(cpu, false, false);
        std::cout << "[LDY] AbsoluteX test 1 passed\n";
    }
    // 跨页测试
    {
        CPU cpu;
        word base2 = 0x12FF;
        byte x_val2 = 2;
        word target2 = base2 + x_val2; // 0x1301
        cpu.SetRegisterX(x_val2);
        cpu.mem_write(target2, 0x01);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base2);
        cpu.ResetStatus();
        OpCode::ldy(cpu, Absolute_X);
        assert(cpu.GetRegisterY() == 0x01);
        check_flags(cpu, false, false);
        std::cout << "[LDY] AbsoluteX test 2 (page cross) passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_ldy() {
    std::cout << "[LDY] Running tests...\n";
    test_ldy_immediate();
    test_ldy_zero_page();
    test_ldy_zero_page_x();
    test_ldy_absolute();
    test_ldy_absolute_x();
    std::cout << "[LDY] All tests passed.\n\n";
}