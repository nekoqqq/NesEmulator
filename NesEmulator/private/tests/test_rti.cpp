#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool carry, bool zero, bool interrupt,
                        bool decimal, bool overflow, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 2) & 1) == interrupt);
    assert(((status >> 3) & 1) == decimal);
    assert(((status >> 6) & 1) == overflow);
    assert(((status >> 7) & 1) == negative);
}

static void test_rti_implied() {
    // 测试1：正常恢复
    {
        CPU cpu;
        // 模拟压栈后的状态：SP = 0xFC
        cpu.SetStackPointer(0xFC);
        byte status_value = 0x81;   // 1000 0001 -> N=1, C=1
        word pc_value = 0x3456;
        // 正确布局：状态在 SP+1=0x01FD, PC低在 SP+2=0x01FE, PC高在 SP+3=0x01FF
        cpu.mem_write(0x01FD, status_value);
        cpu.mem_write(0x01FE, pc_value & 0xFF);
        cpu.mem_write(0x01FF, (pc_value >> 8) & 0xFF);
        
        cpu.ResetStatus();
        cpu.SetPC(0x0000);
        
        OpCode::rti(cpu, Implied);
        
        // 验证
        assert(cpu.GetPC() == pc_value);
        byte expected_status = status_value & ~0x30;
        assert(cpu.GetStatus() == expected_status);
        assert(cpu.GetStackPointer() == 0xFF); // 0xFC + 3 = 0xFF
        check_flags(cpu, true, false, false, false, false, true);
        std::cout << "[RTI] Test 1 passed\n";
    }
    
    // 测试2：边界情况，相同布局
    {
        CPU cpu;
        cpu.SetStackPointer(0xFC);
        cpu.mem_write(0x01FD, 0x40);
        cpu.mem_write(0x01FE, 0x12);
        cpu.mem_write(0x01FF, 0x34);
        cpu.ResetStatus();
        OpCode::rti(cpu, Implied);
        assert(cpu.GetPC() == 0x3412);
        assert(cpu.GetStatus() == 0x40);
        assert(cpu.GetStackPointer() == 0xFF);
        std::cout << "[RTI] Test 2 passed\n";
    }
}

void test_rti() {
    std::cout << "[RTI] Running tests...\n";
    test_rti_implied();
    std::cout << "[RTI] All tests passed.\n\n";
}