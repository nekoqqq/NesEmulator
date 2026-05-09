// test_iny.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool zero, bool negative) {
    byte status = cpu.get_status();
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 隐含寻址 ----------
static void test_iny_implied() {
    // 测试1：0x12 -> 0x13
    {
        CPU cpu;
        cpu.set_y(0x12);
        cpu.reset_stats();
        OpCode::iny(cpu, Implied);
        assert(cpu.get_y() == 0x13);
        check_flags(cpu, false, false);
        std::cout << "[INY] Test 1 passed\n";
    }
    // 测试2：0xFF -> 0x00, Z=1
    {
        CPU cpu;
        cpu.set_y(0xFF);
        cpu.reset_stats();
        OpCode::iny(cpu, Implied);
        assert(cpu.get_y() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[INY] Test 2 passed\n";
    }
    // 测试3：0x7F -> 0x80, N=1
    {
        CPU cpu;
        cpu.set_y(0x7F);
        cpu.reset_stats();
        OpCode::iny(cpu, Implied);
        assert(cpu.get_y() == 0x80);
        check_flags(cpu, false, true);
        std::cout << "[INY] Test 3 passed\n";
    }
    // 测试4：0x80 -> 0x81, N=1
    {
        CPU cpu;
        cpu.set_y(0x80);
        cpu.reset_stats();
        OpCode::iny(cpu, Implied);
        assert(cpu.get_y() == 0x81);
        check_flags(cpu, false, true);
        std::cout << "[INY] Test 4 passed\n";
    }
}

void test_iny() {
    std::cout << "[INY] Running tests...\n";
    test_iny_implied();
    std::cout << "[INY] All tests passed.\n\n";
}