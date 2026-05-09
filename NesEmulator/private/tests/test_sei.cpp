// test_sei.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool carry, bool zero, bool interrupt,
                        bool decimal, bool overflow, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 2) & 1) == interrupt);
    assert(((status >> 3) & 1) == decimal);
    assert(((status >> 6) & 1) == overflow);
    assert(((status >> 7) & 1) == negative);
}

static void test_sei_implied() {
    // 测试1：中断标志由0变1
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::INTERRUPT_FLAG, false);
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.SetFlag(CPU::ZERO_FLAG, true);
        cpu.SetFlag(CPU::DECIMAL_FLAG, true);
        cpu.SetFlag(CPU::OVERFLOW_FLAG, true);
        cpu.SetFlag(CPU::NEGATIVE_FLAG, true);
        OpCode::sei(cpu, Implied);
        check_flags(cpu, true, true, true, true, true, true);
        std::cout << "[SEI] Set interrupt from 0 passed\n";
    }
    // 测试2：中断已是1，保持为1
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::INTERRUPT_FLAG, true);
        OpCode::sei(cpu, Implied);
        check_flags(cpu, false, false, true, false, false, false);
        std::cout << "[SEI] Interrupt already 1 passed\n";
    }
}

void test_sei() {
    std::cout << "[SEI] Running tests...\n";
    test_sei_implied();
    std::cout << "[SEI] All tests passed.\n\n";
}