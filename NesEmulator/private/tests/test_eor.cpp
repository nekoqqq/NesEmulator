// test_eor.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool zero, bool negative) {
    byte status = cpu.get_status();
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 立即数模式 ----------
static void test_eor_immediate() {
    // 测试1：0x55 ^ 0xAA = 0xFF, N=1, Z=0
    {
        CPU cpu;
        cpu.set_a(0x55);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0xAA);
        cpu.reset_stats();
        OpCode::op_eor(cpu, Immediate);
        assert(cpu.get_a() == 0xFF);
        check_flags(cpu, false, true);
        std::cout << "[EOR] Immediate test 1 passed\n";
    }
    // 测试2：0xFF ^ 0xFF = 0x00, Z=1, N=0
    {
        CPU cpu;
        cpu.set_a(0xFF);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0xFF);
        cpu.reset_stats();
        OpCode::op_eor(cpu, Immediate);
        assert(cpu.get_a() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[EOR] Immediate test 2 passed\n";
    }
    // 测试3：0x12 ^ 0x34 = 0x26, Z=0, N=0
    {
        CPU cpu;
        cpu.set_a(0x12);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0x34);
        cpu.reset_stats();
        OpCode::op_eor(cpu, Immediate);
        assert(cpu.get_a() == 0x26);
        check_flags(cpu, false, false);
        std::cout << "[EOR] Immediate test 3 passed\n";
    }
}

// ---------- 零页模式 ----------
static void test_eor_zero_page() {
    word addr = 0x10;
    {
        CPU cpu;
        cpu.set_a(0x55);
        cpu.mem_write(addr, 0xAA);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        OpCode::op_eor(cpu, ZeroPage);
        assert(cpu.get_a() == 0xFF);
        check_flags(cpu, false, true);
        std::cout << "[EOR] ZeroPage test 1 passed\n";
    }
}

// ---------- 零页 X 模式 ----------
static void test_eor_zero_page_x() {
    byte base = 0x10;
    byte x_val = 5;
    word target = (base + x_val) & 0xFF; // 0x15
    {
        CPU cpu;
        cpu.set_a(0xAA);
        cpu.set_x(x_val);
        cpu.mem_write(target, 0xFF);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), base);
        cpu.reset_stats();
        OpCode::op_eor(cpu, ZeroPage_X);
        assert(cpu.get_a() == 0x55); // AA ^ FF = 55
        check_flags(cpu, false, false);
        std::cout << "[EOR] ZeroPageX test 1 passed\n";
    }
    // 绕回测试
    {
        CPU cpu;
        cpu.set_a(0x01);
        cpu.set_x(1);
        cpu.mem_write(0x00, 0x01);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0xFF);
        cpu.reset_stats();
        OpCode::op_eor(cpu, ZeroPage_X);
        assert(cpu.get_a() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[EOR] ZeroPageX test 2 (wrap) passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_eor_absolute() {
    word addr = 0x1234;
    {
        CPU cpu;
        cpu.set_a(0x33);
        cpu.mem_write(addr, 0xCC);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        OpCode::op_eor(cpu, Absolute);
        assert(cpu.get_a() == 0xFF);
        check_flags(cpu, false, true);
        std::cout << "[EOR] Absolute test 1 passed\n";
    }
}

// ---------- 绝对 X 模式 ----------
static void test_eor_absolute_x() {
    word base = 0x1200;
    byte x_val = 0x34;
    word target = base + x_val;
    {
        CPU cpu;
        cpu.set_a(0x00);
        cpu.set_x(x_val);
        cpu.mem_write(target, 0x55);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base);
        cpu.reset_stats();
        OpCode::op_eor(cpu, Absolute_X);
        assert(cpu.get_a() == 0x55);
        check_flags(cpu, false, false);
        std::cout << "[EOR] AbsoluteX test 1 passed\n";
    }
}

// ---------- 绝对 Y 模式 ----------
static void test_eor_absolute_y() {
    word base = 0x1200;
    byte y_val = 0x34;
    word target = base + y_val;
    {
        CPU cpu;
        cpu.set_a(0xFF);
        cpu.set_y(y_val);
        cpu.mem_write(target, 0x55);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base);
        cpu.reset_stats();
        OpCode::op_eor(cpu, Absolute_Y);
        assert(cpu.get_a() == 0xAA);
        check_flags(cpu, false, true);
        std::cout << "[EOR] AbsoluteY test 1 passed\n";
    }
}

// ---------- (间接,X) 模式 ----------
static void test_eor_indirect_x() {
    byte zp_base = 0x10;
    byte x_val = 5;
    word pointer = (zp_base + x_val) & 0xFF;
    word target = 0x4321;
    CPU cpu;
    cpu.mem_write(pointer, target & 0xFF);
    cpu.mem_write(pointer + 1, target >> 8);
    cpu.mem_write(target, 0x0F);
    cpu.set_a(0xF0);
    cpu.set_x(x_val);
    cpu.set_pc(0x1000);
    cpu.mem_write(cpu.get_pc(), zp_base);
    cpu.reset_stats();
    OpCode::op_eor(cpu, Indirect_X);
    assert(cpu.get_a() == 0xFF);
    check_flags(cpu, false, true);
    std::cout << "[EOR] IndirectX test 1 passed\n";
}

// ---------- (间接),Y 模式 ----------
static void test_eor_indirect_y() {
    byte zp_base = 0x10;
    word base_addr = 0x1200;
    byte y_val = 0x34;
    word target = base_addr + y_val;
    CPU cpu;
    cpu.mem_write(zp_base, base_addr & 0xFF);
    cpu.mem_write(zp_base + 1, base_addr >> 8);
    cpu.mem_write(target, 0xAA);
    cpu.set_a(0x55);
    cpu.set_y(y_val);
    cpu.set_pc(0x1000);
    cpu.mem_write(cpu.get_pc(), zp_base);
    cpu.reset_stats();
    OpCode::op_eor(cpu, Indirect_D_Y);
    assert(cpu.get_a() == 0xFF);
    check_flags(cpu, false, true);
    std::cout << "[EOR] IndirectY test 1 passed\n";
}

// ---------- 外部调用入口 ----------
void test_eor() {
    std::cout << "[EOR] Running tests...\n";
    test_eor_immediate();
    test_eor_zero_page();
    test_eor_zero_page_x();
    test_eor_absolute();
    test_eor_absolute_x();
    test_eor_absolute_y();
    test_eor_indirect_x();
    test_eor_indirect_y();
    std::cout << "[EOR] All tests passed.\n\n";
}