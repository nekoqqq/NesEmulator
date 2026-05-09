// test_asl.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool carry, bool zero, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 累加器模式 ----------
static void test_asl_accumulator()
{
    // 测试1：普通左移
    {
        CPU cpu;
        cpu.SetRegisterA(0x12);
        cpu.ResetStatus();
        OpCode::asl(cpu, Accumulator);
        assert(cpu.GetRegisterA() == 0x24);
        check_flags(cpu, false, false, false);
        std::cout << "[ASL] Accumulator test 1 passed\n";
    }
    // 测试2：进位 + 结果为零
    {
        CPU cpu;
        cpu.SetRegisterA(0x80);
        cpu.ResetStatus();
        OpCode::asl(cpu, Accumulator);
        assert(cpu.GetRegisterA() == 0x00);
        check_flags(cpu, true, true, false);
        std::cout << "[ASL] Accumulator test 2 passed\n";
    }
    // 测试3：负标志
    {
        CPU cpu;
        cpu.SetRegisterA(0x40);
        cpu.ResetStatus();
        OpCode::asl(cpu, Accumulator);
        assert(cpu.GetRegisterA() == 0x80);
        check_flags(cpu, false, false, true);
        std::cout << "[ASL] Accumulator test 3 passed\n";
    }
}

// ---------- 零页模式 ----------
static void test_asl_zero_page()
{
    CPU cpu;
    word addr = 0x10;

    // 测试1：普通移位
    cpu.mem_write(addr, 0x12);
    cpu.SetPC(0x1000);
    cpu.mem_write_u16(cpu.GetPC(), addr); // 写入零页地址作为操作数
    cpu.ResetStatus();
    OpCode::asl(cpu, ZeroPage);
    assert(cpu.mem_read(addr) == 0x24);
    check_flags(cpu, false, false, false);
    std::cout << "[ASL] ZeroPage test 1 passed\n";

    // 测试2：进位 + 结果为零
    cpu.mem_write(addr, 0x80);
    cpu.SetPC(0x1000);
    cpu.mem_write_u16(cpu.GetPC(), addr);
    cpu.ResetStatus();
    OpCode::asl(cpu, ZeroPage);
    assert(cpu.mem_read(addr) == 0x00);
    check_flags(cpu, true, true, false);
    std::cout << "[ASL] ZeroPage test 2 passed\n";

    // 测试3：负标志
    cpu.mem_write(addr, 0x40);
    cpu.SetPC(0x1000);
    cpu.mem_write_u16(cpu.GetPC(), addr);
    cpu.ResetStatus();
    OpCode::asl(cpu, ZeroPage);
    assert(cpu.mem_read(addr) == 0x80);
    check_flags(cpu, false, false, true);
    std::cout << "[ASL] ZeroPage test 3 passed\n";
}

// ---------- 零页 X 模式 ----------
static void test_asl_zero_page_x()
{
    // 测试1：普通，基址 0x10，X=5 -> 0x15
    {
        CPU cpu;
        cpu.SetRegisterX(5);
        word target = 0x15;
        cpu.mem_write(target, 0x12);
        cpu.SetPC(0x1000);
        cpu.mem_write_u16(cpu.GetPC(), 0x10); // 操作数基址
        cpu.ResetStatus();
        OpCode::asl(cpu, ZeroPage_X);
        assert(cpu.mem_read(target) == 0x24);
        check_flags(cpu, false, false, false);
        std::cout << "[ASL] ZeroPageX test 1 passed\n";
    }
    // 测试2：零页绕回 (0xFF + 1 -> 0x00)
    {
        CPU cpu;
        cpu.SetRegisterX(1);
        cpu.mem_write(0x00, 0x80);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0xFF);
        cpu.ResetStatus();
        OpCode::asl(cpu, ZeroPage_X);
        assert(cpu.mem_read(0x00) == 0x00);
        check_flags(cpu, true, true, false);
        std::cout << "[ASL] ZeroPageX test 2 passed\n";
    }
    // 测试3：负标志
    {
        CPU cpu;
        cpu.SetRegisterX(2);
        cpu.mem_write(0x12, 0x40); // 基址 0x10, X=2 -> 0x12
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x10);
        cpu.ResetStatus();
        OpCode::asl(cpu, ZeroPage_X);
        assert(cpu.mem_read(0x12) == 0x80);
        check_flags(cpu, false, false, true);
        std::cout << "[ASL] ZeroPageX test 3 passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_asl_absolute()
{
    word addr = 0x1234;

    // 测试1：普通移位
    {
        CPU cpu;
        cpu.mem_write(addr, 0x12);
        cpu.SetPC(0x2000);
        cpu.mem_write_u16(cpu.GetPC(), addr); // 操作数（16位地址）
        cpu.ResetStatus();
        OpCode::asl(cpu, Absolute);
        assert(cpu.mem_read(addr) == 0x24);
        check_flags(cpu, false, false, false);
        std::cout << "[ASL] Absolute test 1 passed\n";
    }
    // 测试2：进位 + 结果为零
    {
        CPU cpu;
        cpu.mem_write(addr, 0x80);
        cpu.SetPC(0x2000);
        cpu.mem_write_u16(cpu.GetPC(), addr);
        cpu.ResetStatus();
        OpCode::asl(cpu, Absolute);
        assert(cpu.mem_read(addr) == 0x00);
        check_flags(cpu, true, true, false);
        std::cout << "[ASL] Absolute test 2 passed\n";
    }
    // 测试3：负标志
    {
        CPU cpu;
        cpu.mem_write(addr, 0x40);
        cpu.SetPC(0x2000);
        cpu.mem_write_u16(cpu.GetPC(), addr);
        cpu.ResetStatus();
        OpCode::asl(cpu, Absolute);
        assert(cpu.mem_read(addr) == 0x80);
        check_flags(cpu, false, false, true);
        std::cout << "[ASL] Absolute test 3 passed\n";
    }
}

// ---------- 绝对 X 模式 ----------
static void test_asl_absolute_x()
{
    // 测试1：普通，无跨页
    {
        CPU cpu;
        word base = 0x1200;
        byte x_val = 0x34;
        word target = base + x_val; // 0x1234
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0x12);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base); // 操作数基址
        cpu.ResetStatus();
        OpCode::asl(cpu, Absolute_X);
        assert(cpu.mem_read(target) == 0x24);
        check_flags(cpu, false, false, false);
        std::cout << "[ASL] AbsoluteX test 1 passed\n";
    }
    // 测试2：跨页，进位 + 结果为零
    {
        CPU cpu;
        word base = 0x12FF;
        byte x_val = 2;
        word target = base + x_val; // 0x1301
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0x80);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base);
        cpu.ResetStatus();
        OpCode::asl(cpu, Absolute_X);
        assert(cpu.mem_read(target) == 0x00);
        check_flags(cpu, true, true, false);
        std::cout << "[ASL] AbsoluteX test 2 passed\n";
    }
    // 测试3：负标志
    {
        CPU cpu;
        word base = 0x1000;
        byte x_val = 0x20;
        word target = base + x_val; // 0x1020
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0x40);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base);
        cpu.ResetStatus();
        OpCode::asl(cpu, Absolute_X);
        assert(cpu.mem_read(target) == 0x80);
        check_flags(cpu, false, false, true);
        std::cout << "[ASL] AbsoluteX test 3 passed\n";
    }
}

// ---------- 外部调用入口 ----------
void test_asl()
{
    std::cout << "[ASL] Running tests...\n";
    test_asl_accumulator();
    test_asl_zero_page();
    test_asl_zero_page_x();
    test_asl_absolute();
    test_asl_absolute_x();
    std::cout << "[ASL] All tests passed.\n";
}