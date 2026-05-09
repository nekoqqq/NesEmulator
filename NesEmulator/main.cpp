/*
NES模拟器使用C++实现
*/
#include <cassert>

#include "public/cpu.h"
#include "public/op_code.h"

using std::cout;
using std::endl;

void test_asl();
void test_adc();
void test_and();

int main()
{
    int test_cnt = 1;
    // Test 1 LDA 立即数
    {
        CPU* cpu = new CPU();
        OpCode* opcode = new OpCode();
        cout << cpu << endl;
        vector<byte> test{0xa9, 0x05, 0x00};
        cpu->load_and_run(test);
        assert(cpu->GetRegisterA() == 0x05);
        assert((cpu->GetStatus() & 0x02) == 0); // &运算符优先级比==更加的低
        assert((cpu->GetStatus() & 0x80) == 0);
        cout << cpu << endl;
        cout << "Test passed: " << test_cnt++ << endl;
    }

    // Test 2 LDA 立即数
    {
        CPU* cpu = new CPU();
        cout << cpu << endl;
        vector<byte> test{0xa9, 0x00, 0x00};
        cpu->load_and_run(test);
        assert((cpu->GetStatus() & 0x02) == 0x02);
        cout << cpu << endl;
        cout << "Test passed: " << test_cnt++ << endl;
    }

    // Test 3 TAX 一个字节，包含了一个隐含的操作数
    {
        CPU* cpu = new CPU();
        cout << cpu << endl;
        cpu->SetRegisterA(10);
        vector<byte> test{0xaa, 0x00};
        cpu->load_and_run_no_reset(test);
        assert(cpu->GetRegisterX() == 10);
        cout << cpu << endl;
        cout << "Test passed: " << test_cnt++ << endl;
    }

    // Test 4 5个操作一起
    {
        CPU* cpu = new CPU();
        cout << cpu << endl;
        vector<byte> test{0xa9, 0xc0, 0xaa, 0xe8, 0x00};
        cpu->load_and_run_no_reset(test);
        assert(cpu->GetRegisterX() == 0xc1);
        cout << cpu << endl;
        cout << "Test passed: " << test_cnt++ << endl;
    }

    // Test 5 INX overflow
    {
        CPU* cpu = new CPU();
        cout << cpu << endl;
        cpu->SetRegisterX(0xff);
        vector<byte> test{0xe8, 0xe8, 0x00};
        cpu->load_and_run_no_reset(test);
        assert(cpu->GetRegisterX() == 0x01);
        cout << cpu << endl;
        cout << "Test passed: " << test_cnt++ << endl;
    }

    // Test 6 INY overflow
    {
        CPU* cpu = new CPU();
        cout << cpu << endl;
        cpu->SetRegisterY(0xff);
        vector<byte> test{0xc8, 0xc8, 0x00};
        cpu->load_and_run_no_reset(test);
        assert(cpu->GetRegisterY() == 0x01);
        cout << cpu << endl;
        cout << "Test passed: " << test_cnt++ << endl;
    }

    // Test 7 LDA
    {
        CPU* cpu = new CPU();
        cout << cpu << endl;
        vector<byte> program{0xa5, 0x10, 0x00};
        cpu->mem_write(0x10, 0x55);
        cpu->load_and_run(program);
        cout << cpu << endl;
        assert(cpu->GetRegisterA() == 0x55);
        cout << "Test passed: " << test_cnt++ << endl;
    }
    test_adc();
    test_asl();
    test_and();
}
