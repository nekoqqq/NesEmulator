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
void test_bcc();

int main()
{
    test_adc();
    test_asl();
    test_and();
    test_bcc();
}
