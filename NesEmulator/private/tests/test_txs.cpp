#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void test_txs_implied() {
    // 测试1：X=0x42，SP变为0x42，标志不变
    {
        CPU cpu;
        cpu.SetRegisterX(0x42);
        cpu.SetStackPointer(0xFF);
        cpu.ResetStatus();
        byte expected_status = cpu.GetStatus();
        OpCode::txs(cpu, Implied);
        assert(cpu.GetStackPointer() == 0x42);
        assert(cpu.GetStatus() == expected_status);
        std::cout << "[TXS] Test 1 passed\n";
    }
    // 测试2：X=0x00
    {
        CPU cpu;
        cpu.SetRegisterX(0x00);
        cpu.SetStackPointer(0xFF);
        cpu.ResetStatus();
        byte expected_status = cpu.GetStatus();
        OpCode::txs(cpu, Implied);
        assert(cpu.GetStackPointer() == 0x00);
        assert(cpu.GetStatus() == expected_status);
        std::cout << "[TXS] Test 2 passed\n";
    }
    // 测试3：X=0xFF
    {
        CPU cpu;
        cpu.SetRegisterX(0xFF);
        cpu.SetStackPointer(0x00);
        cpu.ResetStatus();
        byte expected_status = cpu.GetStatus();
        OpCode::txs(cpu, Implied);
        assert(cpu.GetStackPointer() == 0xFF);
        assert(cpu.GetStatus() == expected_status);
        std::cout << "[TXS] Test 3 passed\n";
    }
    // 测试4：寄存器A,Y不变
    {
        CPU cpu;
        cpu.SetRegisterX(0x10);
        cpu.SetRegisterA(0xAA);
        cpu.SetRegisterY(0xBB);
        cpu.ResetStatus();
        OpCode::txs(cpu, Implied);
        assert(cpu.GetRegisterA() == 0xAA);
        assert(cpu.GetRegisterY() == 0xBB);
        std::cout << "[TXS] Test 4 (A,Y unchanged) passed\n";
    }
}

void test_txs() {
    std::cout << "[TXS] Running tests...\n";
    test_txs_implied();
    std::cout << "[TXS] All tests passed.\n\n";
}