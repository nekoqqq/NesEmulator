// test_lda.cpp
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
static void test_lda_immediate() {
    // 测试1：加载非零非负值
    {
        CPU cpu;
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x42);
        cpu.ResetStatus();
        OpCode::lda(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0x42);
        check_flags(cpu, false, false);
        std::cout << "[LDA] Immediate test 1 passed\n";
    }
    // 测试2：加载0，Z=1
    {
        CPU cpu;
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x00);
        cpu.ResetStatus();
        OpCode::lda(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0x00);
        check_flags(cpu, true, false);
        std::cout << "[LDA] Immediate test 2 passed\n";
    }
    // 测试3：加载负值（bit7=1），N=1
    {
        CPU cpu;
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0x80);
        cpu.ResetStatus();
        OpCode::lda(cpu, Immediate);
        assert(cpu.GetRegisterA() == 0x80);
        check_flags(cpu, false, true);
        std::cout << "[LDA] Immediate test 3 passed\n";
    }
}

// ---------- 零页模式 ----------
static void test_lda_zero_page() {
    word addr = 0x10;
    {
        CPU cpu;
        cpu.mem_write(addr, 0x55);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        OpCode::lda(cpu, ZeroPage);
        assert(cpu.GetRegisterA() == 0x55);
        check_flags(cpu, false, false);
        std::cout << "[LDA] ZeroPage test 1 passed\n";
    }
    {
        CPU cpu;
        cpu.mem_write(addr, 0x00);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), static_cast<byte>(addr));
        cpu.ResetStatus();
        OpCode::lda(cpu, ZeroPage);
        check_flags(cpu, true, false);
        std::cout << "[LDA] ZeroPage test 2 passed\n";
    }
}

// ---------- 零页 X 模式 ----------
static void test_lda_zero_page_x() {
    byte base = 0x10;
    byte x_val = 5;
    word target = (base + x_val) & 0xFF; // 0x15
    {
        CPU cpu;
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0xAA);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), base);
        cpu.ResetStatus();
        OpCode::lda(cpu, ZeroPage_X);
        assert(cpu.GetRegisterA() == 0xAA);
        check_flags(cpu, false, true);
        std::cout << "[LDA] ZeroPageX test 1 passed\n";
    }
    // 绕回测试：0xFF + 1 -> 0x00
    {
        CPU cpu;
        cpu.SetRegisterX(1);
        cpu.mem_write(0x00, 0x80);
        cpu.SetPC(0x1000);
        cpu.mem_write(cpu.GetPC(), 0xFF);
        cpu.ResetStatus();
        OpCode::lda(cpu, ZeroPage_X);
        assert(cpu.GetRegisterA() == 0x80);
        check_flags(cpu, false, true);
        std::cout << "[LDA] ZeroPageX test 2 (wrap) passed\n";
    }
}

// ---------- 绝对模式 ----------
static void test_lda_absolute() {
    word addr = 0x1234;
    {
        CPU cpu;
        cpu.mem_write(addr, 0x33);
        cpu.SetPC(0x2000);
        cpu.mem_write_u16(cpu.GetPC(), addr);
        cpu.ResetStatus();
        OpCode::lda(cpu, Absolute);
        assert(cpu.GetRegisterA() == 0x33);
        check_flags(cpu, false, false);
        std::cout << "[LDA] Absolute test 1 passed\n";
    }
}

// ---------- 绝对 X 模式 ----------
static void test_lda_absolute_x() {
    word base = 0x1200;
    byte x_val = 0x34;
    word target = base + x_val; // 0x1234
    {
        CPU cpu;
        cpu.SetRegisterX(x_val);
        cpu.mem_write(target, 0x7F);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base);
        cpu.ResetStatus();
        OpCode::lda(cpu, Absolute_X);
        assert(cpu.GetRegisterA() == 0x7F);
        check_flags(cpu, false, false);
        std::cout << "[LDA] AbsoluteX test 1 passed\n";
    }
    // 跨页测试（不影响结果，只验证地址正确）
    {
        CPU cpu;
        word base2 = 0x12FF;
        byte x_val2 = 2;
        word target2 = base2 + x_val2; // 0x1301
        cpu.SetRegisterX(x_val2);
        cpu.mem_write(target2, 0x01);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base2);
        cpu.ResetStatus();
        OpCode::lda(cpu, Absolute_X);
        assert(cpu.GetRegisterA() == 0x01);
        check_flags(cpu, false, false);
        std::cout << "[LDA] AbsoluteX test 2 (page cross) passed\n";
    }
}

// ---------- 绝对 Y 模式 ----------
static void test_lda_absolute_y() {
    word base = 0x1200;
    byte y_val = 0x34;
    word target = base + y_val;
    {
        CPU cpu;
        cpu.SetRegisterY(y_val);
        cpu.mem_write(target, 0x44);
        cpu.SetPC(0x3000);
        cpu.mem_write_u16(cpu.GetPC(), base);
        cpu.ResetStatus();
        OpCode::lda(cpu, Absolute_Y);
        assert(cpu.GetRegisterA() == 0x44);
        check_flags(cpu, false, false);
        std::cout << "[LDA] AbsoluteY test 1 passed\n";
    }
}

// ---------- (间接,X) 模式 ----------
static void test_lda_indirect_x() {
    byte zp_base = 0x10;
    byte x_val = 5;
    word pointer = (zp_base + x_val) & 0xFF; // 0x15
    word target = 0x4321;
    CPU cpu;
    cpu.mem_write(pointer, target & 0xFF);
    cpu.mem_write(pointer + 1, target >> 8);
    cpu.mem_write(target, 0x99);
    cpu.SetRegisterX(x_val);
    cpu.SetPC(0x1000);
    cpu.mem_write(cpu.GetPC(), zp_base);
    cpu.ResetStatus();
    OpCode::lda(cpu, Indirect_X);
    assert(cpu.GetRegisterA() == 0x99);
    check_flags(cpu, false, true);
    std::cout << "[LDA] IndirectX test 1 passed\n";
}

// ---------- (间接),Y 模式 ----------
static void test_lda_indirect_y() {
    byte zp_base = 0x10;
    word base_addr = 0x1200;
    byte y_val = 0x34;
    word target = base_addr + y_val;
    CPU cpu;
    cpu.mem_write(zp_base, base_addr & 0xFF);
    cpu.mem_write(zp_base + 1, base_addr >> 8);
    cpu.mem_write(target, 0x12);
    cpu.SetRegisterY(y_val);
    cpu.SetPC(0x1000);
    cpu.mem_write(cpu.GetPC(), zp_base);
    cpu.ResetStatus();
    OpCode::lda(cpu, Indirect_D_Y);
    assert(cpu.GetRegisterA() == 0x12);
    check_flags(cpu, false, false);
    std::cout << "[LDA] IndirectY test 1 passed\n";
}

// ---------- 外部调用入口 ----------
void test_lda() {
    std::cout << "[LDA] Running tests...\n";
    test_lda_immediate();
    test_lda_zero_page();
    test_lda_zero_page_x();
    test_lda_absolute();
    test_lda_absolute_x();
    test_lda_absolute_y();
    test_lda_indirect_x();
    test_lda_indirect_y();
    std::cout << "[LDA] All tests passed.\n\n";
}