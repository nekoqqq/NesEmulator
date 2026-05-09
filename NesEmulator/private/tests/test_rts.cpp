// test_rts.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void test_rts_implied() {
    // 测试1：正常返回，栈上放置 (返回地址 - 1)
    {
        CPU cpu;
        // 模拟 JSR 压栈后的状态：压入返回地址-1
        // 假设返回地址为 0x3456，则压入 0x3455
        // 压栈顺序：先压高字节，再压低字节，压完 SP 指向下一个可用位置
        // 我们直接构造栈内容：SP=0xFD 时，数据在 0x01FE(低字节), 0x01FF(高字节)
        cpu.SetStackPointer(0xFD);
        word expected_return = 0x3456;
        word pushed = expected_return - 1;
        cpu.mem_write(0x01FE, pushed & 0xFF);        // 低字节
        cpu.mem_write(0x01FF, (pushed >> 8) & 0xFF); // 高字节
        
        cpu.SetPC(0x0000);
        OpCode::rts(cpu, Implied);
        
        assert(cpu.GetPC() == expected_return);
        assert(cpu.GetStackPointer() == 0xFF); // 弹两次后 SP = 0xFD + 2 = 0xFF
        std::cout << "[RTS] Test 1 passed\n";
    }
    
    // 测试2：边界情况，跨页
    {
        CPU cpu;
        cpu.SetStackPointer(0xFC);
        word expected_return = 0x1300;
        word pushed = expected_return - 1; // 0x12FF
        cpu.mem_write(0x01FD, pushed & 0xFF);
        cpu.mem_write(0x01FE, (pushed >> 8) & 0xFF);
        cpu.SetPC(0x0000);
        OpCode::rts(cpu, Implied);
        assert(cpu.GetPC() == expected_return);
        assert(cpu.GetStackPointer() == 0xFE);
        std::cout << "[RTS] Test 2 passed\n";
    }
    
    // 测试3：确认 RTS 不改变标志
    {
        CPU cpu;
        cpu.SetStackPointer(0xFC);
        cpu.mem_write(0x01FD, 0x01);
        cpu.mem_write(0x01FE, 0x02);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        byte expected_status = cpu.GetStatus();
        OpCode::rts(cpu, Implied);
        assert(cpu.GetStatus() == expected_status);
        std::cout << "[RTS] Test 3 (flags unchanged) passed\n";
    }
}

void test_rts() {
    std::cout << "[RTS] Running tests...\n";
    test_rts_implied();
    std::cout << "[RTS] All tests passed.\n\n";
}