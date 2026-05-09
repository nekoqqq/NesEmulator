// test_php.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void test_php_implied() {
    // 测试1：压栈，所有标志随机，验证 B 位被置1
    {
        CPU cpu;
        cpu.reset_stats();
        cpu.set_flag(CPU::CARRY_FLAG, true);
        cpu.set_flag(CPU::ZERO_FLAG, false);
        cpu.set_flag(CPU::INTERRUPT_FLAG, true);
        cpu.set_flag(CPU::DECIMAL_FLAG, false);
        cpu.set_flag(CPU::BREAK_FLAG, false); // 当前 B 为 0
        cpu.set_flag(CPU::OVERFLOW_FLAG, true);
        cpu.set_flag(CPU::NEGATIVE_FLAG, false);
        byte original_status = cpu.get_status();
        cpu.set_sp(0xFF);
        
        OpCode::php(cpu, Implied);
        
        // 栈指针减1
        assert(cpu.get_sp() == 0xFE);
        // 读取压栈的值（压入地址 = 0x0100 + 原SP = 0x01FF）
        byte pushed = cpu.mem_read(0x01FF);
        byte expected_status = original_status | CPU::BREAK_FLAG;
        assert(pushed == expected_status);
        // 当前状态应保持不变（B 仍为 0）
        assert(cpu.get_status() == original_status);
        std::cout << "[PHP] Test 1 passed\n";
    }
    
    // 测试2：压栈时所有标志为0，B 位应被置1
    {
        CPU cpu;
        cpu.reset_stats(); // 所有标志 0
        cpu.set_sp(0xFD);
        byte original = cpu.get_status();
        OpCode::php(cpu, Implied);
        assert(cpu.get_sp() == 0xFC);
        byte pushed = cpu.mem_read(0x0100 + 0xFD); // 0x01FD
        assert(pushed == (original | CPU::BREAK_FLAG));
        assert(cpu.get_status() == original);
        std::cout << "[PHP] Test 2 passed\n";
    }
    
    // 测试3：栈顶压栈，验证不改变其他寄存器
    {
        CPU cpu;
        cpu.set_a(0x42);
        cpu.set_x(0x24);
        cpu.set_y(0x84);
        cpu.set_sp(0xFF);
        cpu.reset_stats();
        byte original_status = cpu.get_status();
        
        OpCode::php(cpu, Implied);
        
        assert(cpu.get_a() == 0x42);
        assert(cpu.get_x() == 0x24);
        assert(cpu.get_y() == 0x84);
        assert(cpu.get_sp() == 0xFE);
        assert(cpu.get_status() == original_status);
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