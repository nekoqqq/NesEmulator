// test_dey.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 检查 Z, N 标志
static void check_flags(const CPU& cpu, bool zero, bool negative) {
    byte status = cpu.get_status();
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 隐含寻址 ----------
static void test_dey_implied() {
    // 测试1：0x01 -> 0x00, Z=1, N=0
    {
        CPU cpu;
        cpu.set_y(0x01);
        cpu.reset_stats();
        OpCode::dey(cpu, Implied);
        assert(cpu.get_y() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[DEY] Test 1 passed\n";
    }
    // 测试2：0x00 -> 0xFF, Z=0, N=1
    {
        CPU cpu;
        cpu.set_y(0x00);
        cpu.reset_stats();
        OpCode::dey(cpu, Implied);
        assert(cpu.get_y() == 0xFF);
        check_flags(cpu, false, true);
        std::cout << "[DEY] Test 2 passed\n";
    }
    // 测试3：0x80 -> 0x7F, Z=0, N=0
    {
        CPU cpu;
        cpu.set_y(0x80);
        cpu.reset_stats();
        OpCode::dey(cpu, Implied);
        assert(cpu.get_y() == 0x7F);
        check_flags(cpu, false, false);
        std::cout << "[DEY] Test 3 passed\n";
    }
    // 测试4：0xFF -> 0xFE, Z=0, N=1
    {
        CPU cpu;
        cpu.set_y(0xFF);
        cpu.reset_stats();
        OpCode::dey(cpu, Implied);
        assert(cpu.get_y() == 0xFE);
        check_flags(cpu, false, true);
        std::cout << "[DEY] Test 4 passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_dey() {
    std::cout << "[DEY] Running tests...\n";
    test_dey_implied();
    std::cout << "[DEY] All tests passed.\n\n";
}