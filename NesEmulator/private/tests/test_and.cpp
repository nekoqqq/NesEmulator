// test_and.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 检查 C, Z, N 标志（C 仅用于验证未被改变）
static void check_flags(const CPU& cpu, bool carry, bool zero, bool negative) {
    byte status = cpu.get_status();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 立即数模式 ----------
static void test_and_immediate() {
    // 测试1：普通与运算，结果非零非负
    {
        CPU cpu;
        cpu.set_a(0b11001100);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0b10101010);
        OpCode::op_and(cpu, Immediate);
        assert(cpu.get_a() == 0b10001000);
        check_flags(cpu, false, false, true);
        std::cout << "[AND] Immediate test 1 passed\n";
    }

    // 测试2：结果为0，Z=1
    {
        CPU cpu;
        cpu.set_a(0x55);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0xAA);
        OpCode::op_and(cpu, Immediate);
        assert(cpu.get_a() == 0x00);
        check_flags(cpu, false, true, false);
        std::cout << "[AND] Immediate test 2 passed\n";
    }

    // 测试3：结果负，N=1
    {
        CPU cpu;
        cpu.set_a(0xFF);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0x80);
        OpCode::op_and(cpu, Immediate);
        assert(cpu.get_a() == 0x80);
        check_flags(cpu, false, false, true);
        std::cout << "[AND] Immediate test 3 passed\n";
    }
}

// ---------- 零页模式 ----------
static void test_and_zero_page() {
    word addr = 0x10;

    {
        CPU cpu;
        cpu.set_a(0b11001100);
        cpu.mem_write(addr, 0b10101010);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::op_and(cpu, ZeroPage);
        assert(cpu.get_a() == 0b10001000);
        check_flags(cpu, false, false, true);
        std::cout << "[AND] ZeroPage test 1 passed\n";
    }

    {
        CPU cpu;
        cpu.set_a(0x55);
        cpu.mem_write(addr, 0xAA);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::op_and(cpu, ZeroPage);
        assert(cpu.get_a() == 0x00);
        check_flags(cpu, false, true, false);
        std::cout << "[AND] ZeroPage test 2 passed\n";
    }
}

// ---------- 零页 X 模式 ----------
static void test_and_zero_page_x() {
    {
        CPU cpu;
        cpu.set_a(0b11001100);
        cpu.set_x(5);
        word target = 0x15;
        cpu.mem_write(target, 0b10101010);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0x10);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::op_and(cpu, ZeroPage_X);
        assert(cpu.get_a() == 0b10001000);
        check_flags(cpu, false, false, true);
        std::cout << "[AND] ZeroPageX test 1 passed\n";
    }

    {
        CPU cpu;
        cpu.set_a(0x55);
        cpu.set_x(1);
        cpu.mem_write(0x00, 0xAA);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0xFF);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::op_and(cpu, ZeroPage_X);
        assert(cpu.get_a() == 0x00);
        check_flags(cpu, false, true, false);
        std::cout << "[AND] ZeroPageX test 2 passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_and_absolute() {
    word addr = 0x1234;

    {
        CPU cpu;
        cpu.set_a(0b11001100);
        cpu.mem_write(addr, 0b10101010);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::op_and(cpu, Absolute);
        assert(cpu.get_a() == 0b10001000);
        check_flags(cpu, false, false, true);
        std::cout << "[AND] Absolute test 1 passed\n";
    }

    {
        CPU cpu;
        cpu.set_a(0x55);
        cpu.mem_write(addr, 0xAA);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::op_and(cpu, Absolute);
        assert(cpu.get_a() == 0x00);
        check_flags(cpu, false, true, false);
        std::cout << "[AND] Absolute test 2 passed\n";
    }
}

// ---------- 绝对 X 模式 ----------
static void test_and_absolute_x() {
    word base = 0x1200;
    byte x_val = 0x34;
    word target = base + x_val;

    {
        CPU cpu;
        cpu.set_a(0b11001100);
        cpu.set_x(x_val);
        cpu.mem_write(target, 0b10101010);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::op_and(cpu, Absolute_X);
        assert(cpu.get_a() == 0b10001000);
        check_flags(cpu, false, false, true);
        std::cout << "[AND] AbsoluteX test 1 passed\n";
    }
}

// ---------- 绝对 Y 模式 ----------
static void test_and_absolute_y() {
    word base = 0x1200;
    byte y_val = 0x34;
    word target = base + y_val;

    {
        CPU cpu;
        cpu.set_a(0b11001100);
        cpu.set_y(y_val);
        cpu.mem_write(target, 0b10101010);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::op_and(cpu, Absolute_Y);
        assert(cpu.get_a() == 0b10001000);
        check_flags(cpu, false, false, true);
        std::cout << "[AND] AbsoluteY test 1 passed\n";
    }
}

// ---------- (间接,X) 模式 ----------
static void test_and_indirect_x() {
    byte zp_base = 0x10;
    byte x_val = 5;
    word pointer = (zp_base + x_val) & 0xFF;   // 0x15
    word target = 0x4321;
    CPU cpu;
    cpu.mem_write(pointer, target & 0xFF);
    cpu.mem_write(pointer + 1, target >> 8);
    cpu.mem_write(target, 0b10101010);
    cpu.set_a(0b11001100);
    cpu.set_x(x_val);
    cpu.set_pc(0x1000);
    cpu.mem_write(cpu.get_pc(), zp_base);
    cpu.reset_stats();
    cpu.set_flag(CPU::CARRY_FLAG, false);
    OpCode::op_and(cpu, Indirect_X);
    assert(cpu.get_a() == 0b10001000);
    check_flags(cpu, false, false, true);
    std::cout << "[AND] IndirectX test 1 passed\n";
}

// ---------- (间接),Y 模式 ----------
static void test_and_indirect_y() {
    byte zp_base = 0x10;
    word base_addr = 0x1200;
    byte y_val = 0x34;
    word target = base_addr + y_val;
    CPU cpu;
    cpu.mem_write(zp_base, base_addr & 0xFF);
    cpu.mem_write(zp_base + 1, base_addr >> 8);
    cpu.mem_write(target, 0b10101010);
    cpu.set_a(0b11001100);
    cpu.set_y(y_val);
    cpu.set_pc(0x1000);
    cpu.mem_write(cpu.get_pc(), zp_base);
    cpu.reset_stats();
    cpu.set_flag(CPU::CARRY_FLAG, false);
    OpCode::op_and(cpu, Indirect_D_Y);
    assert(cpu.get_a() == 0b10001000);
    check_flags(cpu, false, false, true);
    std::cout << "[AND] IndirectY test 1 passed\n";
}

// ---------- 外部调用入口 ----------
void test_and() {
    std::cout << "[AND] Running tests...\n";
    test_and_immediate();
    test_and_zero_page();
    test_and_zero_page_x();
    test_and_absolute();
    test_and_absolute_x();
    test_and_absolute_y();
    test_and_indirect_x();
    test_and_indirect_y();
    std::cout << "[AND] All tests passed.\n\n";
}