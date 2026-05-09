// test_bcs.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 辅助：检查 C、Z、N 标志（BCS 不影响标志）
static void check_flags(const CPU& cpu, bool carry, bool zero, bool negative) {
    byte status = cpu.get_status();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 相对寻址模式（BCS 唯一寻址方式） ----------
static void test_bcs_branch_taken() {
    // 测试1：向前跳转，C=1，偏移 +10
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);    // C=1 满足跳转条件
        cpu.set_pc(0x2000);
        cpu.mem_write(cpu.get_pc(), 10);        // 相对偏移 +10
        word old_pc = cpu.get_pc();
        OpCode::bcs(cpu, Relative);
        word expected_pc = old_pc + 1 + 10;
        assert(cpu.get_pc() == expected_pc);
        check_flags(cpu, true, false, false);   // C 仍为 1
        std::cout << "[BCS] Branch forward test passed\n";
    }

    // 测试2：向后跳转，C=1，偏移 -5（补码 0xFB）
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_pc(0x2000);
        cpu.mem_write(cpu.get_pc(), 0xFB);      // -5
        word old_pc = cpu.get_pc();
        OpCode::bcs(cpu, Relative);
        word expected_pc = old_pc + 1 + (signed char)0xFB;
        assert(cpu.get_pc() == expected_pc);
        check_flags(cpu, true, false, false);
        std::cout << "[BCS] Branch backward test passed\n";
    }

    // 测试3：边界值 +127
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_pc(0x2100);
        cpu.mem_write(cpu.get_pc(), 127);
        word old_pc = cpu.get_pc();
        OpCode::bcs(cpu, Relative);
        word expected_pc = old_pc + 1 + 127;
        assert(cpu.get_pc() == expected_pc);
        std::cout << "[BCS] Branch max forward (+127) passed\n";
    }

    // 测试4：边界值 -128
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_pc(0x2200);
        cpu.mem_write(cpu.get_pc(), 0x80);      // -128
        word old_pc = cpu.get_pc();
        OpCode::bcs(cpu, Relative);
        word expected_pc = old_pc + 1 + (signed char)0x80;
        assert(cpu.get_pc() == expected_pc);
        std::cout << "[BCS] Branch max backward (-128) passed\n";
    }
}

static void test_bcs_branch_not_taken() {
    // 测试5：C=0 时不跳转
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);   // C=0 不满足跳转
        cpu.set_pc(0x3000);
        cpu.mem_write(cpu.get_pc(), 50);        // 偏移量被忽略
        word old_pc = cpu.get_pc();
        OpCode::bcs(cpu, Relative);
        word expected_pc = old_pc + 1;         // 仅跳过偏移字节
        assert(cpu.get_pc() == expected_pc);
        check_flags(cpu, false, false, false); // C 仍为 0
        std::cout << "[BCS] No branch when C=0 passed\n";
    }
}

static void test_bcs_page_cross() {
    // 跨页测试：PC=0x20FF，偏移 +2 -> 目标 0x2102
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_pc(0x20FF);
        cpu.mem_write(cpu.get_pc(), 2);
        word old_pc = cpu.get_pc();
        OpCode::bcs(cpu, Relative);
        word expected_pc = old_pc + 1 + 2;
        assert(cpu.get_pc() == expected_pc);
        std::cout << "[BCS] Page crossing branch passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_bcs() {
    std::cout << "[BCS] Running tests...\n";
    test_bcs_branch_taken();
    test_bcs_branch_not_taken();
    test_bcs_page_cross();
    std::cout << "[BCS] All tests passed.\n\n";
}