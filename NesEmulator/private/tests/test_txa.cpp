#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool zero, bool negative) {
    byte status = cpu.get_status();
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

static void test_txa_implied() {
    // 测试1：X=0x42，A得到0x42
    {
        CPU cpu;
        cpu.set_x(0x42);
        cpu.set_a(0x00);
        cpu.reset_stats();
        OpCode::txa(cpu, Implied);
        assert(cpu.get_a() == 0x42);
        check_flags(cpu, false, false);
        std::cout << "[TXA] Test 1 passed\n";
    }
    // 测试2：X=0x00，Z=1
    {
        CPU cpu;
        cpu.set_x(0x00);
        cpu.set_a(0xFF);
        cpu.reset_stats();
        OpCode::txa(cpu, Implied);
        assert(cpu.get_a() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[TXA] Test 2 passed\n";
    }
    // 测试3：X=0x80，N=1
    {
        CPU cpu;
        cpu.set_x(0x80);
        cpu.set_a(0x00);
        cpu.reset_stats();
        OpCode::txa(cpu, Implied);
        assert(cpu.get_a() == 0x80);
        check_flags(cpu, false, true);
        std::cout << "[TXA] Test 3 passed\n";
    }
    // 测试4：Y寄存器不变
    {
        CPU cpu;
        cpu.set_x(0x55);
        cpu.set_y(0xAA);
        cpu.reset_stats();
        OpCode::txa(cpu, Implied);
        assert(cpu.get_y() == 0xAA);
        std::cout << "[TXA] Test 4 (Y unchanged) passed\n";
    }
}

void test_txa() {
    std::cout << "[TXA] Running tests...\n";
    test_txa_implied();
    std::cout << "[TXA] All tests passed.\n\n";
}