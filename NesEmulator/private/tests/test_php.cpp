// test_php.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void test_php_implied() {
    // 测试1：压栈，所有标志随机，验证 B 位被置1
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.SetFlag(CPU::ZERO_FLAG, false);
        cpu.SetFlag(CPU::INTERRUPT_FLAG, true);
        cpu.SetFlag(CPU::DECIMAL_FLAG, false);
        cpu.SetFlag(CPU::BREAK_FLAG, false); // 当前 B 为 0
        cpu.SetFlag(CPU::OVERFLOW_FLAG, true);
        cpu.SetFlag(CPU::NEGATIVE_FLAG, false);
        byte original_status = cpu.GetStatus();
        cpu.SetStackPointer(0xFF);
        
        OpCode::php(cpu, Implied);
        
        // 栈指针减1
        assert(cpu.GetStackPointer() == 0xFE);
        // 读取压栈的值（压入地址 = 0x0100 + 原SP = 0x01FF）
        byte pushed = cpu.mem_read(0x01FF);
        byte expected_status = original_status | CPU::BREAK_FLAG;
        assert(pushed == expected_status);
        // 当前状态应保持不变（B 仍为 0）
        assert(cpu.GetStatus() == original_status);
        std::cout << "[PHP] Test 1 passed\n";
    }
    
    // 测试2：压栈时所有标志为0，B 位应被置1
    {
        CPU cpu;
        cpu.ResetStatus(); // 所有标志 0
        cpu.SetStackPointer(0xFD);
        byte original = cpu.GetStatus();
        OpCode::php(cpu, Implied);
        assert(cpu.GetStackPointer() == 0xFC);
        byte pushed = cpu.mem_read(0x0100 + 0xFD); // 0x01FD
        assert(pushed == (original | CPU::BREAK_FLAG));
        assert(cpu.GetStatus() == original);
        std::cout << "[PHP] Test 2 passed\n";
    }
    
    // 测试3：栈顶压栈，验证不改变其他寄存器
    {
        CPU cpu;
        cpu.SetRegisterA(0x42);
        cpu.SetRegisterX(0x24);
        cpu.SetRegisterY(0x84);
        cpu.SetStackPointer(0xFF);
        cpu.ResetStatus();
        byte original_status = cpu.GetStatus();
        
        OpCode::php(cpu, Implied);
        
        assert(cpu.GetRegisterA() == 0x42);
        assert(cpu.GetRegisterX() == 0x24);
        assert(cpu.GetRegisterY() == 0x84);
        assert(cpu.GetStackPointer() == 0xFE);
        assert(cpu.GetStatus() == original_status);
        byte pushed = cpu.mem_read(0x01FF);
        assert(pushed == (original_status | CPU::BREAK_FLAG));
        std::cout << "[PHP] Test 3 passed\n";
    }
}

void test_php() {
    std::cout << "[PHP] Running tests...\n";
    test_php_implied();
    std::cout << "[PHP] All tests passed.\n\n";
}