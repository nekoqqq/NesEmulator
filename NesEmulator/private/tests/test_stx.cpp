// test_stx.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags_unchanged(const CPU& cpu, byte expected_status) {
    assert(cpu.get_status() == expected_status);
}

// ---------- 零页模式 ----------
static void test_stx_zero_page() {
    word addr = 0x10;
    {
        CPU cpu;
        cpu.set_x(0x55);
        cpu.mem_write(addr, 0x00);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        byte expected_status = cpu.get_status();
        OpCode::stx(cpu, ZeroPage);
        assert(cpu.mem_read(addr) == 0x55);
        assert(cpu.get_x() == 0x55);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[STX] ZeroPage test passed\n";
    }
}

// ---------- 零页 Y 模式 ----------
static void test_stx_zero_page_y() {
    byte base = 0x10;
    byte y_val = 5;
    word target = (base + y_val) & 0xFF; // 0x15
    {
        CPU cpu;
        cpu.set_x(0xAA);
        cpu.set_y(y_val);
        cpu.mem_write(target, 0x00);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), base);
        cpu.reset_stats();
        byte expected_status = cpu.get_status();
        OpCode::stx(cpu, ZeroPage_Y);
        assert(cpu.mem_read(target) == 0xAA);
        assert(cpu.get_x() == 0xAA);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[STX] ZeroPageY test passed\n";
    }
    // 绕回测试
    {
        CPU cpu;
        cpu.set_x(0x77);
        cpu.set_y(1);
        cpu.mem_write(0x00, 0x00);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0xFF);
        cpu.reset_stats();
        byte expected_status = cpu.get_status();
        OpCode::stx(cpu, ZeroPage_Y);
        assert(cpu.mem_read(0x00) == 0x77);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[STX] ZeroPageY wrap test passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_stx_absolute() {
    word addr = 0x1234;
    {
        CPU cpu;
        cpu.set_x(0x42);
        cpu.mem_write(addr, 0x00);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        byte expected_status = cpu.get_status();
        OpCode::stx(cpu, Absolute);
        assert(cpu.mem_read(addr) == 0x42);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[STX] Absolute test passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_stx() {
    std::cout << "[STX] Running tests...\n";
    test_stx_zero_page();
    test_stx_zero_page_y();
    test_stx_absolute();
    std::cout << "[STX] All tests passed.\n\n";
}