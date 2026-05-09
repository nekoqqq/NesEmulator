#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool zero, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

static void test_tya_implied() {
    // 测试1：Y=0x42，A得到0x42
    {
        CPU cpu;
        cpu.SetRegisterY(0x42);
        cpu.SetRegisterA(0x00);
        cpu.ResetStatus();
        OpCode::tya(cpu, Implied);
        assert(cpu.GetRegisterA() == 0x42);
        check_flags(cpu, false, false);
        std::cout << "[TYA] Test 1 passed\n";
    }
    // 测试2：Y=0x00，Z=1
    {
        CPU cpu;
        cpu.SetRegisterY(0x00);
        cpu.SetRegisterA(0xFF);
        cpu.ResetStatus();
        OpCode::tya(cpu, Implied);
        assert(cpu.GetRegisterA() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[TYA] Test 2 passed\n";
    }
    // 测试3：Y=0x80，N=1
    {
        CPU cpu;
        cpu.SetRegisterY(0x80);
        cpu.SetRegisterA(0x00);
        cpu.ResetStatus();
        OpCode::tya(cpu, Implied);
        assert(cpu.GetRegisterA() == 0x80);
        check_flags(cpu, false, true);
        std::cout << "[TYA] Test 3 passed\n";
    }
    // 测试4：X寄存器不变
    {
        CPU cpu;
        cpu.SetRegisterY(0x55);
        cpu.SetRegisterX(0xAA);
        cpu.ResetStatus();
        OpCode::tya(cpu, Implied);
        assert(cpu.GetRegisterX() == 0xAA);
        std::cout << "[TYA] Test 4 (X unchanged) passed\n";
    }
}

void test_tya() {
    std::cout << "[TYA] Running tests...\n";
    test_tya_implied();
    std::cout << "[TYA] All tests passed.\n\n";
}