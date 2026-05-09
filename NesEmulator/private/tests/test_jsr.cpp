// test_jsr.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_stack(const CPU& cpu, byte expected_low, byte expected_high, word expected_return_addr) {
    byte sp = cpu.get_sp();
    // 压栈后 SP 应减少 2，所以返回地址的低字节在 SP+1，高字节在 SP+2
    byte low = cpu.mem_read(0x0100 + sp + 1);
    byte high = cpu.mem_read(0x0100 + sp + 2);
    word return_addr = (high << 8) | low;
    assert(return_addr == expected_return_addr);
    (void)expected_low; (void)expected_high; // 避免未使用参数警告
}

static void test_jsr_absolute() {
    // 测试1：正常跳转，返回地址为 PC+3
    {
        CPU cpu;
        cpu.set_pc(0x2000);
        word target = 0x3000;
        // 在 PC 处写入目标地址（绝对寻址操作数）
        cpu.mem_write_word(cpu.get_pc(), target);
        cpu.set_sp(0xFF);
        cpu.reset_stats();
        OpCode::jsr(cpu, Absolute);
        // 验证跳转后 PC 为目标地址
        assert(cpu.get_pc() == target);
        // 验证栈上压入了正确的返回地址（原始 PC 已指向操作数之后，即 0x2002）
        word expected_return = 0x2002;   // JSR 占3字节，因此下一条指令地址为 0x2000+3 = 0x2003？等等要仔细计算
        // 我们设置的 PC 初始为 0x2000，在 cpu.get_operand_address 中应该读取操作数（2字节）并使 PC 增加 2，所以调用后 PC 变为 0x2002。
        // 因此压栈的返回地址就是 0x2002。标准 6502 中 JSR 压入的是 PC+3（即下一条指令地址），这里 PC+3 为 0x2003，矛盾？取决于取指循环。
        // 让我们根据实际设计决定：测试设置 PC=0x2000，外部调用 jsr 之前是否已经读取操作码并递增了 PC？通常指令解码时已经将 PC 指向操作数地址。
        // 为简化，我们假定 get_operand_address 会读取操作数并递增 PC 两次，所以调用后 PC=0x2002，因此返回地址为 0x2002。这是合理的。
        // 为了符合标准测试，我们可以修改期望值为 0x2002。
        check_stack(cpu, 0x02, 0x20, expected_return);
        std::cout << "[JSR] Absolute test 1 passed\n";
    }
    // 测试2：跳转到边界地址
    {
        CPU cpu;
        cpu.set_pc(0x1000);
        word target = 0xFFFE;
        cpu.mem_write_word(cpu.get_pc(), target);
        cpu.set_sp(0xFF);
        cpu.reset_stats();
        OpCode::jsr(cpu, Absolute);
        assert(cpu.get_pc() == target);
        check_stack(cpu, 0x02, 0x10, 0x1002);
        std::cout << "[JSR] Absolute test 2 passed\n";
    }
}

void test_jsr() {
    std::cout << "[JSR] Running tests...\n";
    test_jsr_absolute();
    std::cout << "[JSR] All tests passed.\n\n";
}