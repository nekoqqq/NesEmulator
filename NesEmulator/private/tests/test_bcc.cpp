// test_bcc.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 辅助：检查 C、Z、N 标志（BCC 不应改变任何标志，但为了验证我们可读取当前值）
static void check_flags(const CPU& cpu, bool carry, bool zero, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 相对寻址模式（BCC 唯一寻址方式） ----------
static void test_bcc_branch_taken() {
    // 测试1：向前跳转，C=0，偏移 +10
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, false);   // C=0 满足跳转条件
        cpu.SetPC(0x2000);
        // 在 PC 指向的操作数位置写入偏移量
        cpu.mem_write(cpu.GetPC(), 10);        // 相对偏移 +10
        cpu.ResetStatus();                     // 确保只清除了标志？这里我们单独设置 C
        cpu.SetFlag(CPU::CARRY_FLAG, false);
        word old_pc = cpu.GetPC();
        OpCode::bcc(cpu, Relative);
        // 执行后 PC 应变为 old_pc + 1（跳过偏移字节） + 偏移量
        word expected_pc = old_pc + 1 + 10;
        assert(cpu.GetPC() == expected_pc);
        // 标志应保持不变（即 C=0，其他标志我们未改变所以为 0）
        check_flags(cpu, false, false, false);
        std::cout << "[BCC] Branch forward test passed\n";
    }

    // 测试2：向后跳转，C=0，偏移 -5（使用补码表示 251）
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, false);
        cpu.SetPC(0x2000);
        cpu.mem_write(cpu.GetPC(), 0xFB);      // -5 的补码
        word old_pc = cpu.GetPC();
        OpCode::bcc(cpu, Relative);
        word expected_pc = old_pc + 1 + (signed char)0xFB;
        assert(cpu.GetPC() == expected_pc);
        check_flags(cpu, false, false, false);
        std::cout << "[BCC] Branch backward test passed\n";
    }

    // 测试3：边界值 +127（最大正向跳转）
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, false);
        cpu.SetPC(0x2100);
        cpu.mem_write(cpu.GetPC(), 127);
        word old_pc = cpu.GetPC();
        OpCode::bcc(cpu, Relative);
        word expected_pc = old_pc + 1 + 127;
        assert(cpu.GetPC() == expected_pc);
        std::cout << "[BCC] Branch max forward (+127) passed\n";
    }

    // 测试4：边界值 -128（最大反向跳转）
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, false);
        cpu.SetPC(0x2200);
        cpu.mem_write(cpu.GetPC(), 0x80);      // -128
        word old_pc = cpu.GetPC();
        OpCode::bcc(cpu, Relative);
        word expected_pc = old_pc + 1 + (signed char)0x80;
        assert(cpu.GetPC() == expected_pc);
        std::cout << "[BCC] Branch max backward (-128) passed\n";
    }
}

static void test_bcc_branch_not_taken() {
    // 测试5：C=1 时不跳转
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);    // C=1 不满足跳转
        cpu.SetPC(0x3000);
        cpu.mem_write(cpu.GetPC(), 50);        // 偏移量被忽略
        word old_pc = cpu.GetPC();
        OpCode::bcc(cpu, Relative);
        word expected_pc = old_pc + 1;         // 仅跳过偏移字节，不添加偏移
        assert(cpu.GetPC() == expected_pc);
        check_flags(cpu, true, false, false);   // C 仍为 1
        std::cout << "[BCC] No branch when C=1 passed\n";
    }
}

// 可选：测试跨页边界时的周期惩罚（如果需要验证额外周期标志，可在这里模拟）
// 但 BCC 跨页时只是多消耗一个时钟周期，不影响 PC 正确性，因此无需专门测试。
static void test_bcc_page_cross() {
    // 验证跳转导致跨页时 PC 仍然正确（不影响结果）
    // 例如 PC=0x20FF，偏移 +2 导致目标 0x2101（跨页）
    {
        CPU cpu;
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, false);
        cpu.SetPC(0x20FF);
        cpu.mem_write(cpu.GetPC(), 2);
        word old_pc = cpu.GetPC();
        OpCode::bcc(cpu, Relative);
        word expected_pc = old_pc + 1 + 2;     // 0x2102? 注意 0x20FF+3=0x2102，正确
        assert(cpu.GetPC() == expected_pc);
        std::cout << "[BCC] Page crossing branch passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_bcc() {
    std::cout << "[BCC] Running tests...\n";
    test_bcc_branch_taken();
    test_bcc_branch_not_taken();
    test_bcc_page_cross();
    std::cout << "[BCC] All tests passed.\n\n";
}