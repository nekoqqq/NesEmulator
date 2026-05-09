// test_inx.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool zero, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 隐含寻址 ----------
static void test_inx_implied() {
    // 测试1：0x12 -> 0x13
    {
        CPU cpu;
        cpu.SetRegisterX(0x12);
        cpu.ResetStatus();
        OpCode::inx(cpu, Implied);
        assert(cpu.GetRegisterX() == 0x13);
        check_flags(cpu, false, false);
        std::cout << "[INX] Test 1 passed\n";
    }
    // 测试2：0xFF -> 0x00, Z=1
    {
        CPU cpu;
        cpu.SetRegisterX(0xFF);
        cpu.ResetStatus();
        OpCode::inx(cpu, Implied);
        assert(cpu.GetRegisterX() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[INX] Test 2 passed\n";
    }
    // 测试3：0x7F -> 0x80, N=1
    {
        CPU cpu;
        cpu.SetRegisterX(0x7F);
        cpu.ResetStatus();
        OpCode::inx(cpu, Implied);
        assert(cpu.GetRegisterX() == 0x80);
        check_flags(cpu, false, true);
        std::cout << "[INX] Test 3 passed\n";
    }
    // 测试4：0x80 -> 0x81, N=1
    {
        CPU cpu;
        cpu.SetRegisterX(0x80);
        cpu.ResetStatus();
        OpCode::inx(cpu, Implied);
        assert(cpu.GetRegisterX() == 0x81);
        check_flags(cpu, false, true);
        std::cout << "[INX] Test 4 passed\n";
    }
}

void test_inx() {
    std::cout << "[INX] Running tests...\n";
    test_inx_implied();
    std::cout << "[INX] All tests passed.\n\n";
}