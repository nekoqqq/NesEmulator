// test_sta.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags_unchanged(const CPU& cpu, byte expected_status) {
    assert(cpu.get_status() == expected_status);
}

// ---------- 零页模式 ----------
static void test_sta_zero_page() {
    word addr = 0x10;
    {
        CPU cpu;
        cpu.set_a(0x55);
        cpu.mem_write(addr, 0x00);  // 预先清空
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        byte expected_status = cpu.get_status();
        OpCode::sta(cpu, ZeroPage);
        assert(cpu.mem_read(addr) == 0x55);
        assert(cpu.get_a() == 0x55);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[STA] ZeroPage test passed\n";
    }
}

// ---------- 零页 X 模式 ----------
static void test_sta_zero_page_x() {
    byte base = 0x10;
    byte x_val = 5;
    word target = (base + x_val) & 0xFF; // 0x15
    {
        CPU cpu;
        cpu.set_a(0xAA);
        cpu.set_x(x_val);
        cpu.mem_write(target, 0x00);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), base);
        cpu.reset_stats();
        byte expected_status = cpu.get_status();
        OpCode::sta(cpu, ZeroPage_X);
        assert(cpu.mem_read(target) == 0xAA);
        assert(cpu.get_a() == 0xAA);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[STA] ZeroPageX test passed\n";
    }
    // 绕回测试
    {
        CPU cpu;
        cpu.set_a(0x77);
        cpu.set_x(1);
        cpu.mem_write(0x00, 0x00);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0xFF);
        cpu.reset_stats();
        byte expected_status = cpu.get_status();
        OpCode::sta(cpu, ZeroPage_X);
        assert(cpu.mem_read(0x00) == 0x77);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[STA] ZeroPageX wrap test passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_sta_absolute() {
    word addr = 0x1234;
    {
        CPU cpu;
        cpu.set_a(0x42);
        cpu.mem_write(addr, 0x00);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        byte expected_status = cpu.get_status();
        OpCode::sta(cpu, Absolute);
        assert(cpu.mem_read(addr) == 0x42);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[STA] Absolute test passed\n";
    }
}

// ---------- 绝对 X 模式 ----------
static void test_sta_absolute_x() {
    word base = 0x1200;
    byte x_val = 0x34;
    word target = base + x_val;
    {
        CPU cpu;
        cpu.set_a(0x99);
        cpu.set_x(x_val);
        cpu.mem_write(target, 0x00);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base);
        cpu.reset_stats();
        byte expected_status = cpu.get_status();
        OpCode::sta(cpu, Absolute_X);
        assert(cpu.mem_read(target) == 0x99);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[STA] AbsoluteX test passed\n";
    }
    // 跨页测试
    {
        CPU cpu;
        word base2 = 0x12FF;
        byte x_val2 = 2;
        word target2 = base2 + x_val2; // 0x1301
        cpu.set_a(0x88);
        cpu.set_x(x_val2);
        cpu.mem_write(target2, 0x00);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base2);
        cpu.reset_stats();
        cpu.set_pc(0x3000); // 重置PC
        byte expected_status = cpu.get_status();
        OpCode::sta(cpu, Absolute_X);
        assert(cpu.mem_read(target2) == 0x88);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[STA] AbsoluteX page cross test passed\n";
    }
}

// ---------- 绝对 Y 模式 ----------
static void test_sta_absolute_y() {
    word base = 0x1200;
    byte y_val = 0x34;
    word target = base + y_val;
    {
        CPU cpu;
        cpu.set_a(0x66);
        cpu.set_y(y_val);
        cpu.mem_write(target, 0x00);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base);
        cpu.reset_stats();
        byte expected_status = cpu.get_status();
        OpCode::sta(cpu, Absolute_Y);
        assert(cpu.mem_read(target) == 0x66);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[STA] AbsoluteY test passed\n";
    }
}

// ---------- (间接,X) 模式 ----------
static void test_sta_indirect_x() {
    byte zp_base = 0x10;
    byte x_val = 5;
    word pointer = (zp_base + x_val) & 0xFF;
    word target = 0x4321;
    CPU cpu;
    cpu.mem_write(pointer, target & 0xFF);
    cpu.mem_write(pointer + 1, target >> 8);
    cpu.mem_write(target, 0x00);
    cpu.set_a(0xBC);
    cpu.set_x(x_val);
    cpu.set_pc(0x1000);
    cpu.mem_write(cpu.get_pc(), zp_base);
    cpu.reset_stats();
    byte expected_status = cpu.get_status();
    OpCode::sta(cpu, Indirect_X);
    assert(cpu.mem_read(target) == 0xBC);
    check_flags_unchanged(cpu, expected_status);
    std::cout << "[STA] IndirectX test passed\n";
}

// ---------- (间接),Y 模式 ----------
static void test_sta_indirect_y() {
    byte zp_base = 0x10;
    word base_addr = 0x1200;
    byte y_val = 0x34;
    word target = base_addr + y_val;
    CPU cpu;
    cpu.mem_write(zp_base, base_addr & 0xFF);
    cpu.mem_write(zp_base + 1, base_addr >> 8);
    cpu.mem_write(target, 0x00);
    cpu.set_a(0xDE);
    cpu.set_y(y_val);
    cpu.set_pc(0x1000);
    cpu.mem_write(cpu.get_pc(), zp_base);
    cpu.reset_stats();
    byte expected_status = cpu.get_status();
    OpCode::sta(cpu, Indirect_D_Y);
    assert(cpu.mem_read(target) == 0xDE);
    check_flags_unchanged(cpu, expected_status);
    std::cout << "[STA] IndirectY test passed\n";
}

// ---------- 外部调用入口 ----------
void test_sta() {
    std::cout << "[STA] Running tests...\n";
    test_sta_zero_page();
    test_sta_zero_page_x();
    test_sta_absolute();
    test_sta_absolute_x();
    test_sta_absolute_y();
    test_sta_indirect_x();
    test_sta_indirect_y();
    std::cout << "[STA] All tests passed.\n\n";
}