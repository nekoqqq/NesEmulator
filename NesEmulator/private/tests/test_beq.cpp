// test_beq.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 辅助：检查 C、Z、N 标志（BEQ 不影响标志）
static void check_flags(const CPU& cpu, bool carry, bool zero, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 相对寻址模式（BEQ 唯一寻址方式） ----------
static void test_beq_branch_taken() {
    // 测试1：向前跳转，Z=1，偏移 +10
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::ZERO_FLAG, true);     // Z=1 满足跳转条件
        cpu.SetPC(0x2000);
        cpu.mem_write(cpu.GetPC(), 10);        // 相对偏移 +10
        word old_pc = cpu.GetPC();
        OpCode::beq(cpu, Relative);
        word expected_pc = old_pc + 1 + 10;
        assert(cpu.GetPC() == expected_pc);
        check_flags(cpu, false, true, false);   // Z 仍为 1
        std::cout << "[BEQ] Branch forward test passed\n";
    }

    // 测试2：向后跳转，Z=1，偏移 -5（补码 0xFB）
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::ZERO_FLAG, true);
        cpu.SetPC(0x2000);
        cpu.mem_write(cpu.GetPC(), 0xFB);      // -5
        word old_pc = cpu.GetPC();
        OpCode::beq(cpu, Relative);
        word expected_pc = old_pc + 1 + (signed char)0xFB;
        assert(cpu.GetPC() == expected_pc);
        check_flags(cpu, false, true, false);
        std::cout << "[BEQ] Branch backward test passed\n";
    }

    // 测试3：边界值 +127
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::ZERO_FLAG, true);
        cpu.SetPC(0x2100);
        cpu.mem_write(cpu.GetPC(), 127);
        word old_pc = cpu.GetPC();
        OpCode::beq(cpu, Relative);
        word expected_pc = old_pc + 1 + 127;
        assert(cpu.GetPC() == expected_pc);
        std::cout << "[BEQ] Branch max forward (+127) passed\n";
    }

    // 测试4：边界值 -128
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::ZERO_FLAG, true);
        cpu.SetPC(0x2200);
        cpu.mem_write(cpu.GetPC(), 0x80);      // -128
        word old_pc = cpu.GetPC();
        OpCode::beq(cpu, Relative);
        word expected_pc = old_pc + 1 + (signed char)0x80;
        assert(cpu.GetPC() == expected_pc);
        std::cout << "[BEQ] Branch max backward (-128) passed\n";
    }
}

static void test_beq_branch_not_taken() {
    // 测试5：Z=0 时不跳转
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::ZERO_FLAG, false);    // Z=0 不满足跳转
        cpu.SetPC(0x3000);
        cpu.mem_write(cpu.GetPC(), 50);        // 偏移量被忽略
        word old_pc = cpu.GetPC();
        OpCode::beq(cpu, Relative);
        word expected_pc = old_pc + 1;         // 仅跳过偏移字节
        assert(cpu.GetPC() == expected_pc);
        check_flags(cpu, false, false, false); // Z 仍为 0
        std::cout << "[BEQ] No branch when Z=0 passed\n";
    }
}

static void test_beq_page_cross() {
    // 跨页测试：PC=0x20FF，偏移 +2 -> 目标 0x2102
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::ZERO_FLAG, true);
        cpu.SetPC(0x20FF);
        cpu.mem_write(cpu.GetPC(), 2);
        word old_pc = cpu.GetPC();
        OpCode::beq(cpu, Relative);
        word expected_pc = old_pc + 1 + 2;
        assert(cpu.GetPC() == expected_pc);
        std::cout << "[BEQ] Page crossing branch passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_beq() {
    std::cout << "[BEQ] Running tests...\n";
    test_beq_branch_taken();
    test_beq_branch_not_taken();
    test_beq_page_cross();
    std::cout << "[BEQ] All tests passed.\n\n";
}