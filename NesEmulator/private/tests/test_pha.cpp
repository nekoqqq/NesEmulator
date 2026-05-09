// test_pha.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags_unchanged(const CPU& cpu, byte expected_status) {
    assert(cpu.GetStatus() == expected_status);
}

static void test_pha_implied() {
    // 测试1：压栈非零值
    {
        CPU cpu;
        cpu.SetRegisterA(0xAB);
        cpu.SetStackPointer(0xFF);
        cpu.ResetStatus();
        byte expected_status = cpu.GetStatus();

        OpCode::pha(cpu, Implied);

        // 验证压栈数据
        byte sp_after = cpu.GetStackPointer();
        assert(sp_after == 0xFE);                     // SP 减 1
        byte pushed = cpu.mem_read(0x0100 + sp_after + 1); // 读取压入的值 (SP+1 位置)
        assert(pushed == 0xAB);
        // 标志不变
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[PHA] Test 1 (push non-zero) passed\n";
    }

    // 测试2：压栈零值
    {
        CPU cpu;
        cpu.SetRegisterA(0x00);
        cpu.SetStackPointer(0xFC);
        cpu.ResetStatus();
        byte expected_status = cpu.GetStatus();

        OpCode::pha(cpu, Implied);

        byte sp_after = cpu.GetStackPointer();
        assert(sp_after == 0xFB);
        byte pushed = cpu.mem_read(0x0100 + sp_after + 1);
        assert(pushed == 0x00);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[PHA] Test 2 (push zero) passed\n";
    }

    // 测试3：栈满时压栈（SP 初始 0xFF，压入后变为 0xFE）
    {
        CPU cpu;
        cpu.SetRegisterA(0x55);
        cpu.SetStackPointer(0xFF);
        cpu.ResetStatus();
        byte expected_status = cpu.GetStatus();

        OpCode::pha(cpu, Implied);

        assert(cpu.GetStackPointer() == 0xFE);
        byte pushed = cpu.mem_read(0x01FF);  // 原 SP=0xFF 时写入 0x01FF
        assert(pushed == 0x55);
        check_flags_unchanged(cpu, expected_status);
        std::cout << "[PHA] Test 3 (stack top) passed\n";
    }
}

void test_pha() {
    std::cout << "[PHA] Running tests...\n";
    test_pha_implied();
    std::cout << "[PHA] All tests passed.\n\n";
}