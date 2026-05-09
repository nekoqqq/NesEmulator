// test_tax.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool zero, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

static void test_tax_implied() {
    // 测试1：传递非零非负值
    {
        CPU cpu;
        cpu.SetRegisterA(0x42);
        cpu.SetRegisterX(0x00);
        cpu.ResetStatus();
        OpCode::tax(cpu, Implied);
        assert(cpu.GetRegisterX() == 0x42);
        assert(cpu.GetRegisterA() == 0x42);
        check_flags(cpu, false, false);
        std::cout << "[TAX] Test 1 passed\n";
    }
    // 测试2：传递0，Z=1
    {
        CPU cpu;
        cpu.SetRegisterA(0x00);
        cpu.SetRegisterX(0xFF);
        cpu.ResetStatus();
        OpCode::tax(cpu, Implied);
        assert(cpu.GetRegisterX() == 0x00);
        assert(cpu.GetRegisterA() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[TAX] Test 2 passed\n";
    }
    // 测试3：传递负值，N=1
    {
        CPU cpu;
        cpu.SetRegisterA(0x80);
        cpu.SetRegisterX(0x00);
        cpu.ResetStatus();
        OpCode::tax(cpu, Implied);
        assert(cpu.GetRegisterX() == 0x80);
        assert(cpu.GetRegisterA() == 0x80);
        check_flags(cpu, false, true);
        std::cout << "[TAX] Test 3 passed\n";
    }
    // 测试4：确保不改变其他寄存器
    {
        CPU cpu;
        cpu.SetRegisterA(0x55);
        cpu.SetRegisterY(0xAA);
        cpu.SetRegisterX(0x00);
        cpu.ResetStatus();
        OpCode::tax(cpu, Implied);
        assert(cpu.GetRegisterY() == 0xAA);
        std::cout << "[TAX] Test 4 (Y unchanged) passed\n";
    }
}

void test_tax() {
    std::cout << "[TAX] Running tests...\n";
    test_tax_implied();
    std::cout << "[TAX] All tests passed.\n\n";
}