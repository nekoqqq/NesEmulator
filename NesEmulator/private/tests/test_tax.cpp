// test_tax.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool zero, bool negative) {
    byte status = cpu.get_status();
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

static void test_tax_implied() {
    // 测试1：传递非零非负值
    {
        CPU cpu;
        cpu.set_a(0x42);
        cpu.set_x(0x00);
        cpu.reset_stats();
        OpCode::tax(cpu, Implied);
        assert(cpu.get_x() == 0x42);
        assert(cpu.get_a() == 0x42);
        check_flags(cpu, false, false);
        std::cout << "[TAX] Test 1 passed\n";
    }
    // 测试2：传递0，Z=1
    {
        CPU cpu;
        cpu.set_a(0x00);
        cpu.set_x(0xFF);
        cpu.reset_stats();
        OpCode::tax(cpu, Implied);
        assert(cpu.get_x() == 0x00);
        assert(cpu.get_a() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[TAX] Test 2 passed\n";
    }
    // 测试3：传递负值，N=1
    {
        CPU cpu;
        cpu.set_a(0x80);
        cpu.set_x(0x00);
        cpu.reset_stats();
        OpCode::tax(cpu, Implied);
        assert(cpu.get_x() == 0x80);
        assert(cpu.get_a() == 0x80);
        check_flags(cpu, false, true);
        std::cout << "[TAX] Test 3 passed\n";
    }
    // 测试4：确保不改变其他寄存器
    {
        CPU cpu;
        cpu.set_a(0x55);
        cpu.set_y(0xAA);
        cpu.set_x(0x00);
        cpu.reset_stats();
        OpCode::tax(cpu, Implied);
        assert(cpu.get_y() == 0xAA);
        std::cout << "[TAX] Test 4 (Y unchanged) passed\n";
    }
}

void test_tax() {
    std::cout << "[TAX] Running tests...\n";
    test_tax_implied();
    std::cout << "[TAX] All tests passed.\n\n";
}