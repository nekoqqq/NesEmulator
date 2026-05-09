// test_sec.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool carry, bool zero, bool interrupt,
                        bool decimal, bool overflow, bool negative) {
    byte status = cpu.get_status();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 2) & 1) == interrupt);
    assert(((status >> 3) & 1) == decimal);
    assert(((status >> 6) & 1) == overflow);
    assert(((status >> 7) & 1) == negative);
}

static void test_sec_implied() {
    // 测试1：进位由0变1
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, false);
        cpu.set_flag(CPU::ZERO_FLAG, true);
        cpu.set_flag(CPU::INTERRUPT_FLAG, true);
        cpu.set_flag(CPU::DECIMAL_FLAG, true);
        cpu.set_flag(CPU::OVERFLOW_FLAG, true);
        cpu.set_flag(CPU::NEGATIVE_FLAG, true);
        OpCode::sec(cpu, Implied);
        check_flags(cpu, true, true, true, true, true, true);
        std::cout << "[SEC] Set carry from 0 passed\n";
    }
    // 测试2：进位已是1，保持为1
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_flag(CPU::ZERO_FLAG, false);
        OpCode::sec(cpu, Implied);
        check_flags(cpu, true, false, false, false, false, false);
        std::cout << "[SEC] Carry already 1 passed\n";
    }
}

void test_sec() {
    std::cout << "[SEC] Running tests...\n";
    test_sec_implied();
    std::cout << "[SEC] All tests passed.\n\n";
}