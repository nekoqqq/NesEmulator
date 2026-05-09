// test_brk.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 辅助：检查标志（C, Z, I, D, B, V, N）
static void check_flags(const CPU& cpu, bool carry, bool zero, bool interrupt,
                        bool decimal, bool brk, bool overflow, bool negative) {
    byte status = cpu.get_status();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 2) & 1) == interrupt);
    assert(((status >> 3) & 1) == decimal);
    assert(((status >> 4) & 1) == brk);
    assert(((status >> 6) & 1) == overflow);
    assert(((status >> 7) & 1) == negative);
}

// 辅助：压栈（使用栈指针接口）
static void push_stack(CPU& cpu, byte value) {
    byte sp = cpu.get_sp();
    cpu.mem_write(0x0100 + sp, value);
    sp--;
    cpu.set_sp(sp);
}

// 辅助：弹栈
static byte pop_stack(CPU& cpu) {
    byte sp = cpu.get_sp();
    sp++;
    cpu.set_sp(sp);
    return cpu.mem_read(0x0100 + sp);
}

// ---------- BRK 测试 ----------
static void test_brk_basic() {
    CPU cpu;
    cpu.mem_write_word(0xFFFE, 0xABCD);
    cpu.reset_stats();
    cpu.set_pc(0x2000);
    cpu.set_sp(0xFF);

    OpCode::brk(cpu, Implied);

    assert(cpu.get_pc() == 0xABCD);

    byte sp_after = cpu.get_sp();               // 应为 0xFC（压了3字节）
    byte saved_status = cpu.mem_read(0x0100 + sp_after + 1);
    byte pc_low = cpu.mem_read(0x0100 + sp_after + 2);
    byte pc_high = cpu.mem_read(0x0100 + sp_after + 3);
    word saved_pc = (pc_high << 8) | pc_low;

    assert(saved_pc == 0x2002);
    assert((saved_status & 0x10) != 0);    // B 标志置位
    assert((cpu.get_status() & 0x04) != 0); // I 标志置位

    std::cout << "[BRK] Basic test passed\n";
}

static void test_brk_pc_increment() {
    CPU cpu;
    cpu.mem_write_word(0xFFFE, 0x1234);
    cpu.set_pc(0x3000);
    cpu.set_sp(0xFF);
    cpu.reset_stats();

    OpCode::brk(cpu, Implied);

    byte sp_after = cpu.get_sp();
    byte lo = cpu.mem_read(0x0100 + sp_after + 2);
    byte hi = cpu.mem_read(0x0100 + sp_after + 3);
    word saved_pc = (hi << 8) | lo;
    assert(saved_pc == 0x3002);

    std::cout << "[BRK] PC increment test passed\n";
}

static void test_brk_flag_behavior() {
    CPU cpu;
    cpu.mem_write_word(0xFFFE, 0x5678);
    cpu.set_pc(0x4000);
    cpu.set_sp(0xFF);
    cpu.reset_stats();
    cpu.set_flag(CPU::INTERRUPT_FLAG, false);

    OpCode::brk(cpu, Implied);

    assert((cpu.get_status() & 0x04) != 0); // I=1

    byte sp_after = cpu.get_sp();
    byte saved_status = cpu.mem_read(0x0100 + sp_after + 1);
    assert((saved_status & 0x10) != 0);    // B=1

    std::cout << "[BRK] Flag behavior test passed\n";
}

static void test_brk_vector_override() {
    CPU cpu;
    cpu.mem_write_word(0xFFFE, 0xDEAD);
    cpu.set_pc(0x5000);
    cpu.set_sp(0xFF);
    cpu.reset_stats();

    OpCode::brk(cpu, Implied);
    assert(cpu.get_pc() == 0xDEAD);

    std::cout << "[BRK] Vector override test passed\n";
}

// ---------- 外部调用入口 ----------
void test_brk() {
    std::cout << "[BRK] Running tests...\n";
    test_brk_basic();
    test_brk_pc_increment();
    test_brk_flag_behavior();
    test_brk_vector_override();
    std::cout << "[BRK] All tests passed.\n\n";
}