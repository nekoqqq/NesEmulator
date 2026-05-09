// test_bvc.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 辅助：检查 C、Z、N 标志（BVC 不影响标志）
static void check_flags(const CPU& cpu, bool carry, bool zero, bool negative) {
    byte status = cpu.get_status();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 相对寻址模式（BVC 唯一寻址方式） ----------
static void test_bvc_branch_taken() {
    // 测试1：向前跳转，V=0，偏移 +10
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::OVERFLOW_FLAG, false);   // V=0 满足跳转条件
        cpu.set_pc(0x2000);
        cpu.mem_write(cpu.get_pc(), 10);
        word old_pc = cpu.get_pc();
        OpCode::bvc(cpu, Relative);
        word expected_pc = old_pc + 1 + 10;
        assert(cpu.get_pc() == expected_pc);
        check_flags(cpu, false, false, false);    // V 不影响 C,Z,N，此处只验证它们为 0
        std::cout << "[BVC] Branch forward test passed\n";
    }

    // 测试2：向后跳转，V=0，偏移 -5（补码 0xFB）
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::OVERFLOW_FLAG, false);
        cpu.set_pc(0x2000);
        cpu.mem_write(cpu.get_pc(), 0xFB);
        word old_pc = cpu.get_pc();
        OpCode::bvc(cpu, Relative);
        word expected_pc = old_pc + 1 + (signed char)0xFB;
        assert(cpu.get_pc() == expected_pc);
        check_flags(cpu, false, false, false);
        std::cout << "[BVC] Branch backward test passed\n";
    }

    // 测试3：边界值 +127
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::OVERFLOW_FLAG, false);
        cpu.set_pc(0x2100);
        cpu.mem_write(cpu.get_pc(), 127);
        word old_pc = cpu.get_pc();
        OpCode::bvc(cpu, Relative);
        word expected_pc = old_pc + 1 + 127;
        assert(cpu.get_pc() == expected_pc);
        std::cout << "[BVC] Branch max forward (+127) passed\n";
    }

    // 测试4：边界值 -128
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::OVERFLOW_FLAG, false);
        cpu.set_pc(0x2200);
        cpu.mem_write(cpu.get_pc(), 0x80);
        word old_pc = cpu.get_pc();
        OpCode::bvc(cpu, Relative);
        word expected_pc = old_pc + 1 + (signed char)0x80;
        assert(cpu.get_pc() == expected_pc);
        std::cout << "[BVC] Branch max backward (-128) passed\n";
    }
}

static void test_bvc_branch_not_taken() {
    // 测试5：V=1 时不跳转
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::OVERFLOW_FLAG, true);    // V=1 不满足跳转
        cpu.set_pc(0x3000);
        cpu.mem_write(cpu.get_pc(), 50);
        word old_pc = cpu.get_pc();
        OpCode::bvc(cpu, Relative);
        word expected_pc = old_pc + 1;            // 仅跳过偏移字节
        assert(cpu.get_pc() == expected_pc);
        check_flags(cpu, false, false, false);    // V 仍为 1，但 check_flags 不检查 V，所以忽略
        std::cout << "[BVC] No branch when V=1 passed\n";
    }
}

static void test_bvc_page_cross() {
    // 跨页测试：PC=0x20FF，偏移 +2 -> 目标 0x2102
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::OVERFLOW_FLAG, false);
        cpu.set_pc(0x20FF);
        cpu.mem_write(cpu.get_pc(), 2);
        word old_pc = cpu.get_pc();
        OpCode::bvc(cpu, Relative);
        word expected_pc = old_pc + 1 + 2;
        assert(cpu.get_pc() == expected_pc);
        std::cout << "[BVC] Page crossing branch passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_bvc() {
    std::cout << "[BVC] Running tests...\n";
    test_bvc_branch_taken();
    test_bvc_branch_not_taken();
    test_bvc_page_cross();
    std::cout << "[BVC] All tests passed.\n\n";
}