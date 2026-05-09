// test_nop.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

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

static void test_nop_implied() {
    // 测试1：NOP 不改变任何寄存器或标志，PC 加 1（由外层处理）
    {
        CPU cpu;
        cpu.SetRegisterA(0xAB);
        cpu.SetRegisterX(0xCD);
        cpu.SetRegisterY(0xEF);
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.SetFlag(CPU::ZERO_FLAG, true);
        cpu.SetFlag(CPU::INTERRUPT_FLAG, true);
        cpu.SetFlag(CPU::DECIMAL_FLAG, true);
        cpu.SetFlag(CPU::BREAK_FLAG, true);
        cpu.SetFlag(CPU::OVERFLOW_FLAG, true);
        cpu.SetFlag(CPU::NEGATIVE_FLAG, true);
        byte expected_status = cpu.GetStatus();

        OpCode::nop(cpu, Implied);

        assert(cpu.GetRegisterA() == 0xAB);
        assert(cpu.GetRegisterX() == 0xCD);
        assert(cpu.GetRegisterY() == 0xEF);
        assert(cpu.GetStatus() == expected_status);
        // PC 的增加由外层指令循环负责，不在 NOP 指令内处理
        std::cout << "[NOP] Implied test 1 passed\n";
    }

    // 测试2：所有标志为 0，确保未被修改
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetRegisterA(0x00);
        cpu.SetRegisterX(0x00);
        cpu.SetRegisterY(0x00);
        byte expected_status = 0;

        OpCode::nop(cpu, Implied);

        assert(cpu.GetRegisterA() == 0x00);
        assert(cpu.GetRegisterX() == 0x00);
        assert(cpu.GetRegisterY() == 0x00);
        assert(cpu.GetStatus() == expected_status);
        std::cout << "[NOP] Implied test 2 passed\n";
    }
}

void test_nop() {
    std::cout << "[NOP] Running tests...\n";
    test_nop_implied();
    std::cout << "[NOP] All tests passed.\n\n";
}