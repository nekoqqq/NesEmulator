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
static void test_ror_accumulator() {
    // 测试1：无进位右移，0x24 -> 0x12, C=0, Z=0, N=0
    {
        CPU cpu;
        cpu.set_a(0x24);
        cpu.set_flag(CPU::CARRY_FLAG, false);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::ror(cpu, Accumulator);
        assert(cpu.get_a() == 0x12);
        check_flags(cpu, false, false, false);
        std::cout << "[ROR] Accumulator test 1 passed\n";
    }
    // 测试2：有进位右移，0x24 + C=1 -> 0x92
    {
        CPU cpu;
        cpu.set_a(0x24);
        cpu.set_flag(CPU::CARRY_FLAG, true);
        OpCode::ror(cpu, Accumulator);
        assert(cpu.get_a() == 0x92);
        check_flags(cpu, false, false, true);
        std::cout << "[ROR] Accumulator test 2 passed\n";
    }
    // 测试3：0x01 右移，C=0 -> 0x00, C=1, Z=1
    {
        CPU cpu;
        cpu.set_a(0x01);
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::ror(cpu, Accumulator);
        assert(cpu.get_a() == 0x00);
        check_flags(cpu, true, true, false);
        std::cout << "[ROR] Accumulator test 3 passed\n";
    }
    // 测试4：0x01 右移，C=1 -> 0x80, C=1, Z=0, N=1
    {
        CPU cpu;
        cpu.set_a(0x01);
        cpu.set_flag(CPU::CARRY_FLAG, true);
        OpCode::ror(cpu, Accumulator);
        assert(cpu.get_a() == 0x80);
        check_flags(cpu, true, false, true);
        std::cout << "[ROR] Accumulator test 4 passed\n";
    }
    // 测试5：0x80 右移，C=0 -> 0x40, C=0, N=0
    {
        CPU cpu;
        cpu.set_a(0x80);
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::ror(cpu, Accumulator);
        assert(cpu.get_a() == 0x40);
        check_flags(cpu, false, false, false);
        std::cout << "[ROR] Accumulator test 5 passed\n";
    }
    // 测试6：0x80 右移，C=1 -> 0xC0, C=0, N=1
    {
        CPU cpu;
        cpu.set_a(0x80);
        cpu.set_flag(CPU::CARRY_FLAG, true);
        OpCode::ror(cpu, Accumulator);
        assert(cpu.get_a() == 0xC0);
        check_flags(cpu, false, false, true);
        std::cout << "[ROR] Accumulator test 6 passed\n";
    }
}

// ---------- 零页模式 ----------
static void test_ror_zero_page() {
    word addr = 0x10;
    // 无进位右移
    {
        CPU cpu;
        cpu.mem_write(addr, 0x24);
        cpu.set_flag(CPU::CARRY_FLAG, false);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::ror(cpu, ZeroPage);
        assert(cpu.mem_read(addr) == 0x12);
        check_flags(cpu, false, false, false);
        std::cout << "[ROR] ZeroPage test 1 passed\n";
    }
    // 有进位右移
    {
        CPU cpu;
        cpu.mem_write(addr, 0x01);
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        OpCode::ror(cpu, ZeroPage);
        assert(cpu.mem_read(addr) == 0x80);
        check_flags(cpu, true, false, true);
        std::cout << "[ROR] ZeroPage test 2 passed\n";
    }
}

// ---------- 零页 X 模式 ----------
static void test_ror_zero_page_x() {
    byte base = 0x10;
    byte x_val = 5;
    word target = (base + x_val) & 0xFF; // 0x15
    {
        CPU cpu;
        cpu.set_x(x_val);
        cpu.mem_write(target, 0x01);
        cpu.set_flag(CPU::CARRY_FLAG, false);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), base);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::ror(cpu, ZeroPage_X);
        assert(cpu.mem_read(target) == 0x00);
        check_flags(cpu, true, true, false);
        std::cout << "[ROR] ZeroPageX test 1 passed\n";
    }
    // 绕回测试
    {
        CPU cpu;
        cpu.set_x(1);
        cpu.mem_write(0x00, 0x80);
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0xFF);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        OpCode::ror(cpu, ZeroPage_X);
        assert(cpu.mem_read(0x00) == 0xC0);
        check_flags(cpu, false, false, true);
        std::cout << "[ROR] ZeroPageX test 2 (wrap) passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_ror_absolute() {
    word addr = 0x1234;
    {
        CPU cpu;
        cpu.mem_write(addr, 0x02);
        cpu.set_flag(CPU::CARRY_FLAG, false);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::ror(cpu, Absolute);
        assert(cpu.mem_read(addr) == 0x01);
        check_flags(cpu, false, false, false);
        std::cout << "[ROR] Absolute test 1 passed\n";
    }
    {
        CPU cpu;
        cpu.mem_write(addr, 0x01);
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        OpCode::ror(cpu, Absolute);
        assert(cpu.mem_read(addr) == 0x80);
        check_flags(cpu, true, false, true);
        std::cout << "[ROR] Absolute test 2 passed\n";
    }
}

// ---------- 绝对 X 模式 ----------
static void test_ror_absolute_x() {
    word base = 0x1200;
    byte x_val = 0x34;
    word target = base + x_val; // 0x1234
    {
        CPU cpu;
        cpu.set_x(x_val);
        cpu.mem_write(target, 0x01);
        cpu.set_flag(CPU::CARRY_FLAG, false);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::ror(cpu, Absolute_X);
        assert(cpu.mem_read(target) == 0x00);
        check_flags(cpu, true, true, false);
        std::cout << "[ROR] AbsoluteX test 1 passed\n";
    }
    // 跨页
    {
        CPU cpu;
        word base2 = 0x12FF;
        byte x_val2 = 2;
        word target2 = base2 + x_val2; // 0x1301
        cpu.set_x(x_val2);
        cpu.mem_write(target2, 0x80);
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base2);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        OpCode::ror(cpu, Absolute_X);
        assert(cpu.mem_read(target2) == 0xC0);
        check_flags(cpu, false, false, true);
        std::cout << "[ROR] AbsoluteX test 2 (page cross) passed\n";
    }
}

void test_ror() {
    std::cout << "[ROR] Running tests...\n";
    test_ror_accumulator();
    test_ror_zero_page();
    test_ror_zero_page_x();
    test_ror_absolute();
    test_ror_absolute_x();
    std::cout << "[ROR] All tests passed.\n\n";
}