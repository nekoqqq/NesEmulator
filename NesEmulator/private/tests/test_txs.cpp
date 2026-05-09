#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void test_txs_implied() {
    // 测试1：X=0x42，SP变为0x42，标志不变
    {
        CPU cpu;
        cpu.set_x(0x42);
        cpu.set_sp(0xFF);
        cpu.reset_stats();
        byte expected_status = cpu.get_status();
        OpCode::txs(cpu, Implied);
        assert(cpu.get_sp() == 0x42);
        assert(cpu.get_status() == expected_status);
        std::cout << "[TXS] Test 1 passed\n";
    }
    // 测试2：X=0x00
    {
        CPU cpu;
        cpu.set_x(0x00);
        cpu.set_sp(0xFF);
        cpu.reset_stats();
        byte expected_status = cpu.get_status();
        OpCode::txs(cpu, Implied);
        assert(cpu.get_sp() == 0x00);
        assert(cpu.get_status() == expected_status);
        std::cout << "[TXS] Test 2 passed\n";
    }
    // 测试3：X=0xFF
    {
        CPU cpu;
        cpu.set_x(0xFF);
        cpu.set_sp(0x00);
        cpu.reset_stats();
        byte expected_status = cpu.get_status();
        OpCode::txs(cpu, Implied);
        assert(cpu.get_sp() == 0xFF);
        assert(cpu.get_status() == expected_status);
        std::cout << "[TXS] Test 3 passed\n";
    }
    // 测试4：寄存器A,Y不变
    {
        CPU cpu;
        cpu.set_x(0x10);
        cpu.set_a(0xAA);
        cpu.set_y(0xBB);
        cpu.reset_stats();
        OpCode::txs(cpu, Implied);
        assert(cpu.get_a() == 0xAA);
        assert(cpu.get_y() == 0xBB);
        std::cout << "[TXS] Test 4 (A,Y unchanged) passed\n";
    }
}

void test_txs() {
    std::cout << "[TXS] Running tests...\n";
    test_txs_implied();
    std::cout << "[TXS] All tests passed.\n\n";
}