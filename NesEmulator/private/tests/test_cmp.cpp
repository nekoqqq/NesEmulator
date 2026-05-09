#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 辅助：检查 C, Z, N 标志（CMP 不影响 I, D, V, B）
static void check_flags(const CPU& cpu, bool carry, bool zero, bool negative) {
    byte status = cpu.get_status();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 立即数模式 ----------
static void test_cmp_immediate() {
    // 测试1：A == M -> Z=1, C=1, N=0
    {
        CPU cpu;
        cpu.set_a(0x42);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0x42);
        cpu.reset_stats();
        OpCode::cmp(cpu, Immediate);
        assert(cpu.get_a() == 0x42);
        check_flags(cpu, true, true, false);
        std::cout << "[CMP] Immediate test 1 (equal) passed\n";
    }
    // 测试2：A > M -> Z=0, C=1, N=0
    {
        CPU cpu;
        cpu.set_a(0x50);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0x30);
        cpu.reset_stats();
        OpCode::cmp(cpu, Immediate);
        assert(cpu.get_a() == 0x50);
        check_flags(cpu, true, false, false);
        std::cout << "[CMP] Immediate test 2 (A > M) passed\n";
    }
    // 测试3：A < M -> Z=0, C=0, N=1 (结果负)
    {
        CPU cpu;
        cpu.set_a(0x20);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0x80);
        cpu.reset_stats();
        OpCode::cmp(cpu, Immediate);
        assert(cpu.get_a() == 0x20);
        check_flags(cpu, false, false, true);
        std::cout << "[CMP] Immediate test 3 (A < M) passed\n";
    }
    // 测试4：边界值 A=0x80, M=0x80 -> 相等
    {
        CPU cpu;
        cpu.set_a(0x80);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0x80);
        cpu.reset_stats();
        OpCode::cmp(cpu, Immediate);
        check_flags(cpu, true, true, false);
        std::cout << "[CMP] Immediate test 4 (boundary equal) passed\n";
    }
}

// ---------- 零页模式 ----------
static void test_cmp_zero_page() {
    word addr = 0x10;
    {
        CPU cpu;
        cpu.set_a(0x10);
        cpu.mem_write(addr, 0x20);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        OpCode::cmp(cpu, ZeroPage);
        check_flags(cpu, false, false, true);
        std::cout << "[CMP] ZeroPage test 1 passed\n";
    }
    {
        CPU cpu;
        cpu.set_a(0x30);
        cpu.mem_write(addr, 0x30);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        OpCode::cmp(cpu, ZeroPage);
        check_flags(cpu, true, true, false);
        std::cout << "[CMP] ZeroPage test 2 passed\n";
    }
}

// ---------- 零页 X 模式 ----------
static void test_cmp_zero_page_x() {
    byte base = 0x10;
    byte x_val = 5;
    word target = (base + x_val) & 0xFF; // 0x15
    {
        CPU cpu;
        cpu.set_a(0x40);
        cpu.set_x(x_val);
        cpu.mem_write(target, 0x30);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), base);
        cpu.reset_stats();
        OpCode::cmp(cpu, ZeroPage_X);
        check_flags(cpu, true, false, false); // 40 >= 30 -> C=1, Z=0, N=0
        std::cout << "[CMP] ZeroPageX test 1 passed\n";
    }
    // 绕回测试
    {
        CPU cpu;
        cpu.set_a(0x01);
        cpu.set_x(1);
        cpu.mem_write(0x00, 0x02);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0xFF);
        cpu.reset_stats();
        OpCode::cmp(cpu, ZeroPage_X);
        check_flags(cpu, false, false, true); // 01 < 02 -> C=0, Z=0, N=1
        std::cout << "[CMP] ZeroPageX test 2 (wrap) passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_cmp_absolute() {
    word addr = 0x1234;
    {
        CPU cpu;
        cpu.set_a(0x55);
        cpu.mem_write(addr, 0x55);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        OpCode::cmp(cpu, Absolute);
        check_flags(cpu, true, true, false);
        std::cout << "[CMP] Absolute test 1 passed\n";
    }
    {
        CPU cpu;
        cpu.set_a(0x10);
        cpu.mem_write(addr, 0x20);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        OpCode::cmp(cpu, Absolute);
        check_flags(cpu, false, false, true);
        std::cout << "[CMP] Absolute test 2 passed\n";
    }
}

// ---------- 绝对 X 模式 ----------
static void test_cmp_absolute_x() {
    word base = 0x1200;
    byte x_val = 0x34;
    word target = base + x_val; // 0x1234
    {
        CPU cpu;
        cpu.set_a(0x7F);
        cpu.set_x(x_val);
        cpu.mem_write(target, 0x7F);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base);
        cpu.reset_stats();
        OpCode::cmp(cpu, Absolute_X);
        check_flags(cpu, true, true, false);
        std::cout << "[CMP] AbsoluteX test 1 passed\n";
    }
    {
        CPU cpu;
        cpu.set_a(0x00);
        cpu.set_x(x_val);
        cpu.mem_write(target, 0x01);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base);
        cpu.reset_stats();
        OpCode::cmp(cpu, Absolute_X);
        check_flags(cpu, false, false, true);
        std::cout << "[CMP] AbsoluteX test 2 passed\n";
    }
}

// ---------- 绝对 Y 模式 ----------
static void test_cmp_absolute_y() {
    word base = 0x1200;
    byte y_val = 0x34;
    word target = base + y_val;
    {
        CPU cpu;
        cpu.set_a(0x01);
        cpu.set_y(y_val);
        cpu.mem_write(target, 0x01);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base);
        cpu.reset_stats();
        OpCode::cmp(cpu, Absolute_Y);
        check_flags(cpu, true, true, false);
        std::cout << "[CMP] AbsoluteY test 1 passed\n";
    }
}

// ---------- (间接,X) 模式 ----------
static void test_cmp_indirect_x() {
    byte zp_base = 0x10;
    byte x_val = 5;
    word pointer = (zp_base + x_val) & 0xFF;
    word target = 0x4321;
    CPU cpu;
    cpu.mem_write(pointer, target & 0xFF);
    cpu.mem_write(pointer + 1, target >> 8);
    cpu.mem_write(target, 0x60);
    cpu.set_a(0x60);
    cpu.set_x(x_val);
    cpu.set_pc(0x1000);
    cpu.mem_write(cpu.get_pc(), zp_base);
    cpu.reset_stats();
    OpCode::cmp(cpu, Indirect_X);
    check_flags(cpu, true, true, false);
    std::cout << "[CMP] IndirectX test 1 passed\n";
}

// ---------- (间接),Y 模式 ----------
static void test_cmp_indirect_y() {
    byte zp_base = 0x10;
    word base_addr = 0x1200;
    byte y_val = 0x34;
    word target = base_addr + y_val;
    CPU cpu;
    cpu.mem_write(zp_base, base_addr & 0xFF);
    cpu.mem_write(zp_base + 1, base_addr >> 8);
    cpu.mem_write(target, 0x90);
    cpu.set_a(0x80);
    cpu.set_y(y_val);
    cpu.set_pc(0x1000);
    cpu.mem_write(cpu.get_pc(), zp_base);
    cpu.reset_stats();
    OpCode::cmp(cpu, Indirect_D_Y);
    check_flags(cpu, false, false, true); // 80 < 90 -> C=0, Z=0, N=1
    std::cout << "[CMP] IndirectY test 1 passed\n";
}

// ---------- 外部调用入口 ----------
void test_cmp() {
    std::cout << "[CMP] Running tests...\n";
    test_cmp_immediate();
    test_cmp_zero_page();
    test_cmp_zero_page_x();
    test_cmp_absolute();
    test_cmp_absolute_x();
    test_cmp_absolute_y();
    test_cmp_indirect_x();
    test_cmp_indirect_y();
    std::cout << "[CMP] All tests passed.\n\n";
}