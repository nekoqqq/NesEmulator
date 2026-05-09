#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool zero, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

static void test_tsx_implied() {
    // 测试1：SP=0x42，X得到0x42，非零非负
    {
        CPU cpu;
        cpu.SetStackPointer(0x42);
        cpu.SetRegisterX(0x00);
        cpu.ResetStatus();
        OpCode::tsx(cpu, Implied);
        assert(cpu.GetRegisterX() == 0x42);
        check_flags(cpu, false, false);
        std::cout << "[TSX] Test 1 passed\n";
    }
    // 测试2：SP=0x00，Z=1
    {
        CPU cpu;
        cpu.SetStackPointer(0x00);
        cpu.SetRegisterX(0xFF);
        cpu.ResetStatus();
        OpCode::tsx(cpu, Implied);
        assert(cpu.GetRegisterX() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[TSX] Test 2 passed\n";
    }
    // 测试3：SP=0x80，N=1
    {
        CPU cpu;
        cpu.SetStackPointer(0x80);
        cpu.SetRegisterX(0x00);
        cpu.ResetStatus();
        OpCode::tsx(cpu, Implied);
        assert(cpu.GetRegisterX() == 0x80);
        check_flags(cpu, false, true);
        std::cout << "[TSX] Test 3 passed\n";
    }
    // 测试4：其他寄存器不变（A,Y）
    {
        CPU cpu;
        cpu.SetStackPointer(0x55);
        cpu.SetRegisterA(0xAA);
        cpu.SetRegisterY(0xBB);
        cpu.ResetStatus();
        OpCode::tsx(cpu, Implied);
        assert(cpu.GetRegisterA() == 0xAA);
        assert(cpu.GetRegisterY() == 0xBB);
        std::cout << "[TSX] Test 4 (A,Y unchanged) passed\n";
    }
}

void test_tsx() {
    std::cout << "[TSX] Running tests...\n";
    test_tsx_implied();
    std::cout << "[TSX] All tests passed.\n\n";
}