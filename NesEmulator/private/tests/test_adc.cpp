// test_adc.cpp
#include <cassert>
#include <iostream>
#include "../../public/cpu.h"
#include "../../public/op_code.h"

// 检查 C, Z, V, N 标志
static void check_flags(const CPU& cpu, bool carry, bool zero, bool overflow, bool negative) {
    byte s = cpu.GetStatus();
    assert(((s >> 0) & 1) == carry);
    assert(((s >> 1) & 1) == zero);
    assert(((s >> 6) & 1) == overflow);
    assert(((s >> 7) & 1) == negative);
}

// ------------------- 立即数模式 -------------------
static void test_adc_immediate() {
    // 测试1：无进位，0x10 + 0x20 = 0x30
    {
        CPU cpu;
        cpu.SetRegisterA(0x10);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG,false);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x20);
        OpCode::adc(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0x30);
        check_flags(cpu, false, false, false, false);
        std::cout << "[ADC] Immediate test 1 passed\n";
    }

    // 测试2：有进位，0x10 + 0x20 + 1 = 0x31
    {
        CPU cpu;
        cpu.SetRegisterA(0x10);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG,true);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x20);
        OpCode::adc(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0x31);
        check_flags(cpu, false, false, false, false);
        std::cout << "[ADC] Immediate test 2 passed\n";
    }

    // 测试3：进位产生，0xFF + 0x01 = 0x00, C=1, Z=1
    {
        CPU cpu;
        cpu.SetRegisterA(0xFF);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG,false);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x01);
        OpCode::adc(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0x00);
        check_flags(cpu, true, true, false, false);
        std::cout << "[ADC] Immediate test 3 passed\n";
    }

    // 测试4：负结果，0x01 + 0x7F = 0x80, N=1
    {
        CPU cpu;
        cpu.SetRegisterA(0x01);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG,false);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x7F);
        OpCode::adc(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0x80);
        check_flags(cpu, false, false, true, true);
        std::cout << "[ADC] Immediate test 4 passed\n";
    }

    // 测试5：正溢出，0x40 + 0x60 = 0xA0, V=1, N=1
    {
        CPU cpu;
        cpu.SetRegisterA(0x40);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG,false);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x60);
        OpCode::adc(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0xA0);
        check_flags(cpu, false, false, true, true);
        std::cout << "[ADC] Immediate test 5 passed\n";
    }

    // 测试6：负溢出，0x80 + 0x80 = 0x00, C=1, V=1, Z=1
    {
        CPU cpu;
        cpu.SetRegisterA(0x80);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG,false);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x80);
        OpCode::adc(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0x00);
        check_flags(cpu, true, true, true, false);
        std::cout << "[ADC] Immediate test 6 passed\n";
    }
}

// ------------------- 零页模式 -------------------
static void test_adc_zero_page() {
    word addr = 0x10;

    // 无进位
    {
        CPU cpu;
        cpu.SetRegisterA(0x10);
        cpu.mem_write(addr, 0x20);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG,false);
        OpCode::adc(cpu, ZeroPage);
        assert(cpu.GetRegisterA() == 0x30);
        check_flags(cpu, false, false, false, false);
        std::cout << "[ADC] ZeroPage test 1 passed\n";
    }

    // 有进位
    {
        CPU cpu;
        cpu.SetRegisterA(0x10);
        cpu.mem_write(addr, 0x20);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG,true);
        OpCode::adc(cpu, ZeroPage);
        assert(cpu.GetRegisterA() == 0x31);
        check_flags(cpu, false, false, false, false);
        std::cout << "[ADC] ZeroPage test 2 passed\n";
    }

    // 产生进位
    {
        CPU cpu;
        cpu.SetRegisterA(0xFF);
        cpu.mem_write(addr, 0x01);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG,false);
        OpCode::adc(cpu, ZeroPage);
        assert(cpu.GetRegisterA() == 0x00);
        check_flags(cpu, true, true, false, false);
        std::cout << "[ADC] ZeroPage test 3 passed\n";
    }
}

// ------------------- 零页 X 模式 -------------------
static void test_adc_zero_page_x() {
    // 普通：基址 0x10, X=5 -> 0x15
    {
        CPU cpu;
        cpu.SetRegisterA(0x10);
        cpu.SetRegisterX(5);
        word target = 0x15;
        cpu.mem_write(target, 0x20);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x10);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG,false);
        OpCode::adc(cpu, ZeroPage_X);
        assert(cpu.GetRegisterA() == 0x30);
        check_flags(cpu, false, false, false, false);
        std::cout << "[ADC] ZeroPageX test 1 passed\n";
    }

    // 绕回：0xFF + 1 = 0x00
    {
        CPU cpu;
        cpu.SetRegisterA(0x01);
        cpu.SetRegisterX(1);
        cpu.mem_write(0x00, 0x02);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0xFF);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG,false);
        OpCode::adc(cpu, ZeroPage_X);
        assert(cpu.GetRegisterA() == 0x03);
        check_flags(cpu, false, false, false, false);
        std::cout << "[ADC] ZeroPageX test 2 passed\n";
    }
}

// ------------------- 绝对模式 -------------------
static void test_adc_absolute() {
    word addr = 0x1234;

    {
        CPU cpu;
        cpu.SetRegisterA(0x10);
        cpu.mem_write(addr, 0x20);
        cpu.SetPC(0x2000);
        cpu.mem_write_u16(cpu.GetPC(), addr);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG,false);
        OpCode::adc(cpu, Absolute);
        assert(cpu.GetRegisterA() == 0x30);
        check_flags(cpu, false, false, false, false);
        std::cout << "[ADC] Absolute test 1 passed\n";
    }
}

// ------------------- 绝对 X 模式 -------------------
static void test_adc_absolute_x() {
    word base = 0x1200;
    byte x_val = 0x34;
    word target = base + x_val;

    {
        CPU cpu;
        cpu.SetRegisterA(0x10);
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0x20);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG,false);
        OpCode::adc(cpu, Absolute_X);
        assert(cpu.GetRegisterA() == 0x30);
        check_flags(cpu, false, false, false, false);
        std::cout << "[ADC] AbsoluteX test 1 passed\n";
    }
}

// ------------------- 绝对 Y 模式 -------------------
static void test_adc_absolute_y() {
    word base = 0x1200;
    byte y_val = 0x34;
    word target = base + y_val;

    {
        CPU cpu;
        cpu.SetRegisterA(0x10);
        cpu.SetRegisterY(y_val);
        cpu.mem_write(target, 0x20);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base);
        cpu.ResetStatus();
        cpu.SetFlag(CPU::CARRY_FLAG,false);
        OpCode::adc(cpu, Absolute_Y);
        assert(cpu.GetRegisterA() == 0x30);
        check_flags(cpu, false, false, false, false);
        std::cout << "[ADC] AbsoluteY test 1 passed\n";
    }
}

// ------------------- (间接,X) 模式 -------------------
static void test_adc_indirect_x() {
    byte zp_base = 0x10;
    byte x_val = 5;
    word pointer_addr = (zp_base + x_val) & 0xFF;  // 0x15
    word target_addr = 0x4321;

    CPU cpu;
    cpu.SetRegisterA(0x10);
    cpu.SetRegisterX(x_val);
    cpu.mem_write(pointer_addr, target_addr & 0xFF);
    cpu.mem_write(pointer_addr + 1, target_addr >> 8);
    cpu.mem_write(target_addr, 0x20);
    cpu.SetPC(0x1000);
    cpu.mem_write(cpu.GetPC(), zp_base);
    cpu.ResetStatus();
    cpu.SetFlag(CPU::CARRY_FLAG,false);
    OpCode::adc(cpu, Indirect_X);
    assert(cpu.GetRegisterA() == 0x30);
    check_flags(cpu, false, false, false, false);
    std::cout << "[ADC] IndirectX test 1 passed\n";
}

// ------------------- (间接),Y 模式 -------------------
static void test_adc_indirect_y() {
    byte zp_base = 0x10;
    word base_addr = 0x1200;
    byte y_val = 0x34;
    word target_addr = base_addr + y_val;

    CPU cpu;
    cpu.SetRegisterA(0x10);
    cpu.SetRegisterY(y_val);
    cpu.mem_write(zp_base, base_addr & 0xFF);
    cpu.mem_write(zp_base + 1, base_addr >> 8);
    cpu.mem_write(target_addr, 0x20);
    cpu.SetPC(0x1000);
    cpu.mem_write(cpu.GetPC(), zp_base);
    cpu.ResetStatus();
    cpu.SetFlag(CPU::CARRY_FLAG,false);
    OpCode::adc(cpu, Indirect_D_Y);
    assert(cpu.GetRegisterA() == 0x30);
    check_flags(cpu, false, false, false, false);
    std::cout << "[ADC] IndirectY test 1 passed\n";
}

// ------------------- 外部入口 -------------------
void test_adc() {
    std::cout << "[ADC] Running tests...\n";
    test_adc_immediate();
    test_adc_zero_page();
    test_adc_zero_page_x();
    test_adc_absolute();
    test_adc_absolute_x();
    test_adc_absolute_y();
    test_adc_indirect_x();
    test_adc_indirect_y();
    std::cout << "[ADC] All tests passed.\n\n";
}