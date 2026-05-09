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
static void test_rol_accumulator() {
    // 测试1：无进位左移，0x12 -> 0x24, C=0, Z=0, N=0
    {
        CPU cpu;
        cpu.set_a(0x12);
        cpu.set_flag(CPU::CARRY_FLAG, false);
        cpu.reset_stats();
        // 注意 ResetStatus 会清除 C，需重新设置
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::rol(cpu, Accumulator);
        assert(cpu.get_a() == 0x24);
        check_flags(cpu, false, false, false);
        std::cout << "[ROL] Accumulator test 1 passed\n";
    }
    // 测试2：有进位左移，0x12 + C=1 -> 0x25
    {
        CPU cpu;
        cpu.set_a(0x12);
        cpu.set_flag(CPU::CARRY_FLAG, true);
        OpCode::rol(cpu, Accumulator);
        assert(cpu.get_a() == 0x25);
        check_flags(cpu, false, false, false);
        std::cout << "[ROL] Accumulator test 2 passed\n";
    }
    // 测试3：0x80 左移，C=0 -> 0x00, C=1, Z=1
    {
        CPU cpu;
        cpu.set_a(0x80);
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::rol(cpu, Accumulator);
        assert(cpu.get_a() == 0x00);
        check_flags(cpu, true, true, false);
        std::cout << "[ROL] Accumulator test 3 passed\n";
    }
    // 测试4：0x40 左移，C=1 -> 0x81, C=0, Z=0, N=1
    {
        CPU cpu;
        cpu.set_a(0x40);
        cpu.set_flag(CPU::CARRY_FLAG, true);
        OpCode::rol(cpu, Accumulator);
        assert(cpu.get_a() == 0x81);
        check_flags(cpu, false, false, true);
        std::cout << "[ROL] Accumulator test 4 passed\n";
    }
    // 测试5：0xFF 左移，C=1 -> 0xFF, C=1, Z=0, N=1 (因为 0xFF<<1=0x1FE 低8位0xFE，加上C=1得0xFF)
    {
        CPU cpu;
        cpu.set_a(0xFF);
        cpu.set_flag(CPU::CARRY_FLAG, true);
        OpCode::rol(cpu, Accumulator);
        assert(cpu.get_a() == 0xFF);
        check_flags(cpu, true, false, true);
        std::cout << "[ROL] Accumulator test 5 passed\n";
    }
}

// ---------- 零页模式 ----------
static void test_rol_zero_page() {
    word addr = 0x10;
    {
        CPU cpu;
        cpu.mem_write(addr, 0x81);
        cpu.set_flag(CPU::CARRY_FLAG, false);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::rol(cpu, ZeroPage);
        assert(cpu.mem_read(addr) == 0x02);
        check_flags(cpu, true, false, false);
        std::cout << "[ROL] ZeroPage test 1 passed\n";
    }
    {
        CPU cpu;
        cpu.mem_write(addr, 0x01);
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), static_cast<byte>(addr));
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        OpCode::rol(cpu, ZeroPage);
        assert(cpu.mem_read(addr) == 0x03);
        check_flags(cpu, false, false, false);
        std::cout << "[ROL] ZeroPage test 2 passed\n";
    }
}

// ---------- 零页 X 模式 ----------
static void test_rol_zero_page_x() {
    byte base = 0x10;
    byte x_val = 5;
    word target = (base + x_val) & 0xFF; // 0x15
    {
        CPU cpu;
        cpu.set_x(x_val);
        cpu.mem_write(target, 0x40);
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), base);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        OpCode::rol(cpu, ZeroPage_X);
        assert(cpu.mem_read(target) == 0x81);
        check_flags(cpu, false, false, true);
        std::cout << "[ROL] ZeroPageX test 1 passed\n";
    }
    // 绕回
    {
        CPU cpu;
        cpu.set_x(1);
        cpu.mem_write(0x00, 0x80);
        cpu.set_flag(CPU::CARRY_FLAG, false);
        cpu.set_pc(0x1000);
        cpu.mem_write(cpu.get_pc(), 0xFF);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::rol(cpu, ZeroPage_X);
        assert(cpu.mem_read(0x00) == 0x00);
        check_flags(cpu, true, true, false);
        std::cout << "[ROL] ZeroPageX test 2 (wrap) passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_rol_absolute() {
    word addr = 0x1234;
    {
        CPU cpu;
        cpu.mem_write(addr, 0x01);
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        OpCode::rol(cpu, Absolute);
        assert(cpu.mem_read(addr) == 0x03);
        check_flags(cpu, false, false, false);
        std::cout << "[ROL] Absolute test 1 passed\n";
    }
    {
        CPU cpu;
        cpu.mem_write(addr, 0xFF);
        cpu.set_flag(CPU::CARRY_FLAG, false);
        cpu.set_pc(0x2000);
        cpu.mem_write_word(cpu.get_pc(), addr);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::rol(cpu, Absolute);
        assert(cpu.mem_read(addr) == 0xFE);
        check_flags(cpu, true, false, true);
        std::cout << "[ROL] Absolute test 2 passed\n";
    }
}

// ---------- 绝对 X 模式 ----------
static void test_rol_absolute_x() {
    word base = 0x1200;
    byte x_val = 0x34;
    word target = base + x_val; // 0x1234
    {
        CPU cpu;
        cpu.set_x(x_val);
        cpu.mem_write(target, 0x7F);
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        OpCode::rol(cpu, Absolute_X);
        assert(cpu.mem_read(target) == 0xFF);
        check_flags(cpu, false, false, true);
        std::cout << "[ROL] AbsoluteX test 1 passed\n";
    }
    // 跨页
    {
        CPU cpu;
        word base2 = 0x12FF;
        byte x_val2 = 2;
        word target2 = base2 + x_val2; // 0x1301
        cpu.set_x(x_val2);
        cpu.mem_write(target2, 0x00);
        cpu.set_flag(CPU::CARRY_FLAG, false);
        cpu.set_pc(0x3000);
        cpu.mem_write_word(cpu.get_pc(), base2);
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        OpCode::rol(cpu, Absolute_X);
        assert(cpu.mem_read(target2) == 0x00);
        check_flags(cpu, false, true, false);
        std::cout << "[ROL] AbsoluteX test 2 (page cross) passed\n";
    }
}

void test_rol() {
    std::cout << "[ROL] Running tests...\n";
    test_rol_accumulator();
    test_rol_zero_page();
    test_rol_zero_page_x();
    test_rol_absolute();
    test_rol_absolute_x();
    std::cout << "[ROL] All tests passed.\n\n";
}