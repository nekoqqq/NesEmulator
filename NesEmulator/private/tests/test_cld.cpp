#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 辅助：检查 C, Z, I, D, B, V, N 标志（CLD 只影响 D）
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

// ---------- 隐含寻址（CLD 唯一模式） ----------
static void test_cld_clear_decimal() {
    // 测试1：D 原本为 1，执行后 D=0，其他标志不变
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.SetFlag(CPU::ZERO_FLAG, true);
        cpu.SetFlag(CPU::INTERRUPT_FLAG, true);
        cpu.SetFlag(CPU::DECIMAL_FLAG, true);   // D=1
        cpu.SetFlag(CPU::BREAK_FLAG, true);
        cpu.SetFlag(CPU::OVERFLOW_FLAG, true);
        cpu.SetFlag(CPU::NEGATIVE_FLAG, true);
        
        OpCode::cld(cpu, Implied);
        
        // 只有 D 清零，其他标志保持为 1
        check_flags(cpu, true, true, true, false, true, true, true);
        std::cout << "[CLD] Clear decimal from 1 passed\n";
    }
    
    // 测试2：D 原本为 0，执行后仍为 0
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::DECIMAL_FLAG, false);
        OpCode::cld(cpu, Implied);
        assert(!(cpu.GetStatus() & CPU::DECIMAL_FLAG));
        std::cout << "[CLD] Decimal already 0 passed\n";
    }
    
    // 测试3：混合标志，仅改变 D
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.SetFlag(CPU::ZERO_FLAG, false);
        cpu.SetFlag(CPU::DECIMAL_FLAG, true);
        cpu.SetFlag(CPU::NEGATIVE_FLAG, true);
        
        OpCode::cld(cpu, Implied);
        
        assert((cpu.GetStatus() & CPU::CARRY_FLAG) != 0);
        assert((cpu.GetStatus() & CPU::ZERO_FLAG) == 0);
        assert((cpu.GetStatus() & CPU::DECIMAL_FLAG) == 0);
        assert((cpu.GetStatus() & CPU::NEGATIVE_FLAG) != 0);
        std::cout << "[CLD] Only D changed passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_cld() {
    std::cout << "[CLD] Running tests...\n";
    test_cld_clear_decimal();
    std::cout << "[CLD] All tests passed.\n\n";
}