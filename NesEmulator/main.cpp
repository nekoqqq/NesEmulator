/*
NES模拟器使用C++实现
*/

#include "public/cpu.h"

using std::cout;
using std::endl;

void test_asl();
void test_adc();
void test_and();
void test_bcc();
void test_bcs();
void test_beq();

int main()
{
    test_adc();
    test_asl();
    test_and();
    test_bcc();
    test_bcs();
    test_beq();
}
