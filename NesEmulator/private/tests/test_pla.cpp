#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool zero, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

static void test_pla_implied() {
    // 测试1：弹出一个非零非负值，标志正确
    {
        CPU cpu;
        // 预先在栈上放一个值（SP=0xFF压入0x42后SP变为0xFE）
        cpu.SetStackPointer(0xFE);
        cpu.mem_write(0x01FF, 0x42);   // 栈顶（SP+1的位置）
        cpu.ResetStatus();
        OpCode::pla(cpu, Implied);
        assert(cpu.GetRegisterA() == 0x42);
        assert(cpu.GetStackPointer() == 0xFF); // SP增加1
        check_flags(cpu, false, false);
        std::cout << "[PLA] Test 1 (pop non-zero) passed\n";
    }
    // 测试2：弹出0，Z=1
    {
        CPU cpu;
        cpu.SetStackPointer(0xFD);
        cpu.mem_write(0x01FE, 0x00);
        cpu.ResetStatus();
        OpCode::pla(cpu, Implied);
        assert(cpu.GetRegisterA() == 0x00);
        assert(cpu.GetStackPointer() == 0xFE);
        check_flags(cpu, true, false);
        std::cout << "[PLA] Test 2 (pop zero) passed\n";
    }
    // 测试3：弹出负值（>=0x80），N=1
    {
        CPU cpu;
        cpu.SetStackPointer(0xFC);
        cpu.mem_write(0x01FD, 0x80);
        cpu.ResetStatus();
        OpCode::pla(cpu, Implied);
        assert(cpu.GetRegisterA() == 0x80);
        assert(cpu.GetStackPointer() == 0xFD);
        check_flags(cpu, false, true);
        std::cout << "[PLA] Test 3 (pop negative) passed\n";
    }
    // 测试4：多次弹出，验证栈指针变化和值正确
    {
        CPU cpu;
        cpu.SetStackPointer(0xFD);
        cpu.mem_write(0x01FE, 0x11);
        cpu.mem_write(0x01FF, 0x22);
        // 第一次弹出
        OpCode::pla(cpu, Implied);
        assert(cpu.GetRegisterA() == 0x11);
        assert(cpu.GetStackPointer() == 0xFE);
        // 第二次弹出
        OpCode::pla(cpu, Implied);
        assert(cpu.GetRegisterA() == 0x22);
        assert(cpu.GetStackPointer() == 0xFF);
        std::cout << "[PLA] Test 4 (multiple pops) passed\n";
    }
}

void test_pla() {
    std::cout << "[PLA] Running tests...\n";
    test_pla_implied();
    std::cout << "[PLA] All tests passed.\n\n";
}