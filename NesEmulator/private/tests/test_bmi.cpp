// test_bmi.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 辅助：检查 C、Z、N 标志（BMI 不影响标志）
static void check_flags(const CPU& cpu, bool carry, bool zero, bool negative) {
    byte status = cpu.get_status();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 相对寻址模式（BMI 唯一寻址方式） ----------
static void test_bmi_branch_taken() {
    // 测试1：向前跳转，N=1，偏移 +10
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::NEGATIVE_FLAG, true);   // N=1 满足跳转条件
        cpu.set_pc(0x2000);
        cpu.mem_write(cpu.get_pc(), 10);
        word old_pc = cpu.get_pc();
        OpCode::bmi(cpu, Relative);
        word expected_pc = old_pc + 1 + 10;
        assert(cpu.get_pc() == expected_pc);
        check_flags(cpu, false, false, true);    // N 仍为 1
        std::cout << "[BMI] Branch forward test passed\n";
    }

    // 测试2：向后跳转，N=1，偏移 -5
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::NEGATIVE_FLAG, true);
        cpu.set_pc(0x2000);
        cpu.mem_write(cpu.get_pc(), 0xFB);        // -5
        word old_pc = cpu.get_pc();
        OpCode::bmi(cpu, Relative);
        word expected_pc = old_pc + 1 + (signed char)0xFB;
        assert(cpu.get_pc() == expected_pc);
        check_flags(cpu, false, false, true);
        std::cout << "[BMI] Branch backward test passed\n";
    }

    // 测试3：边界值 +127
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::NEGATIVE_FLAG, true);
        cpu.set_pc(0x2100);
        cpu.mem_write(cpu.get_pc(), 127);
        word old_pc = cpu.get_pc();
        OpCode::bmi(cpu, Relative);
        word expected_pc = old_pc + 1 + 127;
        assert(cpu.get_pc() == expected_pc);
        std::cout << "[BMI] Branch max forward (+127) passed\n";
    }

    // 测试4：边界值 -128
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::NEGATIVE_FLAG, true);
        cpu.set_pc(0x2200);
        cpu.mem_write(cpu.get_pc(), 0x80);        // -128
        word old_pc = cpu.get_pc();
        OpCode::bmi(cpu, Relative);
        word expected_pc = old_pc + 1 + (signed char)0x80;
        assert(cpu.get_pc() == expected_pc);
        std::cout << "[BMI] Branch max backward (-128) passed\n";
    }
}

static void test_bmi_branch_not_taken() {
    // 测试5：N=0 时不跳转
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::NEGATIVE_FLAG, false);  // N=0 不满足跳转
        cpu.set_pc(0x3000);
        cpu.mem_write(cpu.get_pc(), 50);
        word old_pc = cpu.get_pc();
        OpCode::bmi(cpu, Relative);
        word expected_pc = old_pc + 1;
        assert(cpu.get_pc() == expected_pc);
        check_flags(cpu, false, false, false);   // N 仍为 0
        std::cout << "[BMI] No branch when N=0 passed\n";
    }
}

static void test_bmi_page_cross() {
    // 跨页测试
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::NEGATIVE_FLAG, true);
        cpu.set_pc(0x20FF);
        cpu.mem_write(cpu.get_pc(), 2);
        word old_pc = cpu.get_pc();
        OpCode::bmi(cpu, Relative);
        word expected_pc = old_pc + 1 + 2;
        assert(cpu.get_pc() == expected_pc);
        std::cout << "[BMI] Page crossing branch passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_bmi() {
    std::cout << "[BMI] Running tests...\n";
    test_bmi_branch_taken();
    test_bmi_branch_not_taken();
    test_bmi_page_cross();
    std::cout << "[BMI] All tests passed.\n\n";
}