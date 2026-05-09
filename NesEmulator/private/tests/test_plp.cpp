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
    // B 标志位不测试，因为 PLP 弹出的 B 可能被忽略
    assert(((status >> 6) & 1) == overflow);
    assert(((status >> 7) & 1) == negative);
}

static void test_plp_implied() {
    // 测试1：弹出一个值，恢复所有标志
    {
        CPU cpu;
        cpu.SetStackPointer(0xFD);
        cpu.mem_write(0x01FE, 0xCB); // 1100 1011
        cpu.ResetStatus();
        assert(cpu.GetStatus() == 0);
        OpCode::plp(cpu, Implied);
        assert(cpu.GetStackPointer() == 0xFE);
        // D 标志应为 true
        check_flags(cpu, true, true, false, true, true, true);
        std::cout << "[PLP] Test 1 (restore flags) passed\n";
    }

    // 测试2：弹出一个全0的字节，清除所有标志（原本有值也被清除）
    {
        CPU cpu;
        cpu.SetStackPointer(0xFC);
        cpu.mem_write(0x01FD, 0x00);
        cpu.ResetStatus();
        // 先设置一些标志，确保会被覆盖
        cpu.SetFlag(CPU::CARRY_FLAG, true);
        cpu.SetFlag(CPU::NEGATIVE_FLAG, true);
        OpCode::plp(cpu, Implied);
        assert(cpu.GetStackPointer() == 0xFD);
        check_flags(cpu, false, false, false, false, false, false);
        std::cout << "[PLP] Test 2 (clear all flags) passed\n";
    }

    // 测试3：弹出一个仅设置进位和零标志的字节
    {
        CPU cpu;
        cpu.SetStackPointer(0xFB);
        cpu.mem_write(0x01FC, 0x03); // 0000 0011 => C=1, Z=1
        cpu.ResetStatus();
        OpCode::plp(cpu, Implied);
        assert(cpu.GetStackPointer() == 0xFC);
        check_flags(cpu, true, true, false, false, false, false);
        std::cout << "[PLP] Test 3 (set C and Z) passed\n";
    }

    // 测试4：确保其他寄存器不受影响
    {
        CPU cpu;
        cpu.SetStackPointer(0xFA);
        cpu.mem_write(0x01FB, 0x00);
        cpu.SetRegisterA(0x55);
        cpu.SetRegisterX(0xAA);
        cpu.SetRegisterY(0x33);
        cpu.ResetStatus();
        OpCode::plp(cpu, Implied);
        assert(cpu.GetRegisterA() == 0x55);
        assert(cpu.GetRegisterX() == 0xAA);
        assert(cpu.GetRegisterY() == 0x33);
        assert(cpu.GetStackPointer() == 0xFB);
        std::cout << "[PLP] Test 4 (other registers unchanged) passed\n";
    }
}

void test_plp() {
    std::cout << "[PLP] Running tests...\n";
    test_plp_implied();
    std::cout << "[PLP] All tests passed.\n\n";
}