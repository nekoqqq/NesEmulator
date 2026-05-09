// test_lsr.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool carry, bool zero, bool negative) {
    byte status = cpu.get_status();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 累加器模式 ----------
static void test_lsr_accumulator() {
    // 测试1：0x02 -> 0x01, C=0, Z=0, N=0
    {
        CPU cpu;
        cpu.set_a(0x02);
        cpu.reset_stats();
        OpCode::lsr(cpu, Accumulator);
        assert(cpu.get_a() == 0x01);
        check_flags(cpu, false, false, false);
        std::cout << "[LSR] Accumulator test 1 passed\n";
    }
    // 测试2：0x01 -> 0x00, C=1, Z=1, N=0
    {
        CPU cpu;
        cpu.set_a(0x01);
        cpu.reset_stats();
        OpCode::lsr(cpu, Accumulator);
        assert(cpu.get_a() == 0x00);
        check_flags(cpu, true, true, false);
        std::cout << "[LSR] Accumulator test 2 passed\n";
    }
    // 测试3：0x80 -> 0x40, C=0, Z=0, N=0
    {
        CPU cpu;
        cpu.set_a(0x80);
        cpu.reset_stats();
        OpCode::lsr(cpu, Accumulator);
        assert(cpu.get_a() == 0x40);
        check_flags(cpu, false, false, false);
        std::cout << "[LSR] Accumulator test 3 passed\n";
    }
    // 测试4：0xFF -> 0x7F, C=1, Z=0, N=0
    {
        CPU cpu;
        cpu.set_a(0xFF);
        cpu.reset_stats();
        OpCode::lsr(cpu, Accumulator);
        assert(cpu.get_a() == 0x7F);
        check_flags(cpu, true, false, false);
        std::cout << "[LSR] Accumulator test 4 passed\n";
    }
}

// ---------- 零页模式 ----------
static void test_lsr_zero_page() {
    word addr = 0x10;
    // 测试1：0x04 -> 0x02
    {
        CPU cpu;
        cpu.mem_write(addr, 0x04);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        OpCode::lsr(cpu, ZeroPage);
        assert(cpu.mem_read(addr) == 0x02);
        check_flags(cpu, false, false, false);
        std::cout << "[LSR] ZeroPage test 1 passed\n";
    }
    // 测试2：0x01 -> 0x00, C=1, Z=1
    {
        CPU cpu;
        cpu.mem_write(addr, 0x01);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        OpCode::lsr(cpu, ZeroPage);
        assert(cpu.mem_read(addr) == 0x00);
        check_flags(cpu, true, true, false);
        std::cout << "[LSR] ZeroPage test 2 passed\n";
    }
}

// ---------- 零页 X 模式 ----------
static void test_lsr_zero_page_x() {
    byte base = 0x10;
    byte x_val = 5;
    word target = (base + x_val) & 0xFF; // 0x15
    {
        CPU cpu;
        cpu.set_x(x_val);
        cpu.mem_write(target, 0x03);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), base);
        cpu.reset_stats();
        OpCode::lsr(cpu, ZeroPage_X);
        assert(cpu.mem_read(target) == 0x01);
        check_flags(cpu, true, false, false);
        std::cout << "[LSR] ZeroPageX test 1 passed\n";
    }
    // 绕回测试
    {
        CPU cpu;
        cpu.set_x(1);
        cpu.mem_write(0x00, 0x02);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0xFF);
        cpu.reset_stats();
        OpCode::lsr(cpu, ZeroPage_X);
        assert(cpu.mem_read(0x00) == 0x01);
        check_flags(cpu, false, false, false);
        std::cout << "[LSR] ZeroPageX test 2 (wrap) passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_lsr_absolute() {
    word addr = 0x1234;
    {
        CPU cpu;
        cpu.mem_write(addr, 0x02);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        OpCode::lsr(cpu, Absolute);
        assert(cpu.mem_read(addr) == 0x01);
        check_flags(cpu, false, false, false);
        std::cout << "[LSR] Absolute test 1 passed\n";
    }
}

// ---------- 绝对 X 模式 ----------
static void test_lsr_absolute_x() {
    word base = 0x1200;
    byte x_val = 0x34;
    word target = base + x_val; // 0x1234
    {
        CPU cpu;
        cpu.set_x(x_val);
        cpu.mem_write(target, 0x80);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base);
        cpu.reset_stats();
        OpCode::lsr(cpu, Absolute_X);
        assert(cpu.mem_read(target) == 0x40);
        check_flags(cpu, false, false, false);
        std::cout << "[LSR] AbsoluteX test 1 passed\n";
    }
    // 跨页测试
    {
        CPU cpu;
        word base2 = 0x12FF;
        byte x_val2 = 2;
        word target2 = base2 + x_val2; // 0x1301
        cpu.set_x(x_val2);
        cpu.mem_write(target2, 0xFF);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base2);
        cpu.reset_stats();
        OpCode::lsr(cpu, Absolute_X);
        assert(cpu.mem_read(target2) == 0x7F);
        check_flags(cpu, true, false, false);
        std::cout << "[LSR] AbsoluteX test 2 (page cross) passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_lsr() {
    std::cout << "[LSR] Running tests...\n";
    test_lsr_accumulator();
    test_lsr_zero_page();
    test_lsr_zero_page_x();
    test_lsr_absolute();
    test_lsr_absolute_x();
    std::cout << "[LSR] All tests passed.\n\n";
}