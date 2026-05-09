#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 辅助：检查 C, Z, I, D, B, V, N 标志（CLI 只影响 I）
static void check_flags(const CPU& cpu, bool carry, bool zero, bool interrupt,
                        bool decimal, bool brk, bool overflow, bool negative) {
    byte status = cpu.get_status();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 2) & 1) == interrupt);
    assert(((status >> 3) & 1) == decimal);
    assert(((status >> 4) & 1) == brk);
    assert(((status >> 6) & 1) == overflow);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 隐含寻址（CLI 唯一模式） ----------
static void test_cli_clear_interrupt() {
    // 测试1：I 原本为 1，执行后 I=0，其他标志不变
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_flag(CPU::ZERO_FLAG, true);
        cpu.set_flag(CPU::INTERRUPT_FLAG, true);   // I=1
        cpu.set_flag(CPU::DECIMAL_FLAG, true);
        cpu.set_flag(CPU::BREAK_FLAG, true);
        cpu.set_flag(CPU::OVERFLOW_FLAG, true);
        cpu.set_flag(CPU::NEGATIVE_FLAG, true);
        
        OpCode::cli(cpu, Implied);
        
        // 只有 I 清零，其他标志保持为 1
        check_flags(cpu, true, true, false, true, true, true, true);
        std::cout << "[CLI] Clear interrupt from 1 passed\n";
    }
    
    // 测试2：I 原本为 0，执行后仍为 0
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::INTERRUPT_FLAG, false);
        OpCode::cli(cpu, Implied);
        assert(!(cpu.get_status() & CPU::INTERRUPT_FLAG));
        std::cout << "[CLI] Interrupt already 0 passed\n";
    }
    
    // 测试3：混合标志，仅改变 I
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_flag(CPU::ZERO_FLAG, false);
        cpu.set_flag(CPU::INTERRUPT_FLAG, true);
        cpu.set_flag(CPU::NEGATIVE_FLAG, true);
        
        OpCode::cli(cpu, Implied);
        
        assert((cpu.get_status() & CPU::CARRY_FLAG) != 0);
        assert((cpu.get_status() & CPU::ZERO_FLAG) == 0);
        assert((cpu.get_status() & CPU::INTERRUPT_FLAG) == 0);
        assert((cpu.get_status() & CPU::NEGATIVE_FLAG) != 0);
        std::cout << "[CLI] Only I changed passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_cli() {
    std::cout << "[CLI] Running tests...\n";
    test_cli_clear_interrupt();
    std::cout << "[CLI] All tests passed.\n\n";
}