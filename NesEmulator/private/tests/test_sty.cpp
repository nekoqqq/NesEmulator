// test_sty.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags_unchanged(const CPU& cpu, byte expected_status) {
    assert(cpu.GetStatus() == expected_status);
}

// ---------- 零页模式 ----------
static void test_sty_zero_page() {
    word addr = 0x10;
    {
        CPU cpu;
        cpu.SetRegisterY(0x55);
        cpu.mem_write(addr, 0x00);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        byte expected_status = cpu.GetStatus();
        OpCode::sty(cpu, ZeroPage);
        assert(cpu.mem_read(addr) == 0x55);
        assert(cpu.GetRegisterY() == 0x55);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[STY] ZeroPage test passed\n";
    }
}

// ---------- 零页 X 模式 ----------
static void test_sty_zero_page_x() {
    byte base = 0x10;
    byte x_val = 5;
    word target = (base + x_val) & 0xFF; // 0x15
    {
        CPU cpu;
        cpu.SetRegisterY(0xAA);
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0x00);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), base);
        cpu.ResetStatus();
        byte expected_status = cpu.GetStatus();
        OpCode::sty(cpu, ZeroPage_X);
        assert(cpu.mem_read(target) == 0xAA);
        assert(cpu.GetRegisterY() == 0xAA);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[STY] ZeroPageX test passed\n";
    }
    // 绕回测试
    {
        CPU cpu;
        cpu.SetRegisterY(0x77);
        cpu.SetRegisterX(1);
        cpu.mem_write(0x00, 0x00);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0xFF);
        cpu.ResetStatus();
        byte expected_status = cpu.GetStatus();
        OpCode::sty(cpu, ZeroPage_X);
        assert(cpu.mem_read(0x00) == 0x77);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[STY] ZeroPageX wrap test passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_sty_absolute() {
    word addr = 0x1234;
    {
        CPU cpu;
        cpu.SetRegisterY(0x42);
        cpu.mem_write(addr, 0x00);
        cpu.SetPC(0x2000);
        cpu.mem_write_u16(cpu.GetPC(), addr);
        cpu.ResetStatus();
        byte expected_status = cpu.GetStatus();
        OpCode::sty(cpu, Absolute);
        assert(cpu.mem_read(addr) == 0x42);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[STY] Absolute test passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_sty() {
    std::cout << "[STY] Running tests...\n";
    test_sty_zero_page();
    test_sty_zero_page_x();
    test_sty_absolute();
    std::cout << "[STY] All tests passed.\n\n";
}