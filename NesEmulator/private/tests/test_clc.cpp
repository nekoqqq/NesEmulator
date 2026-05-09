#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 辅助：检查 C, Z, I, D, B, V, N 标志（CLC 只影响 C）
static void check_flags(const CPU& cpu, bool carry, bool zero, bool interrupt,
                        bool decimal, bool brk, bool overflow, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 2) & 1) == interrupt);
    assert(((status >> 3) & 1) == decimal);
    assert(((status >> 4) & 1) == brk);
    assert(((status >> 6) & 1) == overflow);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 隐含寻址（CLC 唯一模式） ----------
static void test_clc_clear_carry() {
    // 测试1：C 原本为 1，执行后 C=0，其他标志不变
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.SetFlag(CPU::ZERO_FLAG, true);
        cpu.SetFlag(CPU::INTERRUPT_FLAG, true);
        cpu.SetFlag(CPU::DECIMAL_FLAG, true);
        cpu.SetFlag(CPU::BREAK_FLAG, true);
        cpu.SetFlag(CPU::OVERFLOW_FLAG, true);
        cpu.SetFlag(CPU::NEGATIVE_FLAG, true);
        
        OpCode::clc(cpu, Implied);
        
        // 只有 C 清零，其他标志保持为 1
        check_flags(cpu, false, true, true, true, true, true, true);
        std::cout << "[CLC] Clear carry from 1 passed\n";
    }
    
    // 测试2：C 原本为 0，执行后仍为 0，其他标志任意（这里全 0）
    {
        CPU cpu;
        cpu.ResetStatus();  // 所有标志为 0
        OpCode::clc(cpu, Implied);
        check_flags(cpu, false, false, false, false, false, false, false);
        std::cout << "[CLC] Carry already 0 passed\n";
    }
    
    // 测试3：混合标志，仅改变 C
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.SetFlag(CPU::ZERO_FLAG, false);
        cpu.SetFlag(CPU::NEGATIVE_FLAG, true);
        
        OpCode::clc(cpu, Implied);
        
        assert(!(cpu.GetStatus() & CPU::CARRY_FLAG));
        assert((cpu.GetStatus() & CPU::ZERO_FLAG) == 0);
        assert((cpu.GetStatus() & CPU::NEGATIVE_FLAG) != 0);
        std::cout << "[CLC] Only C changed passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_clc() {
    std::cout << "[CLC] Running tests...\n";
    test_clc_clear_carry();
    std::cout << "[CLC] All tests passed.\n\n";
}