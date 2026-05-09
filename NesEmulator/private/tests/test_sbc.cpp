#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

static void check_flags(const CPU& cpu, bool carry, bool zero, bool overflow, bool negative) {
    byte status = cpu.GetStatus();
    assert(((status >> 0) & 1) == carry);
    assert(((status >> 1) & 1) == zero);
    assert(((status >> 6) & 1) == overflow);
    assert(((status >> 7) & 1) == negative);
}

// ---------- 立即数模式 ----------
static void test_sbc_immediate() {
    // 测试1：无借位，0x50 - 0x20 = 0x30
    {
        CPU cpu;
        cpu.SetRegisterA(0x50);
        cpu.SetFlag(CPU::CARRY_FLAG, true);   // C=1 表示无借位
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x20);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        OpCode::sbc(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0x30);
        check_flags(cpu, true, false, false, false);
        std::cout << "[SBC] Immediate test 1 passed\n";
    }
    // 测试2：有借位，0x20 - 0x30 = 0xF0, C=0, N=1
    {
        CPU cpu;
        cpu.SetRegisterA(0x20);
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x30);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        OpCode::sbc(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0xF0);
        check_flags(cpu, false, false, false, true);
        std::cout << "[SBC] Immediate test 2 passed\n";
    }
    // 测试3：产生0，C=1, Z=1
    {
        CPU cpu;
        cpu.SetRegisterA(0x20);
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x20);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        OpCode::sbc(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0x00);
        check_flags(cpu, true, true, false, false);
        std::cout << "[SBC] Immediate test 3 passed\n";
    }
    // 测试4：溢出，0x80 - 0x01 = 0x7F, C=1, V=1? 实际上 -128 - 1 = -129 超出8位有符号，产生溢出
    {
        CPU cpu;
        cpu.SetRegisterA(0x80);
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x01);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        OpCode::sbc(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0x7F);
        check_flags(cpu, true, false, true, false);
        std::cout << "[SBC] Immediate test 4 (overflow) passed\n";
    }
}

// ---------- 零页模式 ----------
static void test_sbc_zero_page() {
    word addr = 0x10;
    {
        CPU cpu;
        cpu.SetRegisterA(0x50);
        cpu.mem_write(addr, 0x20);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        OpCode::sbc(cpu, ZeroPage);
        assert(cpu.GetRegisterA() == 0x30);
        check_flags(cpu, true, false, false, false);
        std::cout << "[SBC] ZeroPage test 1 passed\n";
    }
}

// ---------- 零页 X 模式 ----------
static void test_sbc_zero_page_x() {
    byte base = 0x10;
    byte x_val = 5;
    word target = (base + x_val) & 0xFF;
    {
        CPU cpu;
        cpu.SetRegisterA(0x30);
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0x10);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), base);
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        OpCode::sbc(cpu, ZeroPage_X);
        assert(cpu.GetRegisterA() == 0x20);
        check_flags(cpu, true, false, false, false);
        std::cout << "[SBC] ZeroPageX test 1 passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_sbc_absolute() {
    word addr = 0x1234;
    {
        CPU cpu;
        cpu.SetRegisterA(0x40);
        cpu.mem_write(addr, 0x10);
        cpu.SetPC(0x2000);
        cpu.mem_write_u16(cpu.GetPC(), addr);
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        OpCode::sbc(cpu, Absolute);
        assert(cpu.GetRegisterA() == 0x30);
        check_flags(cpu, true, false, false, false);
        std::cout << "[SBC] Absolute test 1 passed\n";
    }
}

// ---------- 绝对 X 模式 ----------
static void test_sbc_absolute_x() {
    word base = 0x1200;
    byte x_val = 0x34;
    word target = base + x_val;
    {
        CPU cpu;
        cpu.SetRegisterA(0x50);
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0x20);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base);
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        OpCode::sbc(cpu, Absolute_X);
        assert(cpu.GetRegisterA() == 0x30);
        check_flags(cpu, true, false, false, false);
        std::cout << "[SBC] AbsoluteX test 1 passed\n";
    }
}

// ---------- 绝对 Y 模式 ----------
static void test_sbc_absolute_y() {
    word base = 0x1200;
    byte y_val = 0x34;
    word target = base + y_val;
    {
        CPU cpu;
        cpu.SetRegisterA(0x70);
        cpu.SetRegisterY(y_val);
        cpu.mem_write(target, 0x20);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base);
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        OpCode::sbc(cpu, Absolute_Y);
        assert(cpu.GetRegisterA() == 0x50);
        check_flags(cpu, true, false, false, false);
        std::cout << "[SBC] AbsoluteY test 1 passed\n";
    }
}

// ---------- (间接,X) 模式 ----------
static void test_sbc_indirect_x() {
    byte zp_base = 0x10;
    byte x_val = 5;
    word pointer = (zp_base + x_val) & 0xFF;
    word target = 0x4321;
    CPU cpu;
    cpu.mem_write(pointer, target & 0xFF);
    cpu.mem_write(pointer + 1, target >> 8);
    cpu.mem_write(target, 0x30);
    cpu.SetRegisterA(0x50);
    cpu.SetRegisterX(x_val);
    cpu.SetPC(0x1000);
    cpu.mem_write(cpu.GetPC(), zp_base);
    cpu.SetFlag(CPU::CARRY_FLAG, true);
    cpu.ResetStatus();
    cpu.SetFlag(CPU::CARRY_FLAG, true);
    OpCode::sbc(cpu, Indirect_X);
    assert(cpu.GetRegisterA() == 0x20);
    check_flags(cpu, true, false, false, false);
    std::cout << "[SBC] IndirectX test 1 passed\n";
}

// ---------- (间接),Y 模式 ----------
static void test_sbc_indirect_y() {
    byte zp_base = 0x10;
    word base_addr = 0x1200;
    byte y_val = 0x34;
    word target = base_addr + y_val;
    CPU cpu;
    cpu.mem_write(zp_base, base_addr & 0xFF);
    cpu.mem_write(zp_base + 1, base_addr >> 8);
    cpu.mem_write(target, 0x20);
    cpu.SetRegisterA(0x40);
    cpu.SetRegisterY(y_val);
    cpu.SetPC(0x1000);
    cpu.mem_write(cpu.GetPC(), zp_base);
    cpu.SetFlag(CPU::CARRY_FLAG, true);
    cpu.ResetStatus();
    cpu.SetFlag(CPU::CARRY_FLAG, true);
    OpCode::sbc(cpu, Indirect_D_Y);
    assert(cpu.GetRegisterA() == 0x20);
    check_flags(cpu, true, false, false, false);
    std::cout << "[SBC] IndirectY test 1 passed\n";
}

// ---------- 外部调用入口 ----------
void test_sbc() {
    std::cout << "[SBC] Running tests...\n";
    test_sbc_immediate();
    test_sbc_zero_page();
    test_sbc_zero_page_x();
    test_sbc_absolute();
    test_sbc_absolute_x();
    test_sbc_absolute_y();
    test_sbc_indirect_x();
    test_sbc_indirect_y();
    std::cout << "[SBC] All tests passed.\n\n";
}