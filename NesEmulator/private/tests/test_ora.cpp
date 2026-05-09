// test_ora.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool zero, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 立即数模式 ----------
static void test_ora_immediate() {
    // 测试1：0x12 | 0x34 = 0x36
    {
        CPU cpu;
        cpu.SetRegisterA(0x12);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x34);
        cpu.ResetStatus();
        OpCode::op_ora(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0x36);
        check_flags(cpu, false, false);
        std::cout << "[ORA] Immediate test 1 passed\n";
    }
    // 测试2：0x00 | 0x00 = 0x00, Z=1
    {
        CPU cpu;
        cpu.SetRegisterA(0x00);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x00);
        cpu.ResetStatus();
        OpCode::op_ora(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[ORA] Immediate test 2 passed\n";
    }
    // 测试3：0x80 | 0x01 = 0x81, N=1
    {
        CPU cpu;
        cpu.SetRegisterA(0x80);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x01);
        cpu.ResetStatus();
        OpCode::op_ora(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0x81);
        check_flags(cpu, false, true);
        std::cout << "[ORA] Immediate test 3 passed\n";
    }
}

// ---------- 零页模式 ----------
static void test_ora_zero_page() {
    word addr = 0x10;
    {
        CPU cpu;
        cpu.SetRegisterA(0x50);
        cpu.mem_write(addr, 0x0A);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        OpCode::op_ora(cpu, ZeroPage);
        assert(cpu.GetRegisterA() == 0x5A);
        check_flags(cpu, false, false);
        std::cout << "[ORA] ZeroPage test 1 passed\n";
    }
}

// ---------- 零页 X 模式 ----------
static void test_ora_zero_page_x() {
    byte base = 0x10;
    byte x_val = 5;
    word target = (base + x_val) & 0xFF; // 0x15
    {
        CPU cpu;
        cpu.SetRegisterA(0x33);
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0xCC);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), base);
        cpu.ResetStatus();
        OpCode::op_ora(cpu, ZeroPage_X);
        assert(cpu.GetRegisterA() == 0xFF);
        check_flags(cpu, false, true);
        std::cout << "[ORA] ZeroPageX test 1 passed\n";
    }
    // 绕回测试
    {
        CPU cpu;
        cpu.SetRegisterA(0x01);
        cpu.SetRegisterX(1);
        cpu.mem_write(0x00, 0x02);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0xFF);
        cpu.ResetStatus();
        OpCode::op_ora(cpu, ZeroPage_X);
        assert(cpu.GetRegisterA() == 0x03);
        check_flags(cpu, false, false);
        std::cout << "[ORA] ZeroPageX test 2 (wrap) passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_ora_absolute() {
    word addr = 0x1234;
    {
        CPU cpu;
        cpu.SetRegisterA(0x0F);
        cpu.mem_write(addr, 0xF0);
        cpu.SetPC(0x2000);
        cpu.mem_write_u16(cpu.GetPC(), addr);
        cpu.ResetStatus();
        OpCode::op_ora(cpu, Absolute);
        assert(cpu.GetRegisterA() == 0xFF);
        check_flags(cpu, false, true);
        std::cout << "[ORA] Absolute test 1 passed\n";
    }
}

// ---------- 绝对 X 模式 ----------
static void test_ora_absolute_x() {
    word base = 0x1200;
    byte x_val = 0x34;
    word target = base + x_val; // 0x1234
    {
        CPU cpu;
        cpu.SetRegisterA(0x55);
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0xAA);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base);
        cpu.ResetStatus();
        OpCode::op_ora(cpu, Absolute_X);
        assert(cpu.GetRegisterA() == 0xFF);
        check_flags(cpu, false, true);
        std::cout << "[ORA] AbsoluteX test 1 passed\n";
    }
    // 跨页测试
    {
        CPU cpu;
        word base2 = 0x12FF;
        byte x_val2 = 2;
        word target2 = base2 + x_val2; // 0x1301
        cpu.SetRegisterA(0x01);
        cpu.SetRegisterX(x_val2);
        cpu.mem_write(target2, 0x80);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base2);
        cpu.ResetStatus();
        OpCode::op_ora(cpu, Absolute_X);
        assert(cpu.GetRegisterA() == 0x81);
        check_flags(cpu, false, true);
        std::cout << "[ORA] AbsoluteX test 2 (page cross) passed\n";
    }
}

// ---------- 绝对 Y 模式 ----------
static void test_ora_absolute_y() {
    word base = 0x1200;
    byte y_val = 0x34;
    word target = base + y_val;
    {
        CPU cpu;
        cpu.SetRegisterA(0x00);
        cpu.SetRegisterY(y_val);
        cpu.mem_write(target, 0x77);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base);
        cpu.ResetStatus();
        OpCode::op_ora(cpu, Absolute_Y);
        assert(cpu.GetRegisterA() == 0x77);
        check_flags(cpu, false, false);
        std::cout << "[ORA] AbsoluteY test 1 passed\n";
    }
}

// ---------- (间接,X) 模式 ----------
static void test_ora_indirect_x() {
    byte zp_base = 0x10;
    byte x_val = 5;
    word pointer = (zp_base + x_val) & 0xFF; // 0x15
    word target = 0x4321;
    CPU cpu;
    cpu.mem_write(pointer, target & 0xFF);
    cpu.mem_write(pointer + 1, target >> 8);
    cpu.mem_write(target, 0x12);
    cpu.SetRegisterA(0x00);
    cpu.SetRegisterX(x_val);
    cpu.SetPC(0x1000);
    cpu.mem_write(cpu.GetPC(), zp_base);
    cpu.ResetStatus();
    OpCode::op_ora(cpu, Indirect_X);
    assert(cpu.GetRegisterA() == 0x12);
    check_flags(cpu, false, false);
    std::cout << "[ORA] IndirectX test 1 passed\n";
}

// ---------- (间接),Y 模式 ----------
static void test_ora_indirect_y() {
    byte zp_base = 0x10;
    word base_addr = 0x1200;
    byte y_val = 0x34;
    word target = base_addr + y_val;
    CPU cpu;
    cpu.mem_write(zp_base, base_addr & 0xFF);
    cpu.mem_write(zp_base + 1, base_addr >> 8);
    cpu.mem_write(target, 0x20);
    cpu.SetRegisterA(0x10);
    cpu.SetRegisterY(y_val);
    cpu.SetPC(0x1000);
    cpu.mem_write(cpu.GetPC(), zp_base);
    cpu.ResetStatus();
    OpCode::op_ora(cpu,Indirect_D_Y);
    assert(cpu.GetRegisterA() == 0x30);
    check_flags(cpu, false, false);
    std::cout << "[ORA] IndirectY test 1 passed\n";
}

// ---------- 外部调用入口 ----------
void test_ora() {
    std::cout << "[ORA] Running tests...\n";
    test_ora_immediate();
    test_ora_zero_page();
    test_ora_zero_page_x();
    test_ora_absolute();
    test_ora_absolute_x();
    test_ora_absolute_y();
    test_ora_indirect_x();
    test_ora_indirect_y();
    std::cout << "[ORA] All tests passed.\n\n";
}